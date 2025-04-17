//
// Created by data0 on 17.04.2025.
//

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include "../Errorhandler/ErrorHandler.h"

TEST(MyInfoCaptureTest, SeesIC6) {
    // 1) Redirect std::clog
    std::ostringstream capture;
    auto oldBuf = std::clog.rdbuf(capture.rdbuf());

    // 2) Trigger your logInfo call
    ErrorHandler::logInfo("LZ77::deserializeTokens",
                         ErrorCodes::Compression::IC6,
                         "deserializeTokens complete");

    // 3) Grab what was written
    std::string s = capture.str();

    EXPECT_NE(s.find("IC6"), std::string::npos)
        << "Expected to see IC6 in:\n" << s;

    // 4) Restore the original buffer
    std::clog.rdbuf(oldBuf);
}
