/**
 * HMAC-Keyed Multi-String Encryption Scheme
 * Browser ES module — depends only on the Web Crypto API (window.crypto.subtle)
 *
 * Block size: 64 bytes of plaintext per unit
 * Unit layout: [ IV (12) | AES-GCM ciphertext [ TAG (32) | DATA (64) ] | GCM auth (16) ] = 124 bytes
 */

const BLOCK_SIZE   = 64;   // bytes of plaintext data per block
const TAG_SIZE     = 32;   // HMAC-SHA256 output = 256-bit tag
const IV_SIZE      = 12;   // AES-GCM nonce
const GCM_TAG_SIZE = 16;   // AES-GCM authentication tag appended by encrypt()
const UNIT_SIZE    = IV_SIZE + TAG_SIZE + BLOCK_SIZE + GCM_TAG_SIZE; // 124 bytes

const subtle = globalThis.crypto.subtle;

/** Import a raw key string as an HKDF key. */
async function importRawKey(keyString) {
  const raw = new TextEncoder().encode(keyString);
  return subtle.importKey("raw", raw, { name: "HKDF" }, false, ["deriveKey", "deriveBits"]);
}

/** Derive a subkey using HKDF with the given info string and usage. */
async function deriveSubkey(hkdfKey, info, usage, algorithm) {
  const infoBytes = new TextEncoder().encode(info);
  return subtle.deriveKey(
    { name: "HKDF", hash: "SHA-256", salt: new Uint8Array(0), info: infoBytes },
    hkdfKey,
    algorithm,
    false,
    usage
  );
}

/** Derive encryption and HMAC keys from the input key string. */
async function deriveKeys(keyString) {
  const hkdf = await importRawKey(keyString);
  const [encKey, hmacKey] = await Promise.all([
    deriveSubkey(hkdf, "enc",  ["encrypt", "decrypt"], { name: "AES-GCM", length: 256 }),
    deriveSubkey(hkdf, "hmac", ["sign"],               { name: "HMAC", hash: "SHA-256", length: 256 }),
  ]);
  return { encKey, hmacKey };
}

/** base[j] = HMAC(Khmac[j], "base") — 32 bytes */
async function computeBase(hmacKey) {
  const data = new TextEncoder().encode("base");
  const sig  = await subtle.sign("HMAC", hmacKey, data);
  return new Uint8Array(sig);
}

/** tag = (base as uint256) + index, big-endian 32 bytes */
function addIndex(base32, index) {
  const tag = new Uint8Array(base32);
  let carry = index;
  for (let i = 31; i >= 0 && carry > 0; i--) {
    const sum = tag[i] + (carry & 0xff);
    tag[i]    = sum & 0xff;
    carry     = (carry >>> 8) + (sum >>> 8);
  }
  return tag;
}

/** result = (a as uint256) - (b as uint256), returns Number or -1 if negative / too large */
function subBase(tagBytes, base32) {
  let borrow = 0;
  const result = new Uint8Array(32);
  for (let i = 31; i >= 0; i--) {
    let diff = tagBytes[i] - base32[i] - borrow;
    if (diff < 0) { diff += 256; borrow = 1; } else { borrow = 0; }
    result[i] = diff;
  }
  if (borrow !== 0) return -1; // negative
  // Only accept indices that fit in a safe integer
  for (let i = 0; i < 28; i++) if (result[i] !== 0) return -1; // way too large
  return (result[28] * 0x1000000) + (result[29] << 16) + (result[30] << 8) + result[31];
}

/**
 * Pad a byte array using PKCS#7 padding to a multiple of blockSize.
 * @param {Uint8Array} bytes The plaintext bytes to pad
 * @param {number} blockSize The block size in bytes (e.g., 64)
 * @returns {Uint8Array} padded The padded plaintext bytes, with length a multiple of blockSize
 */
function pkcs7Pad(bytes, blockSize) {
  const pad = blockSize - (bytes.length % blockSize);
  const out  = new Uint8Array(bytes.length + pad);
  out.set(bytes);
  out.fill(pad, bytes.length);
  return out;
}

/**
 * Remove PKCS#7 padding from a byte array.
 * @param {Uint8Array} bytes The padded plaintext bytes
 * @returns {Uint8Array} unpadded The original plaintext bytes with padding removed
 * @throws {Error} if the input is empty or has invalid padding
 */
function pkcs7Unpad(bytes) {
  if (bytes.length === 0) throw new Error("Empty input");
  const pad = bytes[bytes.length - 1];
  if (pad < 1 || pad > BLOCK_SIZE) throw new Error("Invalid padding");
  return bytes.slice(0, bytes.length - pad);
}

/**
 * Shuffle an array in place using the Fisher-Yates algorithm and crypto.getRandomValues() for randomness.
 * 
 * Algorithm in use is the Fisher-Yates shuffle, which iterates through the array from the last element to the first,
 * swapping each element with a randomly selected earlier element (or itself).
 * The randomness is sourced from crypto.getRandomValues() to ensure cryptographic quality randomness.
 * @param {Array} arr The array to shuffle
 */
function shuffleInPlace(arr) {
  const rng = new Uint32Array(1);
  for (let i = arr.length - 1; i > 0; i--) {
    globalThis.crypto.getRandomValues(rng);
    const j = rng[0] % (i + 1);
    [arr[i], arr[j]] = [arr[j], arr[i]];
  }
}

/**
 * Encrypt an array of { key, string } entries.
 * Returns a Uint8Array blob of M × 124-byte units in random order.
 *
 * @param {Array<{ key: string, string: string }>} entries
 * @returns {Promise<Uint8Array>}
 */
export async function encrypt(entries) {
  const encoder = new TextEncoder();
  const units   = [];

  for (const { key, string } of entries) {
    const { encKey, hmacKey } = await deriveKeys(key);
    const base    = await computeBase(hmacKey);
    const padded  = pkcs7Pad(encoder.encode(string), BLOCK_SIZE);
    const nBlocks = padded.length / BLOCK_SIZE;

    for (let i = 0; i < nBlocks; i++) {
      const tag      = addIndex(base, i);
      const data     = padded.subarray(i * BLOCK_SIZE, (i + 1) * BLOCK_SIZE);
      const plain    = new Uint8Array(TAG_SIZE + BLOCK_SIZE);
      plain.set(tag,  0);
      plain.set(data, TAG_SIZE);

      const iv         = globalThis.crypto.getRandomValues(new Uint8Array(IV_SIZE));
      const cipherBuf  = await subtle.encrypt({ name: "AES-GCM", iv }, encKey, plain);
      const cipher     = new Uint8Array(cipherBuf);

      const unit = new Uint8Array(UNIT_SIZE);
      unit.set(iv,     0);
      unit.set(cipher, IV_SIZE);
      units.push(unit);
    }
  }

  shuffleInPlace(units);

  const blob = new Uint8Array(units.length * UNIT_SIZE);
  units.forEach((u, i) => blob.set(u, i * UNIT_SIZE));
  return blob;
}

/**
 * Encrypt with additional random noise entry to obfuscate the length and distribution of the real entries.
 * The noise entry is generated with a random key and a random string of length proportional to the total length of the real entries.
 * The proportion of noise can be adjusted with the noise parameter,
 * which has "high" and "low" values or "length" field to control the ratio of noise to real data.
 *
 * @param {Array<{ key: string, string: string }>} entries The real entries to encrypt
 * @param {{ high: number, low: number, length?: number }} noise Optional parameter to control the proportion of noise added
 *     It is recommended to set this to custom values to avoid length based attacks, but the default is a reasonable range for typical use cases.
 * @param {number} [noise.high] The upper bound of the noise ratio (e.g., 0.4 means noise can be up to 40% of the total length of real entries)
 * @param {number} [noise.low] The lower bound of the noise ratio (e.g., 0.6 means noise can be at least 60% of the total length of real entries)
 * @param {number} [noise.length] The fixed length of the noise entry, if specified. This will override the high/low ratio if provided.
 * @returns {Promise<Uint8Array>} A Uint8Array blob of encrypted data containing both the real entries and the noise entry, in random order.
 * @example
 * // Ecrypt to a specific proportion of noise (e.g., between 30% and 50% of the total length of real entries)
 * // This is useful if the number of real entries is small and variable.
 * // In general, if the number of real entries is small, the variance and size of the noise should be larger to better obfuscate the real entries.
 * const blob = await encryptWithNoise([
 *   { key: "alice-secret", string: "Hello from Alice" },
 *   { key: "bob-secret",   string: "Hello from Bob"   },
 *   { key: "carol-secret", string: "Hello from Carol" },
 * ], { high: 0.3, low: 0.8 });
 * 
 * // Encrypt with a fixed length of noise (e.g., 1024 bytes)
 * // This is useful if the number of real entries is large and relatively consistent, or if you want to enforce a certain blob size for other reasons.
 * // It is worth noting that fixed blob sizes are secure against length based attacks, but are less flexible.
 * const blob = await encryptWithNoise([
 *   { key: "alice-secret", string: "Hello from Alice" },
 *   { key: "bob-secret",   string: "Hello from Bob"   },
 *   { key: "carol-secret", string: "Hello from Carol" },
 * ], { length: 1024 });
 */
export async function encryptWithNoise(entries, noise = {"high": 0.4, "low": 0.6}) {
  // Handle entry being a single pair of key and string, or an array of them
  if (!Array.isArray(entries)) {
    entries = [entries];
  }
  // Create also a random string and a random key to add noise
  // SAFETY:
  // We can use Math.random() here because the noise is just to obfuscate the length and distribution of the real entries,
  // and does not need to be cryptographically secure. The security of the scheme relies on the encryption and HMAC,
  // not on the randomness of the noise.
  const totalLength = entries.reduce((sum, { string }) => sum + string.length, 0);
  const noiseLength = // If noise has "length" property, use it as fixed length
    noise.length !== undefined ? noise.length :
    Math.floor(totalLength * (Math.random() * (noise.high - noise.low) + noise.low));
  const noiseString = Array.from({ length: noiseLength }, () => String.fromCharCode(32 + Math.floor(Math.random() * 95))).join("");
  const noiseKey    = Array.from({ length: 16 }, () => String.fromCharCode(32 + Math.floor(Math.random() * 95))).join("");

  const allEntries = [...entries, { key: noiseKey, string: noiseString }];
  return encrypt(allEntries);
}

/**
 * Decrypt a blob produced by encrypt(), returning the string that belongs to key.
 * Returns a Uint8Array of the raw plaintext bytes, or null if nothing matched.
 *
 * @param {Uint8Array} blob
 * @param {string}     key
 * @returns {Promise<Uint8Array|null>}
 */
export async function decrypt(blob, key) {
  if (blob.length % UNIT_SIZE !== 0) throw new Error("Blob length is not a multiple of unit size");

  const { encKey, hmacKey } = await deriveKeys(key);
  const base    = await computeBase(hmacKey);
  const nUnits  = blob.length / UNIT_SIZE;
  const blocks  = new Map(); // index map

  for (let u = 0; u < nUnits; u++) {
    const unit = blob.subarray(u * UNIT_SIZE, (u + 1) * UNIT_SIZE);
    const iv   = unit.subarray(0, IV_SIZE);
    const ciph = unit.subarray(IV_SIZE);

    let plain;
    try {
      const buf = await subtle.decrypt({ name: "AES-GCM", iv }, encKey, ciph);
      plain = new Uint8Array(buf);
    } catch {
      continue; // GCM auth failed — not our block
    }

    const tag   = plain.subarray(0, TAG_SIZE);
    const data  = plain.subarray(TAG_SIZE);
    const index = subBase(tag, base);
    if (index < 0) continue;

    blocks.set(index, data);
  }

  if (blocks.size === 0) return null;

  // Find max continuous run from 0
  let len = 0;
  while (blocks.has(len)) len++;
  if (len === 0) return null;

  const padded = new Uint8Array(len * BLOCK_SIZE);
  for (let i = 0; i < len; i++) padded.set(blocks.get(i), i * BLOCK_SIZE);

  return pkcs7Unpad(padded);
}

/**
 * Convenience wrapper: decrypt and return a UTF-8 string.
 *
 * @param {Uint8Array} blob
 * @param {string}     key
 * @returns {Promise<string|null>}
 */
export async function decryptText(blob, key) {
  const bytes = await decrypt(blob, key);
  return bytes ? new TextDecoder().decode(bytes) : null;
}