// Created by data0 on 18.04.2025.

#include <gtest/gtest.h>
#include "../../FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.h"
#include "../../Errorhandler/ErrorCodes.h"

using namespace CompressionAPI;
using ErrorCodes::Compression;

// 1) Round‑trip literal‑only token
TEST(LZ77_Integration, RoundTrip_LiteralOnly) {
    Token t{ /*offset=*/0, /*length=*/0,
             /*literal=*/"abcdefg",
             /*fileIdentifier=*/"",
             /*checksum=*/0,
             Token::TokenType::LITERAL };
    std::vector<Token> v{t};

    auto s  = serializeTokens(v, /*includeFileId=*/false, /*use32BitOffset=*/true);
    auto v2 = deserializeTokens(s);

    ASSERT_EQ(v2.size(), 1u);
    EXPECT_EQ(v2[0].literal, "abcdefg");
}

// 2) Round‑trip with 64‑bit offset, no file ID
TEST(LZ77_Integration, RoundTrip_64BitOffset) {
    Token t{ /*offset=*/0x123456789ABCDEF0ULL,
             /*length=*/7u,
             /*literal=*/"NETWORK",
             /*fileIdentifier=*/"",
             /*checksum=*/0xABCDu,
             Token::TokenType::MATCH };
    std::vector<Token> v{t};

    auto s  = serializeTokens(v, /*includeFileId=*/false, /*use32BitOffset=*/false);
    auto v2 = deserializeTokens(s);

    ASSERT_EQ(v2.size(), 1u);
    EXPECT_EQ(v2[0].offset, t.offset);
    EXPECT_EQ(v2[0].literal, t.literal);
}

// 3) Multiple tokens
TEST(LZ77_Integration, RoundTrip_MultipleTokens) {
    Token t1{0,0,"foo","", 0, Token::TokenType::LITERAL};
    Token t2{3,3,"bar","", 0, Token::TokenType::MATCH};
    std::vector<Token> v{t1,t2};

    auto s  = serializeTokens(v, /*includeFileId=*/false, /*use32BitOffset=*/true);
    auto v2 = deserializeTokens(s);

    ASSERT_EQ(v2.size(), 2u);
    EXPECT_EQ(v2[0].literal, "foo");
    EXPECT_EQ(v2[1].offset,  3u);
}

// 4) Corrupted data triggers an exception
TEST(LZ77_Integration, Deserialize_CorruptedData_Throws) {
    const Token t{1,1,"x","",0,Token::TokenType::LITERAL};
    const std::vector v{t};
    auto s = serializeTokens(v, /*includeFileId=*/false, /*use32BitOffset=*/true);

    // Flip one byte at the end to corrupt checksum
    const std::string truncated = s.substr(0, s.size() - 5);

    EXPECT_THROW(deserializeTokens(truncated), Compression);
}

// compressBlob + decompressBlob must return the original data.
TEST(LZ77_Integration, CompressDecompress_RoundTrip) {
    std::string data = "AAAAABBBBBCCCCCDDDDDEEEEE";
    auto c = compressBlob(data, false, true);
    ASSERT_EQ(c.errorCode, Compression::SUCCESS);
    auto d = decompressBlob(c.data);
    ASSERT_EQ(d.errorCode, Compression::SUCCESS);
    EXPECT_EQ(d.data, data);
}

// Flip checksum bytes → throws ES2
TEST(LZ77_Integration, Deserialize_CorruptChecksum) {
    auto s = serializeTokens({{/*…*/}}, true, true);
    // flip one checksum byte:
    s[s.size()-1] ^= 0xFF;
    EXPECT_THROW(deserializeTokens(s), Compression);
}