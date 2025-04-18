// Created by data0 on 18.04.2025.

// LZ77UnitTests.cpp
#include <gtest/gtest.h>
#include "../../FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.h"
#include "../../Errorhandler/ErrorCodes.h"

using namespace CompressionAPI;
using ErrorCodes::Compression;


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

    // Out‑of‑bounds:
    EXPECT_THROW(readValue<uint32_t>(buf.data(), pos, /*too small*/2), Compression);
}

TEST(LZ77_Unit, ReadValue_OutOfBounds) {
    std::string small(3, '\0');
    size_t pos = 0;
    EXPECT_THROW(readValue<uint32_t>(small.data(), pos, small.size()), Compression);
}

TEST(LZ77_Unit, HeaderParsing_TokenCountZero) {
    // includeID=1, use32BitOffset=1 ⇒ header "1::1::"
    std::string data = "1::1::";
    appendValue<uint32_t>(data, 0u);

    auto tokens = deserializeTokens(data);
    EXPECT_TRUE(tokens.empty());
}

// Check that serializeTokens emits correct "includeID::offset32::" header
TEST(LZ77_Unit, Serialization_HeaderFlags) {
    const auto s = serializeTokens({}, /*incID*/false, /*32bit*/true);
    EXPECT_TRUE(s.rfind("0::1::", 0) == 0);
}

// Malformed header flags must throw
TEST(LZ77_Unit, HeaderParsing_MissingDelimiters) {
    EXPECT_THROW(deserializeTokens(""),       Compression);
    EXPECT_THROW(deserializeTokens("1:0::"),  Compression);
    EXPECT_THROW(deserializeTokens("1::0:"),  Compression);
    EXPECT_THROW(deserializeTokens("X::Y::"), Compression);
}

// serializeTokens writes the correct textual flags
TEST(LZ77_Unit, SerializeHeaderFlags) {
    std::string s = serializeTokens({}, /*includeFileId=*/true, /*use32BitOffset=*/false);

    size_t p = 0, f1 = s.find("::", p);
    EXPECT_EQ(s.substr(0, f1), "1");
    p = f1 + 2;
    size_t f2 = s.find("::", p);
    EXPECT_EQ(s.substr(p, f2 - p), "0");
}

// 6) Single‑token serialize↔deserialize preserves all fields
TEST(LZ77_Unit, SerializationRoundTrip_TokenFields) {
    Token t{
      /*offset=*/0xDEADBEEFul,
      /*length=*/5u,
      /*literal=*/"hello",
      /*fileIdentifier=*/"f.txt",
      /*checksum=*/0xFEEDu,
      /*type=*/Token::TokenType::MATCH
    };
    std::vector v{t};
    const auto s  = serializeTokens(v, /*includeFileId=*/true, /*use32BitOffset=*/true);
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

// Preserve all fields in 32‑bit+ID mode
TEST(LZ77_Unit, Serialization_SingleToken) {
    Token t{42,7,"abc","file.txt",0xFFEE, Token::TokenType::MATCH};
    auto s = serializeTokens({t}, true, true);
    auto v = deserializeTokens(s);
    ASSERT_EQ(v.size(),1);
    EXPECT_EQ(v[0], t);
}

// Missing first "::" → parse failure
TEST(LZ77_Unit, Header_Parse_MissingDelim1) {
    EXPECT_THROW(deserializeTokens(""), Compression);
    EXPECT_THROW(deserializeTokens("1:0::"), Compression);
}

// Non‑numeric flags → parse failure
TEST(LZ77_Unit, Header_Parse_InvalidFlags) {
    EXPECT_THROW(deserializeTokens("X::Y::0"), Compression);
}