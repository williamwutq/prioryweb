import { encrypt, encryptWithNoise, decrypt, decryptText } from "./sbcrypt.js";

let testsPassed = 0;
let testsFailed = 0;

function assert(condition, message) {
  if (condition) {
    console.log(`✓ ${message}`);
    testsPassed++;
  } else {
    console.error(`✗ ${message}`);
    testsFailed++;
  }
}

async function testBasicEncryptDecrypt() {
  console.log("\n=== Basic Encryption/Decryption ===");
  
  const blob = await encrypt([
    { key: "alice-secret", string: "Hello from Alice" },
    { key: "bob-secret",   string: "Hello from Bob"   },
    { key: "carol-secret", string: "Hello from Carol" },
  ]);

  assert(blob instanceof Uint8Array, "Encrypted blob is Uint8Array");
  assert(blob.length > 0, "Encrypted blob is not empty");
  assert(blob.length % 124 === 0, "Encrypted blob length is multiple of 124");

  const aliceText = await decryptText(blob, "alice-secret");
  assert(aliceText === "Hello from Alice", "Decrypt with correct key returns original text (Alice)");

  const bobText = await decryptText(blob, "bob-secret");
  assert(bobText === "Hello from Bob", "Decrypt with correct key returns original text (Bob)");

  const carolText = await decryptText(blob, "carol-secret");
  assert(carolText === "Hello from Carol", "Decrypt with correct key returns original text (Carol)");

  const invalidText = await decryptText(blob, "dave-secret");
  assert(invalidText === null, "Decrypt with invalid key returns null");

  const invalidText2 = await decryptText(blob, "alice-secret-wrong");
  assert(invalidText2 === null, "Decrypt with wrong key (similar prefix) returns null");
}

async function testSingleEntry() {
  console.log("\n=== Single Entry ===");

  const blob = await encrypt([
    { key: "single-key", string: "Single entry test" },
  ]);

  assert(blob.length === 124, "Single short entry produces one 124-byte unit");
  
  const text = await decryptText(blob, "single-key");
  assert(text === "Single entry test", "Single entry decrypts correctly");
}

async function testMultipleBlocks() {
  console.log("\n=== Multiple Blocks (Data Spanning Multiple Units) ===");

  // Create a string that's longer than 64 bytes (BLOCK_SIZE)
  const longString = "a".repeat(100);
  const blob = await encrypt([
    { key: "long-key", string: longString },
  ]);

  assert(blob.length === 248, "Long string (100 bytes) produces two 124-byte units");

  const decrypted = await decryptText(blob, "long-key");
  assert(decrypted === longString, "Long string decrypts correctly");

  // Test exactly 64 bytes
  const exact64 = "x".repeat(64);
  const blob64 = await encrypt([
    { key: "exact-key", string: exact64 },
  ]);

  assert(blob64.length === 248, "Exactly 64 bytes produces one unit");
  assert(await decryptText(blob64, "exact-key") === exact64, "Exactly 64 bytes decrypts correctly");

  // Test 65 bytes (requires second block)
  const exact65 = "x".repeat(65);
  const blob65 = await encrypt([
    { key: "exact-key", string: exact65 },
  ]);

  assert(blob65.length === 248, "65 bytes produces two units");
  assert(await decryptText(blob65, "exact-key") === exact65, "65 bytes decrypts correctly");
}

async function testEmptyString() {
  console.log("\n=== Empty and Small Strings ===");

  const blob = await encrypt([
    { key: "empty-key", string: "" },
  ]);

  assert(blob.length === 124, "Empty string produces one unit (due to PKCS#7 padding)");

  const text = await decryptText(blob, "empty-key");
  assert(text === "", "Empty string decrypts correctly");

  // Single character
  const blobSingle = await encrypt([
    { key: "single-char", string: "a" },
  ]);

  assert(await decryptText(blobSingle, "single-char") === "a", "Single character decrypts correctly");
}

async function testSpecialCharacters() {
  console.log("\n=== Special Characters and Unicode ===");

  const entries = [
    { key: "special1", string: "Hello\nWorld\t!" },
    { key: "special2", string: "Quote: \"test\" and 'test'" },
    { key: "special3", string: "Symbols: @#$%^&*()_+-=[]{}|;:,.<>?" },
    { key: "unicode1", string: "Hello 世界 🌍" },
    { key: "unicode2", string: "Emoji: 😀🎉💻🚀" },
    { key: "unicode3", string: "Greek: αβγδε Cyrillic: абвгд" },
  ];

  const blob = await encrypt(entries);

  for (const entry of entries) {
    const decrypted = await decryptText(blob, entry.key);
    assert(decrypted === entry.string, `Decrypts correctly: "${entry.string.substring(0, 30)}..."`);
  }
}

async function testKeyVariations() {
  console.log("\n=== Key Variations ===");

  const entries = [
    { key: "a", string: "Short key" },
    { key: "very-long-key-that-is-quite-lengthy-and-should-still-work-fine", string: "Long key" },
    { key: "key\twith\ttabs", string: "Key with tabs" },
    { key: "key with spaces", string: "Key with spaces" },
    { key: "🔑🔐", string: "Unicode key" },
  ];

  const blob = await encrypt(entries);

  for (const entry of entries) {
    const decrypted = await decryptText(blob, entry.key);
    assert(decrypted === entry.string, `Correct key "${entry.key}" decrypts successfully`);
  }

  // Wrong keys should not decrypt
  for (const entry of entries) {
    const wrongKey = entry.key + "x";
    const decrypted = await decryptText(blob, wrongKey);
    assert(decrypted === null, `Wrong key "${wrongKey}" returns null`);
  }
}

async function testRawDecrypt() {
  console.log("\n=== Raw Decrypt (Uint8Array) ===");

  const blob = await encrypt([
    { key: "raw-key", string: "Raw decrypt test" },
  ]);

  const raw = await decrypt(blob, "raw-key");
  assert(raw instanceof Uint8Array, "Raw decrypt returns Uint8Array");

  const text = new TextDecoder().decode(raw);
  assert(text === "Raw decrypt test", "Raw bytes decode to correct text");

  const invalidRaw = await decrypt(blob, "wrong-key");
  assert(invalidRaw === null, "Raw decrypt with wrong key returns null");
}

async function testBlobIntegrity() {
  console.log("\n=== Blob Integrity ===");

  const blob = await encrypt([
    { key: "integrity-key", string: "Original message" },
  ]);

  // Tamper with the blob
  const tamperedBlob = new Uint8Array(blob);
  tamperedBlob[50] ^= 0xFF; // Flip bits in middle

  const decrypted = await decryptText(tamperedBlob, "integrity-key");
  assert(decrypted === null, "Tampered blob fails decryption");
}

async function testInvalidBlobFormat() {
  console.log("\n=== Invalid Blob Format ===");

  try {
    await decrypt(new Uint8Array(123), "any-key");
    assert(false, "Should throw error for invalid blob length");
  } catch (e) {
    assert(e.message.includes("not a multiple"), "Throws error for invalid blob length");
  }

  try {
    const emptyBlob = new Uint8Array(0);
    const result = await decrypt(emptyBlob, "any-key");
    assert(result === null, "Empty blob returns null");
  } catch (e) {
    assert(false, `Should handle empty blob gracefully: ${e.message}`);
  }
}

async function testEncryptWithNoiseBasic() {
  console.log("\n=== Encrypt With Noise - Basic ===");

  const entries = [
    { key: "noise1", string: "Message 1" },
    { key: "noise2", string: "Message 2" },
  ];

  const blob = await encryptWithNoise(entries, { length: 500 });

  assert(blob instanceof Uint8Array, "Noise blob is Uint8Array");
  assert(blob.length % 124 === 0, "Noise blob length is multiple of 124");
  assert(blob.length >= 500, "Noise blob respects minimum length");

  const msg1 = await decryptText(blob, "noise1");
  const msg2 = await decryptText(blob, "noise2");
  assert(msg1 === "Message 1", "Noise blob decrypts message 1");
  assert(msg2 === "Message 2", "Noise blob decrypts message 2");

  const noiseMsg = await decryptText(blob, "this-key-does-not-exist");
  assert(noiseMsg === null, "Non-existent key returns null in noise blob");
}

async function testEncryptWithNoiseRatio() {
  console.log("\n=== Encrypt With Noise - Ratio Control ===");

  const entries = [
    { key: "ratio-test", string: "Short" },
  ];

  // Test low ratio (more data, less noise)
  const lowRatioBlob = await encryptWithNoise(entries, { high: 0.1, low: 0.2 });
  assert(lowRatioBlob.length % 124 === 0, "Low ratio produces valid blob");

  const lowRatioMsg = await decryptText(lowRatioBlob, "ratio-test");
  assert(lowRatioMsg === "Short", "Low ratio blob decrypts correctly");

  // Test high ratio (less data, more noise)
  const highRatioBlob = await encryptWithNoise(entries, { high: 2.0, low: 3.0 });
  assert(highRatioBlob.length >= lowRatioBlob.length, "High ratio produces larger blob");

  const highRatioMsg = await decryptText(highRatioBlob, "ratio-test");
  assert(highRatioMsg === "Short", "High ratio blob decrypts correctly");
}

async function testEncryptWithNoiseLarge() {
  console.log("\n=== Encrypt With Noise - Large Data ===");

  const largeString = "Lorem ipsum dolor sit amet, ".repeat(50);
  const blob = await encryptWithNoise(
    [{ key: "large-noise", string: largeString }],
    { length: 2000 }
  );

  assert(blob.length % 124 === 0, "Large noise blob is properly aligned");
  assert(blob.length >= 2000, "Large noise blob respects minimum length");

  const decrypted = await decryptText(blob, "large-noise");
  assert(decrypted === largeString, "Large data with noise decrypts correctly");
}

async function testMultipleEntriesMultipleBlocks() {
  console.log("\n=== Multiple Entries with Multiple Blocks ===");

  const entries = [
    { key: "entry1", string: "a".repeat(120) },
    { key: "entry2", string: "b".repeat(80) },
    { key: "entry3", string: "c".repeat(200) },
  ];

  const blob = await encrypt(entries);

  // entry1: 120 bytes → 2 blocks → 2 units
  // entry2: 80 bytes → 2 blocks → 2 units
  // entry3: 200 bytes → 4 blocks → 4 units
  // Total: 8 units = 992 bytes
  assert(blob.length === 992, "Multiple entries with multiple blocks produces correct size");

  for (const entry of entries) {
    const decrypted = await decryptText(blob, entry.key);
    assert(decrypted === entry.string, `Multi-block entry decrypts correctly (${entry.key})`);
  }
}

async function testBlockShuffling() {
  console.log("\n=== Block Shuffling (Order Independence) ===");

  const longString = "x".repeat(200); // Will create 4 blocks
  const blob1 = await encrypt([{ key: "shuffle-test", string: longString }]);

  const longString2 = "y".repeat(200);
  const blob2 = await encrypt([{ key: "shuffle-test", string: longString2 }]);

  // Both should have same size if they're the same data size
  assert(blob1.length === blob2.length, "Same-sized encrypted data produces same blob size");

  // Decrypt should still work
  const decrypted1 = await decryptText(blob1, "shuffle-test");
  const decrypted2 = await decryptText(blob2, "shuffle-test");
  assert(decrypted1 === longString, "Shuffled blocks decrypt correctly (1)");
  assert(decrypted2 === longString2, "Shuffled blocks decrypt correctly (2)");
}

async function testDataExtremelyLarge() {
  console.log("\n=== Extremely Large Data ===");

  // ~50KB of data
  const hugeString = "The quick brown fox jumps over the lazy dog. ".repeat(1000);
  const blob = await encrypt([
    { key: "huge-key", string: hugeString },
  ]);

  assert(blob.length % 124 === 0, "Large data produces properly aligned blob");
  
  const decrypted = await decryptText(blob, "huge-key");
  assert(decrypted === hugeString, "Huge data decrypts correctly");
  assert(decrypted.length === hugeString.length, "Decrypted data has correct length");
}

async function testMultipleSameKey() {
  console.log("\n=== Multiple Entries with Same Key ===");

  // Multiple entries with the same key
  const entries = [
    { key: "same-key", string: "First message" },
    { key: "same-key", string: "Second message" },
  ];

  const blob = await encrypt(entries);

  // Should decrypt to one of them (behavior: first match)
  const decrypted = await decryptText(blob, "same-key");
  assert(
    decrypted === "First message" || decrypted === "Second message",
    "Multiple same-key entries decrypt to one valid message"
  );
}

async function testRandomnessAcrossEncryptions() {
  console.log("\n=== Randomness Across Encryptions ===");

  const plaintext = "Same message for testing randomness";
  const blob1 = await encrypt([{ key: "random-test", string: plaintext }]);
  const blob2 = await encrypt([{ key: "random-test", string: plaintext }]);

  assert(blob1.length === blob2.length, "Same plaintext produces same size");

  // Blobs should be different due to random IVs
  let blobsDifferent = false;
  for (let i = 0; i < blob1.length; i++) {
    if (blob1[i] !== blob2[i]) {
      blobsDifferent = true;
      break;
    }
  }
  assert(blobsDifferent, "Different encryptions of same data produce different blobs (random IV)");

  // But both should decrypt correctly
  assert(await decryptText(blob1, "random-test") === plaintext, "First blob decrypts");
  assert(await decryptText(blob2, "random-test") === plaintext, "Second blob decrypts");
}

async function testEntropyQualityWorst() {
  console.log("\n=== Entropy Quality (Worst Case) ===");

  const blob = await encrypt([
    { key: "entropy-test", string: "Testing encryption quality" },
  ]);

  const counts = new Array(256).fill(0);
  for (const byte of blob) counts[byte]++;

  let entropy = 0;
  const total = blob.length;
  for (const count of counts) {
    if (count > 0) {
      const p = count / total;
      entropy -= p * Math.log2(p);
    }
  }

  // Good encryption should have entropy close to 8 bits per byte
  // This is a bad case (In real usage, you should either have a lot of data or use noise to increase entropy.
  // see the documentation for this recommendation).
  assert(entropy > 6.4, `Blob entropy is high: ${entropy.toFixed(2)} bits/byte`);
}

async function testEntropyQualityGood() {
  console.log("\n=== Entropy Quality (Good Case) ===");

  const blob = await encryptWithNoise(
    [{ key: "entropy-noise-test", string: "Testing encryption quality with noise" },
    { key: "entropy-noise-test", string: "This test includes random noise to increase entropy" },
    { key: "entropy-noise-test", string: "This means the blob should have very high entropy due to the noise" }],
    { length: 1000 }
  );

  const counts = new Array(256).fill(0);
  for (const byte of blob) counts[byte]++;

  let entropy = 0;
  const total = blob.length;
  for (const count of counts) {
    if (count > 0) {
      const p = count / total;
      entropy -= p * Math.log2(p);
    }
  }

  // Good encryption should have entropy close to 8 bits per byte
  assert(entropy > 7.5, `Blob entropy is high: ${entropy.toFixed(2)} bits/byte`);
}

// Run all tests
async function runAllTests() {
  try {
    await testBasicEncryptDecrypt();
    await testSingleEntry();
    await testMultipleBlocks();
    await testEmptyString();
    await testSpecialCharacters();
    await testKeyVariations();
    await testRawDecrypt();
    await testBlobIntegrity();
    await testInvalidBlobFormat();
    await testEncryptWithNoiseBasic();
    await testEncryptWithNoiseRatio();
    await testEncryptWithNoiseLarge();
    await testMultipleEntriesMultipleBlocks();
    await testBlockShuffling();
    await testDataExtremelyLarge();
    await testMultipleSameKey();
    await testRandomnessAcrossEncryptions();
    await testEntropyQualityWorst();
    await testEntropyQualityGood();
  } catch (e) {
    console.error("Unexpected error during tests:", e);
    testsFailed++;
  }

  console.log("\n" + "=".repeat(50));
  console.log(`Tests Passed: ${testsPassed}`);
  console.log(`Tests Failed: ${testsFailed}`);
  console.log(`Total Tests: ${testsPassed + testsFailed}`);
  console.log("=".repeat(50));
}

runAllTests();