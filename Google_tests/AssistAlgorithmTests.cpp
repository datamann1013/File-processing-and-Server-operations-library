// Created by admin on 15.04.2025.

#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.h"
#include "../FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.cpp"

namespace fs = std::filesystem;

#define ASSERT_NOT_IMPLEMENTED(result)                                   \
    if (result.errorCode == ErrorCodes::Compression::ENN99) {            \
    FAIL() << "Function not implemented (returned ENN99)";               \
}

#define ASSERT_LOG_CONTAINS(strm, substr) {                              \
    std::string s = strm.str(); EXPECT_NE(s.find(substr),                \
        std::string::npos) << "Expected log to contain '"<<substr<<"'";  \
        }

TEST(AssistAlgorithmTests, FileLocatorFilesDirectory) {
    const fs::path path(TEST_FILES_DIR);
    std::cout << "Current working directory: " << fs::current_path() << std::endl;
    EXPECT_TRUE(fs::exists(path)) << "TestFiles directory not found: " << path.string();
}

TEST(AssistAlgorithmTests, AppendAndReadInteger_LZ77) {
    std::string output;
    int32_t testVal = 123456;
    CompressionAPI::appendValue(output, testVal);
    ASSERT_EQ(output.size(), sizeof(testVal));

    size_t pos = 0;
    const int32_t readVal = CompressionAPI::readValue<int32_t>(output.data(), pos, sizeof(testVal));
    EXPECT_EQ(testVal, readVal);
    EXPECT_EQ(pos, sizeof(testVal));
}

TEST(AssistAlgorithmTests, AppendAndReadUint8_LZ77) {
    std::string output;
    constexpr uint8_t testVal = 42;
    CompressionAPI::appendValue(output, testVal);
    ASSERT_EQ(output.size(), sizeof(testVal));

    size_t pos = 0;
    const uint8_t readVal = CompressionAPI::readValue<uint8_t>(output.data(), pos, sizeof(testVal));
    EXPECT_EQ(testVal, readVal);
    EXPECT_EQ(pos, sizeof(testVal));
}

TEST(AssistAlgorithmTests, SerializeDeserializeICLog) {
    std::ostringstream capture; auto old = std::clog.rdbuf(capture.rdbuf());
    CompressionAPI::Token token{100,50,"lit","ID",0xABCD1234,CompressionAPI::Token::TokenType::MATCH};

    std::vector v={token};
    auto bin = serializeTokens(v,true,true);

    ASSERT_LOG_CONTAINS(capture, "IC3"); // start serialize
    ASSERT_LOG_CONTAINS(capture, "IC4"); // complete serialize

    capture.str(""); capture.clear();
    auto v2 = CompressionAPI::deserializeTokens(bin);
    ASSERT_LOG_CONTAINS(capture, "IC5"); // start deserialize
    ASSERT_LOG_CONTAINS(capture, "IC6"); // complete deserialize

    std::clog.rdbuf(old);
}

TEST(AssistAlgorithmTests, WithFileIdentifierAnd32BitOffset_LZ77) {
    // Create sample token.
    CompressionAPI::Token token;
    token.offset = 100;
    token.length = 50;
    token.literal = "Hello, World!";
    token.fileIdentifier = "Sample.txt";
    token.checksum = 0xDEADBEEF;
    token.type = CompressionAPI::Token::TokenType::MATCH;

    std::vector tokens = { token };

    // Serialize tokens (include file identifier, use 32-bit offset).
    const std::string serialized = serializeTokens(tokens, true, true);

    // Deserialize tokens.
    const std::vector<CompressionAPI::Token> deserialized = CompressionAPI::deserializeTokens(serialized);
    ASSERT_EQ(deserialized.size(), tokens.size());
    const CompressionAPI::Token &t = deserialized[0];
    EXPECT_EQ(t.offset, token.offset);
    EXPECT_EQ(t.length, token.length);
    EXPECT_EQ(t.literal, token.literal);
    EXPECT_EQ(t.fileIdentifier, token.fileIdentifier);
    EXPECT_EQ(t.checksum, token.checksum);
    EXPECT_EQ(t.type, token.type);
}

TEST(AssistAlgorithmTests, WithoutFileIdentifierAnd64BitOffset_LZ77) {
    constexpr bool includeFileId = false;
    constexpr bool use32BitOffset = false;

    // Build header matching those flags:
    const std::string header =
        std::to_string(includeFileId ? 1 : 0) + "::" +
        std::to_string(use32BitOffset ? 1 : 0) + "::";

    CompressionAPI::Token token;
    token.offset         = 1234567890123ull;
    token.length         = 75;
    token.literal        = "DataExample";
    token.fileIdentifier = "";
    token.checksum       = 0xCAFEBABE;
    token.type           = CompressionAPI::Token::TokenType::LITERAL;

    std::vector tokens = {token};

    std::string tokenData = serializeTokens(tokens, includeFileId, use32BitOffset);

    size_t headerEnd = tokenData.find("::", 0);
    headerEnd = tokenData.find("::", headerEnd + 2) + 2;
    std::string customSerialized = header + tokenData.substr(headerEnd);

    auto deserialized = CompressionAPI::deserializeTokens(customSerialized);
    ASSERT_EQ(deserialized.size(), tokens.size());
    auto &t = deserialized[0];
    EXPECT_EQ(t.offset,  token.offset);
    EXPECT_EQ(t.length,  token.length);
    EXPECT_EQ(t.literal, token.literal);
    EXPECT_EQ(t.checksum, token.checksum);
    EXPECT_EQ(t.type,    token.type);
}

TEST(AssistAlgorithmTests, DeserializeTruncatedDataThrows_LZ77) {
    // Create a valid token, serialize it, then truncate the output.
    CompressionAPI::Token token;
    token.offset = 500;
    token.length = 50;
    token.literal = "DataForTest";
    token.fileIdentifier = "File.txt";
    token.checksum = 123456;
    token.type = CompressionAPI::Token::TokenType::MATCH;

    std::vector tokens = {token};
    const std::string serialized = serializeTokens(tokens, true, true);

    // Truncate the string by removing the last 5 bytes.
    const std::string truncated = serialized.substr(0, serialized.size() - 5);

    // Expect deserialization to throw an exception due to insufficient data.
    EXPECT_THROW({
        auto tokensDeser = CompressionAPI::deserializeTokens(truncated);
    }, std::runtime_error);
}

TEST(AssistAlgorithmTests, LZ77CompressTest) {
    // Open the test file
    std::ifstream file(TEST_FILE_SINGULAR);
    ASSERT_TRUE(file.is_open()) << "Could not open file: TEST_FILE_SINGULAR";

    // Read the file's content into a string.
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string inputData = buffer.str();

    auto result = CompressionAPI::compressBlob(inputData);

    ASSERT_NOT_IMPLEMENTED(result);

    // - Check if decompression reverses the compression.
    // - Validate output sizes or tokens.
}