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

// 1) Round-trip literal-only token
TEST(LZ77_Integration, RoundTrip_LiteralOnly) {
    Token t{ /*offset=*/0, /*length=*/0,
             /*literal=*/"abcdefg",
             /*fileIdentifier=*/"",
             /*checksum=*/static_cast<uint32_t>('a'+ 'b' + 'c' + 'd' + 'e' + 'f' + 'g'),
             Token::TokenType::LITERAL };
    std::vector<Token> v{t};

    auto s  = serializeTokens(v, /*includeFileId=*/false, /*use32BitOffset=*/true,
                              /*packageID=*/0, /*globalChecksum=*/t.checksum);
    auto v2 = deserializeTokens(s);

    ASSERT_EQ(v2.size(), 1u);
    EXPECT_EQ(v2[0].literal, "abcdefg");
    EXPECT_EQ(v2[0].checksum, t.checksum);
}

// 2) Round-trip with 64-bit offset, no file ID
TEST(LZ77_Integration, RoundTrip_64BitOffset) {
    Token t{ /*offset=*/0x123456789ABCDEF0ULL,
             /*length=*/7u,
             /*literal=*/"NETWORK",
             /*fileIdentifier=*/"",
             /*checksum=*/0xABCDu,
             Token::TokenType::MATCH };
    std::vector<Token> v{t};

    auto s  = serializeTokens(v, /*includeFileId=*/false, /*use32BitOffset=*/false,
                              /*packageID=*/1, /*globalChecksum=*/t.checksum);
    auto v2 = deserializeTokens(s);

    ASSERT_EQ(v2.size(), 1u);
    EXPECT_EQ(v2[0].offset, t.offset);
    EXPECT_EQ(v2[0].literal, t.literal);
    EXPECT_EQ(v2[0].checksum, t.checksum);
}

// 3) Multiple tokens
TEST(LZ77_Integration, RoundTrip_MultipleTokens) {
    Token t1{0,0,"foo","", static_cast<uint32_t>('f'+'o'+'o'), Token::TokenType::LITERAL};
    Token t2{3,3,"bar","", static_cast<uint32_t>(3+3+'b'+'a'+'r'), Token::TokenType::MATCH};
    std::vector<Token> v{t1,t2};

    uint32_t globalCS = t1.checksum + t2.checksum;
    auto s  = serializeTokens(v, /*includeFileId=*/false, /*use32BitOffset=*/true,
                              /*packageID=*/2, /*globalChecksum=*/globalCS);
    auto v2 = deserializeTokens(s);

    ASSERT_EQ(v2.size(), 2u);
    EXPECT_EQ(v2[0].literal, "foo");
    EXPECT_EQ(v2[1].offset,  3u);
    EXPECT_EQ(v2[1].checksum, t2.checksum);
}

// 4) Corrupted data (truncated) triggers exception
TEST(LZ77_Integration, Deserialize_CorruptedData_Throws) {
    Token t{1,1,"x","", static_cast<uint32_t>('x'), Token::TokenType::LITERAL};
    std::vector<Token> v{t};
    auto s = serializeTokens(v, false, true, /*packageID=*/3, /*globalChecksum=*/t.checksum);

    // Truncate before footer
    std::string truncated = s.substr(0, s.size() - sizeof(uint32_t)/2);
    EXPECT_THROW(deserializeTokens(truncated), Compression);
}

// 5) compressBlob + decompressBlob round-trip
TEST(LZ77_Integration, CompressDecompress_RoundTrip) {
    const std::string data = "AAAAABBBBBCCCCCDDDDDEEEEE";
    auto c = compressBlob(data, /*includeID=*/false, /*offset32=*/true);
    ASSERT_EQ(c.errorCode, Compression::SUCCESS);
    auto d = decompressBlob(c.data);
    ASSERT_EQ(d.errorCode, Compression::SUCCESS);
    EXPECT_EQ(d.data, data);
}

// 6) decompressBlob on malformed serialized throws
TEST(LZ77_Integration, Deserialize_CorruptChecksum) {
    // Build a valid serialization then corrupt final checksum
    Token t{0,0,"Z","", static_cast<uint32_t>('Z'), Token::TokenType::LITERAL};
    auto s = serializeTokens({t}, /*includeFileId=*/true, /*use32BitOffset=*/true,
                              /*packageID=*/4, /*globalChecksum=*/t.checksum);
    // flip last byte
    s[s.size()-1] ^= 0xAA;
    EXPECT_THROW(deserializeTokens(s), Compression);
}

// 7) Empty input: compress then decompress yields empty
TEST(LZ77_Integration, CompressDecompress_EmptyInput) {
    const std::string empty = "";
    auto c = compressBlob(empty, false, true);
    ASSERT_EQ(c.errorCode, Compression::SUCCESS);
    auto d = decompressBlob(c.data);
    ASSERT_EQ(d.errorCode, Compression::SUCCESS);
    EXPECT_EQ(d.data, empty);
}

// 8) End-of-block token handling: explicit EOB stops decompression
TEST(LZ77_Integration, Decompress_EndOfBlock) {
    // Manually craft tokens: literal "Hi" then END_OF_BLOCK
    std::vector<Token> v;
    Token t1{0,0,"Hi","", 'H'+'i', Token::TokenType::LITERAL};
    Token t2{0,0,"","", 0u, Token::TokenType::END_OF_BLOCK};
    v.push_back(t1);
    v.push_back(t2);
    uint32_t sum = t1.checksum;
    auto s = serializeTokens(v, false, true, /*packageID=*/5, /*globalChecksum=*/sum);
    auto d = decompressBlob(s);
    ASSERT_EQ(d.errorCode, Compression::SUCCESS);
    EXPECT_EQ(d.data, "Hi");
}
