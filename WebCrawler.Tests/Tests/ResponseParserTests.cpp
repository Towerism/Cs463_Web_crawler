// File: ResponseParserTests.cpp
// Martin Fracker
// CSCE 463-500 Spring 2017

#include <gtest/gtest.h>
#include "ResponseParser.h"

class ResponseParserTests : public ::testing::Test
{
protected:
  ResponseParser target;
};

TEST_F(ResponseParserTests, ParseReturnsTrueForBasicResponse)
{
  auto result = target.Parse("HTTP/1.0 200 OK\r\nContent-Length: 16367\r\n\r\ncontent");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("HTTP/1.0 200 OK\r\nContent-Length: 16367\r\n\r\n", result->Header);
  EXPECT_EQ(200, result->StatusCode);
  EXPECT_EQ("content", result->Content);
}

TEST_F(ResponseParserTests, ParseReturnsTrueForBasicNon200Response)
{
  auto result = target.Parse("HTTP/1.0 500 Internal Server Error\r\nContent-Length: 16367\r\n\r\ncontent");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("HTTP/1.0 500 Internal Server Error\r\nContent-Length: 16367\r\n\r\n", result->Header);
  EXPECT_EQ(500, result->StatusCode);
  EXPECT_EQ("content", result->Content);
}

TEST_F(ResponseParserTests, ParseReturnsTrueForComplexResponse)
{
  auto result = target.Parse("HTTP/1.0 200 OK\r\n Cache-Control: private\r\n Content-Type: text/html\r\n Server: Microsoft-IIS/7.0\r\n X-Powered-By: ASP.NET\r\n MicrosoftOfficeWebServer: 5.0_Pub\r\n MS-Author-Via: MS-FP/4.0\r\n Date: Thu, 17 Jan 2013 09:22:34 GMT\r\n Connection: close\r\n Content-Length: 16367\r\n \r\n<html> <head> <meta http-equiv = \"Content-Language\" content = \"en-us\"> <meta httpequiv = \"Content-Type\" content = \"text/html; charset=windows-1252\">");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("HTTP/1.0 200 OK\r\n Cache-Control: private\r\n Content-Type: text/html\r\n Server: Microsoft-IIS/7.0\r\n X-Powered-By: ASP.NET\r\n MicrosoftOfficeWebServer: 5.0_Pub\r\n MS-Author-Via: MS-FP/4.0\r\n Date: Thu, 17 Jan 2013 09:22:34 GMT\r\n Connection: close\r\n Content-Length: 16367\r\n \r\n"
    , result->Header);
  EXPECT_EQ(200, result->StatusCode);
  EXPECT_EQ("<html> <head> <meta http-equiv = \"Content-Language\" content = \"en-us\"> <meta httpequiv = \"Content-Type\" content = \"text/html; charset=windows-1252\">"
    , result->Content);
}

TEST_F(ResponseParserTests, ParseReturnsTrueWithoutHtml)
{
  auto result = target.Parse("HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n", result->Header);
  EXPECT_EQ(200, result->StatusCode);
  EXPECT_EQ("", result->Content);
} 

TEST_F(ResponseParserTests, ParseReturnsTrueForHttp1Dot1)
{
  auto result = target.Parse("HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n", result->Header);
  EXPECT_EQ(200, result->StatusCode);
  EXPECT_EQ("", result->Content);
} 

TEST_F(ResponseParserTests, ParseReturnsTrueForExample)
{
  auto result = target.Parse("HTTP/1.1 200 OK\r\nCache-Control: private\r\nContent-Length: 23089\r\nContent-Type: text/html\r\nServer: Microsoft-IIS/7.0\r\nSet-Cookie: ASPSESSIONIDQSASDCCC=OIIBOKFCABNCBAHNCLLGHLIO; path=  /\r \n Date: Sun, 22 Jan 2017 16:45:27 GMT\r\nConnection: close\r\n\r\n");
  EXPECT_TRUE(result->Success);
}

TEST_F(ResponseParserTests, ParseReturnsTrueForEvilResponse)
{
  auto result = target.Parse("HTTP\/1.1 200 OK\r\nConnection: close\r\nDate: Tue, 3\r\n\r\n\r\n\t\t\r\n\t\t\r\n\t\t\r\n\r\n\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\r\n\r\n\t\t\r\n\t\t\r\n\t\t\t\r\n\t\r\n\t\r\n\t\r\n\r\n\r\n\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\r\n\t\r\n\r\n\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t \r\n\t\t\t\r\n\t\t\t\r\n\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\t\t\r\n\r\n\r\n\r\n\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\r\n\t\t\r\n\t\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\r\n\r\n\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\t\t\r\n\r\n\r\n\t\r\n\r\n\t\r\n\r\n\r\n\t\r\n\t\r\n\r\n\r\n\t\r\n\r\n\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\t\r\n\t\t\t\t\r\n\t\t\t\r\n\t\t\r\n\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\r\n\t\t \r\n\t\t\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\t\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t\r\n\t\r\n\t\t\r\n\t\t\r\n\t\r\n\r\n\r\n\r\n\t\t\r\n\t\t\r\n\t\t\t\r\n\t\t\t\t\t\t\t\r\n\t\t\t\r\n\t\t\r\n\t\t\r\n\t");
  EXPECT_TRUE(result->Success);
}
