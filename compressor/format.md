# Data Compression Format

This document describes the data compression format used by the compressor library. The format is designed to efficiently store and transmit data while minimizing size and maximizing speed.

## Information Structure

The compressed data is structured as follows:
- Header: Contains metadata about the compressed data, including version information, data type, and flags. This is stored in the first 2 bytes.
- Keyword Length: 10 bits indicating the length of the keyword field that follows the header. This is an unsigned integer value, which means the longest possible keyword is 1023*5 = 5115 bits (639 bytes), representing 1022 characters plus a null terminator. No tagged resource should be this long.
- Keyword: A variable-length field containing a keyword that describes the data. The length of this field is specified by the Keyword Length byte. Since keywords are lowercase, they are encoded in 5 bits per character. (See: "Base 32 Format for Lowercase Letters" section below.)
- Compressed Data: The actual compressed data follows the keyword field. Since data are all ASCII characters, a variant of base 64 encoding is used to optimize space. (See: "Special Base 64 Encoding for ASCII Data" section in the compressor documentation.)
- Footer: The last byte contains a checksum for data integrity verification.

## Header Format
The header is 2 bytes long and is structured as follows:
- Byte 1:
  - Bits 0-3: Version (3 bits) - Indicates the version of the compression format.
  - Bits 4-7: Data Type (4 bits) - Specifies the type of data being compressed:
    - 0: Text and General
    - 1: Page URL
    - 2: Resource URL
    - 3: File URL
    - 4: File Data
    - 5: Formated Data Entry
    - 6: Formated Person
    - 7: Formated Event
    - 8-15: Reserved for future use (In deed, the 4th bit is always 0 for now, but no parser should assume that.)
- Byte 2:
  - Bits 0-7: Flags (8 bits) - Various flags that modify the behavior of the compression:
    - Bit 0: True
    - Bit 1: Compression Enabled (1 bit) - Indicates whether compression is enabled (1) or disabled (0).
    - Bit 2: Public Resource (1 bit) - Indicates whether the resource is public (1) or private (0).
    - Bit 3: Local Resource (1 bit) - Indicates whether the resource is local (1) or remote (0).
    - Bit 4: Dynamic Entry (1 bit) - Indicates whether the entry is dynamic (1) or static (0). Most entries are static.
    - Bit 5: Disable Cache (1 bit) - Indicates whether the searcher should not cache this resource (1) or can cache it (0). Parsers should not assume this bit is always 0, or it always equals to "dynamic entry".
    - Bits 6: Is Trusted (1 bit) - Indicates whether the resource is from a trusted source (1) or not (0). Parsers can ignore this bit for normal purposes, as modern formats include 'trusted: true/false' in the data itself.
    - Bit 7: Reserved (1 bit) - Reserved for future use. Parsers should ignore this bit.

## Base 32 Format for Lowercase Letters
To efficiently encode lowercase letters (a-z) in the keyword field, we use a base 32 encoding scheme. Each lowercase letter is represented using 5 bits, allowing us to pack multiple characters into fewer bytes. The mapping is as follows:
- 0b00000 - null (reserved, also used as terminator), or terminate and start new sequence if preceded by escape (Never used in keywords)
- 0b00001 - '\' (escape, for numbers and special characters), or '\' if preceded by escape
- 0b00010 - ' ' (space), or '\t' (tab) if preceded by escape
- 0b00011 - '\n\r' (newline), or '&nbsp;' if preceded by escape
- 0b00100 - '|' (separator), or starting unicode sequence if preceded by escape (In this case, the next 5 bits indicate the length of the unicode sequence in bytes, followed by unicde data encoded in digits (a-j for 0-9).)
- 0b00101 - 'a', or '0' if preceded by escape
- 0b00110 - 'b', or '1' if preceded by escape
- 0b00111 - 'c', or '2' if preceded by escape
- 0b01000 - 'd', or '3' if preceded by escape
- 0b01001 - 'e', or '4' if preceded by escape
- 0b01010 - 'f', or '5' if preceded by escape
- 0b01011 - 'g', or '6' if preceded by escape
- 0b01100 - 'h', or '7' if preceded by escape
- 0b01101 - 'i', or '8' if preceded by escape
- 0b01110 - 'j', or '9' if preceded by escape
- 0b01111 - 'k', or '.' if preceded by escape
- 0b10000 - 'l', or ',' if preceded by escape
- 0b10001 - 'm', or '-' if preceded by escape
- 0b10010 - 'n', or '_' if preceded by escape
- 0b10011 - 'o', or '/' if preceded by escape
- 0b10100 - 'p', or ':' if preceded by escape
- 0b10101 - 'q', or ';' if preceded by escape
- 0b10110 - 'r', or '?' if preceded by escape
- 0b10111 - 's', or '!' if preceded by escape
- 0b11000 - 't', or '@' if preceded by escape
- 0b11001 - 'u', or '#' if preceded by escape
- 0b11010 - 'v', or '$' if preceded by escape
- 0b11011 - 'w', or '+' if preceded by escape
- 0b11100 - 'x', or '=' if preceded by escape
- 0b11101 - 'y', or '*' if preceded by escape
- 0b11110 - 'z', or '&' if preceded by escape
- 0b11111 - (reserved)

Behavior of Padding:
- If the final group of data does not make a full byte, it is padded with binary zeros to make a full byte before encoding.

## Base 32 Format Encoding Example
To encode the keyword list 'data', '123', and 'hello world', which has string representation of 'data|123|hello world', we would proceed as follows:
1. Calculate the length of the keyword string:
   - 'data|123|hello world' has 20 characters.
   - The null terminator is included in the length, making it 21.
2. The Keyword Length would thus be 21, in binary: 0b0000010101.
3. Convert each character to its corresponding 5-bit representation using the mapping above:
    - 'd' -> 0b01000
    - 'a' -> 0b00101
    - 't' -> 0b11000
    - 'a' -> 0b00101
    - '|' -> 0b00100
    - '1' -> 0b00001 + 0b00110
    - '2' -> 0b00001 + 0b00111
    - '3' -> 0b00001 + 0b01000
    - '|' -> 0b00100
    - 'h' -> 0b01100
    - 'e' -> 0b01001
    - 'l' -> 0b10000
    - 'l' -> 0b10000
    - 'o' -> 0b10011
    - ' ' -> 0b00010
    - 'w' -> 0b11011
    - 'o' -> 0b10011
    - 'r' -> 0b10110
    - 'l' -> 0b10000
    - 'd' -> 0b01000
    - null terminator -> 0b00000
4. Concatenate the 5-bit values to form a bitstream together with the Keyword Length.
In 5 bit groups, the bitstream would look like this:
```00000 10101 01000 00101 11000 00101 00100 00001 00110 00001 00111 00001 01000 00100 01100 01001 10000 10000 10011 00010 11011 10011 10110 10000 01000 00000```
5. Finally, pack the bitstream into bytes for storage or transmission.
In bytes, the packed data would be:
```0b00000101 0b01010000 0b10111000 0b00101001 0b10000010 0b01100000 0b10011100 0b00100011 0b00100110 0b11001110 0b11010000 0b10000000```

## Special Base 64 Encoding for ASCII Data
The compressed data section uses a variant of base 64 encoding optimized for ASCII characters. Each group of 3 bytes (24 bits) of data is divided into 4 groups of 6 bits each. Each 6-bit group is then mapped to a printable ASCII character using the following mapping:
- 0-25: 'A'-'Z'
- 26-51: 'a'-'z'
- 52-61: '0'-'9'
- 62: ':' (assignment), or '|' (separator) if preceded by escape
- 63: '\' (escape), or terminator if preceded by escape

Special escape sequence:
- Whitespaces (space, tab, newline) are represented using the escape character followed by a specific code:
  - ' ' (space): escape + s
  - '\t' (tab): escape + t
  - '\n' (newline): escape + n
  - '\r' (carriage return): escape + r
  - '&nbsp;': escape + p

- Special characters (.,-_/ etc.) are also represented using the escape character followed by a specific code:
  - '.' : escape + A
  - ',' : escape + B
  - '-' : escape + C
  - '_' : escape + D
  - '/' : escape + E
  - ':' : escape + F
  - ';' : escape + G
  - '?' : escape + H
  - '!' : escape + I
  - '@' : escape + J
  - '#' : escape + K
  - '$' : escape + L
  - '+' : escape + M
  - '=' : escape + N
  - '*' : escape + O
  - '&' : escape + P
  - '%' : escape + Q
  - '^' : escape + R
  - '(' : escape + S
  - ')' : escape + T
  - '[' : escape + U
  - ']' : escape + V
  - '{' : escape + W
  - '}' : escape + X
  - '<' : escape + Y
  - '>' : escape + Z
  - '\' : escape + 0
  - '|' : escape + 1
  - '~' : escape + 2
  - '`' : escape + 3
  - '"' : escape + 4
  - ''' : escape + 5

- Special Code can be used indicate the start of a sequence. We have:
  - Unicode Sequence: escape + u, followed by the length of the unicode sequence in characters, followed by unicode data encoded in standard Base64 format. For example, the unicode string "你好" would be encoded as escape + u, length 8 (since each character is 3 bytes in UTF-8, summing to 48 bits or 8 Base64 characters), followed by the Base64 representation of the UTF-8 bytes.
  - Raw ASCII Sequence: escape + a, followed by the length of the raw ASCII sequence in characters, followed by the raw ASCII data. For example, the raw ASCII string "<data>" would be encoded as escape + a, length 8 (since each character is 1 byte, summing to 6 bytes, or 48 bits, 8 Base64 characters), followed by the raw ASCII representation of the string. Generally, this is only useful for large amount of special characters.
  - Binary Sequence: escape + b, followed by the length of the binary sequence in bytes, followed by the raw binary data. This is useful for embedding non-ASCII data within the compressed data.
  - Integer Sequence: escape + i, followed by the integer value encoded in standard Base64 format. This is useful for embedding numeric values directly within the compressed data. Since the integers used here are unsigned 64-bit integers, there should be 11 Base64 characters following the escape + i.
  - Double Precision Floating Point Sequence: escape + d, followed by the double value encoded in standard Base64 format. This is useful for embedding floating-point values directly within the compressed data. Since the doubles used here are IEEE 754 double-precision binary floating-point format: binary64, there should be 11 Base64 characters following the escape + d.

- Functional Characters:
  - Start of Sequence: escape + s (This is typically not needed)
  - Terminator: \\\\ (escape + escape) or \\e (escape + e)
  - Separator: escape + :
  - Assignment: :

Behavior of padding:
- If the final group of data does not make a full byte, it is padded with binary ones to make a full byte before encoding. This is also the reason we recommend using double escape (\\\\) as the terminator, since it blends better with the padding.

## Special Base 64 Encoding Example
To encode the ASCII string "value:Hello, World!", we would proceed as follows:
1. Convert each character to its corresponding Base 64 representation using the mapping above:
   - 'v' -> '0b101111' (47)
   - 'a' -> '0b011010' (26)
   - 'l' -> '0b100101' (37)
   - 'u' -> '0b101110' (46)
   - 'e' -> '0b011100' (28)
   - ':' -> '0b111110' (62)
   - 'H' -> '0b000111' (7)
   - 'e' -> '0b011100' (28)
   - 'l' -> '0b100101' (37)
   - 'l' -> '0b100101' (37)
   - 'o' -> '0b100110' (38)
   - ',' -> '0b111111' (escape) + '0b000001' (B)
   - ' ' -> '0b111111' (escape) + '0b011100' (s)
   - 'W' -> '0b001010' (48)
   - 'o' -> '0b100110' (38)
   - 'r' -> '0b101001' (41)
   - 'l' -> '0b100101' (37)
   - 'd' -> '0b100100' (36)
   - '!' -> '0b111111' (escape) + '0b001001' (I)
   - terminator -> '0b111111' (escape) + '0b111111' (escape)
2. Concatenate the 6-bit values to form a bitstream.
In 6 bit groups, the bitstream would look like this:
```101111 011010 100101 101110 011100 111110 000111 011100 100101 100101 100110 111111 000001 111111 011100 001010 100110 101001 100101 100100 111111 001001 111111 111111```
3. Finally, pack the bitstream into bytes for storage or transmission.
In bytes, the packed data would be:
```0b10111101 0b10101001 0b01101110 0b01110011 0b11100001 0b11011100 0b10010110 0b01011001 0b10111111 0b00000111 0b11110111 0b00001010 0b10011010 0b10011001 0b01100100 0b11111100 0b10011111 0b11111111```

## Checksum Calculation
The checksum is calculated using a simple XOR operation over all bytes in the compressed data (excluding the checksum byte itself). The resulting single byte is stored as the last byte of the compressed data.
To calculate the checksum:
1. Initialize a checksum variable to 0.
2. For each byte in the compressed data (excluding the checksum byte), perform an XOR operation with the checksum variable.
3. The final value of the checksum variable is the checksum byte.