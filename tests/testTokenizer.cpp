/**
 * Copyright (C) 2010 Jakub Wieczorek <fawek@fawek.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <gtest/gtest.h>

#include "Tokenizer.h"

#include <iostream>
#include <fstream>
#include <string>

const char* tokenTypeName(Tokenizer::Token::Type type)
{
    switch (type) {
        case Tokenizer::Token::None:
            return "None";
        case Tokenizer::Token::Text:
            return "Text";
        case Tokenizer::Token::OpenComment:
            return "OpenComment";
        case Tokenizer::Token::CloseComment:
            return "CloseComment";
        case Tokenizer::Token::OpenVariable:
            return "OpenVariable";
        case Tokenizer::Token::CloseVariable:
            return "CloseVariable";
        case Tokenizer::Token::OpenTag:
            return "OpenTag";
        case Tokenizer::Token::CloseTag:
            return "CloseTag";
        case Tokenizer::Token::EndOfInput:
            return "EndOfInput";
        default:
            assert(0);
    }
}

bool shouldPrintContents(Tokenizer::Token::Type type)
{
    switch (type) {
        case Tokenizer::Token::None:
        case Tokenizer::Token::OpenComment:
        case Tokenizer::Token::CloseComment:
        case Tokenizer::Token::OpenVariable:
        case Tokenizer::Token::CloseVariable:
        case Tokenizer::Token::OpenTag:
        case Tokenizer::Token::CloseTag:
        case Tokenizer::Token::EndOfInput:
            return false;
        case Tokenizer::Token::Text:
            return true;
        default:
            assert(0);
    }
}

std::string removeNewLines(const std::string& string)
{
    std::string ret;
    for (int i = 0; i < string.size(); ++i) {
        if (string[i] == '\n')
            ret.append("\\n");
        else
            ret.push_back(string[i]);
    }
    return ret;
}

class TokenizerTest : public ::testing::TestWithParam<const char*> { };

#define DATA_PATH "./testTokenizerData/"

TEST_P(TokenizerTest, Tokenize) {
    const char* testName = GetParam();
    
    char* inputFile = (char*) malloc(sizeof(char) * (strlen(DATA_PATH) + strlen(testName) + strlen(".in") + 1));
    strcpy(inputFile, DATA_PATH);
    strcat(inputFile, testName);
    strcat(inputFile, ".in");
    std::fstream inputStream(inputFile, std::fstream::in);
    ASSERT_TRUE(inputStream.good());
    
    char* resultFile = (char*) malloc(sizeof(char) * (strlen(DATA_PATH) + strlen(testName) + strlen(".out") + 1));
    strcpy(resultFile, DATA_PATH);
    strcat(resultFile, testName);
    strcat(resultFile, ".out");
    std::fstream resultStream(resultFile, std::fstream::in);
    ASSERT_TRUE(resultStream.good());
    
    Tokenizer tokenizer(&inputStream);
    Tokenizer::Token token;
    std::string output;
    
    do {
        tokenizer.nextToken(token);

        ASSERT_NE(token.type, Tokenizer::Token::None);
        
        std::string line;
        line.append(tokenTypeName(token.type));
        
        if (shouldPrintContents(token.type)) {
            line.push_back(' ');
            line.push_back('"');
            line.append(removeNewLines(token.contents));
            line.push_back('"');
        }

        output.append(line);
        output.push_back('\n');
    } while (token.type != Tokenizer::Token::EndOfInput);
    
    std::istringstream outputStream(output);
    int line = 1;
    while (resultStream.good() || outputStream.good()) {
        std::string outputLine, resultLine;
        getline(outputStream, outputLine);
        getline(resultStream, resultLine);
        
        ASSERT_EQ(resultLine, outputLine) << "Line " << line;
        ++line;
    }
}

INSTANTIATE_TEST_CASE_P(Basic, TokenizerTest, ::testing::Values(
    "almostLikeClauses", "emptyFile", "filterWithLiteralParameter", "simpleComment", "simpleTag", "simpleTemplate", "simpleVariable", "tagWithLiteralParameter"
    ));

