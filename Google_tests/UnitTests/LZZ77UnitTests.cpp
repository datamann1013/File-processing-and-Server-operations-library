// Created by data0 on 18.04.2025.

#include <gtest/gtest.h>
#include "../../FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.h"
#include "../../Errorhandler/ErrorCodes.h"

using namespace CompressionAPI;
using ErrorCodes::Compression;

// Helper to write delimiters
static void appendDelim(std::string &s) {
    s += "::";
}

TEST(LZ77_Unit, AppendAndReadPrimitives) {
    std::string buf;
    appendValue<uint8_t>(buf,  0xAB);
    appendValue<uint32_t>(buf, 0x12345678);
    appendValue<uint64_t>(buf, 0x1122334455667788ULL);

    size_t pos = 0;
    EXPECT_EQ(readValue<uint8_t>(buf.data(), pos, buf.size()), 0xAB);
    EXPECT_EQ(pos, sizeof(uint8_t));

    EXPECT_EQ(readValue<uint32_t>(buf.data(), pos, buf.size()), 0x12345678);
    EXPECT_EQ(pos, sizeof(uint8_t) + sizeof(uint32_t));

    EXPECT_EQ(readValue<uint64_t>(buf.data(), pos, buf.size()), 0x1122334455667788ULL);
    EXPECT_EQ(pos, buf.size());

    // Out-of-bounds:
    EXPECT_THROW(readValue<uint32_t>(buf.data(), pos, /*too small*/2), Compression);
}

TEST(LZ77_Unit, ReadValue_OutOfBounds) {
    std::string small(3, '\0');
    size_t pos = 0;
    EXPECT_THROW(readValue<uint32_t>(small.data(), pos, small.size()), Compression);
}

TEST(LZ77_Unit, HeaderParsing_TokenCountZero) {
    // includeID=1, use32BitOffset=1, packageID=0, tokenCount=0, globalChecksum=0
    std::string data;
    data += '1'; appendDelim(data);
    data += '1'; appendDelim(data);
    appendValue<uint32_t>(data, 0u); // packageID
    appendDelim(data);
    appendValue<uint32_t>(data, 0u); // tokenCount
    appendDelim(data);
    appendValue<uint32_t>(data, 0u); // globalChecksum

    auto tokens = deserializeTokens(data);
    EXPECT_TRUE(tokens.empty());
}

// Check that serializeTokens emits correct "includeID::offset32::" header prefix
TEST(LZ77_Unit, Serialization_HeaderFlags) {
    const auto s = serializeTokens({}, /*incID*/false, /*32bit*/true, /*packageID*/0, /*globalChecksum*/0);
    EXPECT_TRUE(s.rfind("0::1::", 0) == 0);
}

// Malformed header flags must throw (too short, missing parts)
TEST(LZ77_Unit, HeaderParsing_MissingDelimiters) {
    EXPECT_THROW(deserializeTokens(""), Compression);
    EXPECT_THROW(deserializeTokens("1:0::"), Compression);
    EXPECT_THROW(deserializeTokens("1::0:"), Compression);
    EXPECT_THROW(deserializeTokens("X::Y::"), Compression);
}

// serializeTokens writes the correct textual flags
TEST(LZ77_Unit, SerializeHeaderFlags) {
    std::string s = serializeTokens({}, /*includeFileId=*/true, /*use32BitOffset=*/false, /*packageID*/0, /*globalChecksum*/0);

    size_t p = 0;
    size_t f1 = s.find("::", p);
    EXPECT_EQ(s.substr(0, f1), "1");
    p = f1 + 2;
    size_t f2 = s.find("::", p);
    EXPECT_EQ(s.substr(p, f2 - p), "0");
}

// Round‐trip of a single token preserves all fields
TEST(LZ77_Unit, SerializationRoundTrip_TokenFields) {
    Token t{
      /*offset=*/0xDEADBEEFul,
      /*length=*/5u,
      /*literal=*/"hello",
      /*fileIdentifier=*/"f.txt",
      /*checksum=*/0xFEEDu,
      /*type=*/Token::TokenType::MATCH
    };
    std::vector<Token> v{t};
    const auto s  = serializeTokens(v, /*includeFileId=*/true, /*use32BitOffset=*/true, /*packageID*/1, /*globalChecksum*/0xFEEDu);
    const auto v2 = deserializeTokens(s);

    ASSERT_EQ(v2.size(), 1u);
    const auto &u = v2[0];
    EXPECT_EQ(u.offset,         t.offset);
    EXPECT_EQ(u.length,         t.length);
    EXPECT_EQ(u.literal,        t.literal);
    EXPECT_EQ(u.fileIdentifier, t.fileIdentifier);
    EXPECT_EQ(u.checksum,       t.checksum);
    EXPECT_EQ(u.type,           t.type);
}

// Preserve all fields in 32-bit+ID mode
TEST(LZ77_Unit, Serialization_SingleToken) {
    Token t{42,7,"abc","file.txt",0xFFEEu, Token::TokenType::MATCH};
    auto s = serializeTokens({t}, /*includeFileId=*/true, /*use32BitOffset=*/true, /*packageID*/1, /*globalChecksum*/0xFFEEu);
    auto v = deserializeTokens(s);
    ASSERT_EQ(v.size(),1);
    EXPECT_EQ(v[0], t);
}

// Detect global checksum mismatch
TEST(LZ77_Unit, Deserialize_GlobalChecksumMismatch) {
    Token lit{0,0,"A","",static_cast<uint32_t>('A'), Token::TokenType::LITERAL};
    auto full = serializeTokens({lit}, /*includeFileId=*/false, /*use32BitOffset=*/false, /*packageID*/1, /*globalChecksum*/lit.checksum);
    // Corrupt last 4 bytes (global checksum)
    size_t offs = full.size() - sizeof(uint32_t);
    full[offs] ^= 0xFF;
    EXPECT_THROW(deserializeTokens(full), Compression);
}

// FileID preservation in header and token
TEST(LZ77_Unit, Deserialize_FinalFileId) {
    std::string data;
    data += '1'; appendDelim(data);
    data += '0'; appendDelim(data);
    appendValue<uint32_t>(data, 5u);         // packageID
    appendDelim(data);
    appendValue<uint32_t>(data, 0u);         // tokenCount
    appendDelim(data);
    // final fileId
    const std::string fid = "file42";
    appendValue<uint32_t>(data, static_cast<uint32_t>(fid.size()));
    data += fid;
    appendDelim(data);
    appendValue<uint32_t>(data, 0u);         // globalChecksum

    EXPECT_NO_THROW({ auto tokens = deserializeTokens(data); EXPECT_TRUE(tokens.empty()); });
}
