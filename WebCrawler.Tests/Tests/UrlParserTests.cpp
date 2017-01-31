// File: UrlParserTests.h
// Martin Fracker
// CSCE 463-500 Spring 2017

#include <gtest/gtest.h>
#include "UrlParser.h"

class UrlParserTests : public ::testing::Test
{
protected:
  UrlParser target;
};

TEST_F(UrlParserTests, ParseReturnsFalseForBadUrl)
{
  auto result = target.Parse("https://tamu.edu");
  EXPECT_FALSE(result->Success);
}

TEST_F(UrlParserTests, ParseReturnsTrueForNoPathNoPortNoQuery)
{
  auto result = target.Parse("http://tamu.edu");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("tamu.edu", result->Host);
  EXPECT_EQ("/", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForArbitraryHostAndExtension)
{
  auto result = target.Parse("http://arbitrary.me");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("arbitrary.me", result->Host);
  EXPECT_EQ("/", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForSubdomain)
{
  auto result = target.Parse("http://howdy.tamu.edu");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("howdy.tamu.edu", result->Host);
  EXPECT_EQ("/", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForPort)
{
  auto result = target.Parse("http://www.tamu.edu:79");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("www.tamu.edu", result->Host);
  EXPECT_EQ("/", result->Request);
  EXPECT_EQ(79, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForIp)
{
  auto result = target.Parse("http://165.155.344.455");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("165.155.344.455", result->Host);
  EXPECT_EQ("/", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForUnevenIp)
{
  auto result = target.Parse("http://165.5.34.455");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("165.5.34.455", result->Host);
  EXPECT_EQ("/", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForIpPortEmptyPath)
{
  auto result = target.Parse("http://165.5.34.455:40/");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("165.5.34.455", result->Host);
  EXPECT_EQ("/", result->Request);
  EXPECT_EQ(40, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForIpPortNonEmptyPath)
{
  auto result = target.Parse("http://165.5.34.455/NonEmptyPath/file.html");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("165.5.34.455", result->Host);
  EXPECT_EQ("/NonEmptyPath/file.html", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForIpPortSimplePath)
{
  auto result = target.Parse("http://165.5.34.455:80/file.html");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("165.5.34.455", result->Host);
  EXPECT_EQ("/file.html", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForIpPortPathFragment)
{
  auto result = target.Parse("http://165.5.34.455:40/file.html#fragment_1");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("165.5.34.455", result->Host);
  EXPECT_EQ("/file.html", result->Request);
  EXPECT_EQ(40, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForMultipleSubdomains)
{
  auto result = target.Parse("http://a.b.c.d.e.f");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("a.b.c.d.e.f", result->Host);
  EXPECT_EQ("/", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForDashes)
{
  auto result = target.Parse("http://cs-net.tam-u.edu");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("cs-net.tam-u.edu", result->Host);
  EXPECT_EQ("/", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForPhp)
{
  auto result = target.Parse("http://tamu.edu/addrbook.php");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("tamu.edu", result->Host);
  EXPECT_EQ("/addrbook.php", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForPhpWithSlash)
{
  auto result = target.Parse("http://tamu.edu/addrbook.php/");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("tamu.edu", result->Host);
  EXPECT_EQ("/addrbook.php/", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForQuery)
{
  auto result = target.Parse("http://tamu.edu/view?test=1");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("tamu.edu", result->Host);
  EXPECT_EQ("/view?test=1", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForStrayColon)
{
  auto result = target.Parse("http://tamu.edu/in:dex.html");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("tamu.edu", result->Host);
  EXPECT_EQ("/in:dex.html", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForStraySlash)
{
  auto result = target.Parse("http://tamu.edu?query=1/bash");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("tamu.edu", result->Host);
  EXPECT_EQ("?query=1/bash", result->Request);
  EXPECT_EQ(80, result->Port);
}

TEST_F(UrlParserTests, ParseReturnsTrueForTerribleUrl)
{
  auto result = target.Parse("http://adserver.adtechus.com/addyn/3.0/5214.1/3229440/0/-1/ADTECH;loc=100;alias=thestar.com_news_canada_134x156_1;size=134x156;kvcalais=lawyer:suzanne_c_t_:suzanne_c_t:quebec:taxation:taxation_in_the_united_states:tax_deduction:;kvng=revenu_qu_bec:suzanne_c_t_:supreme_court_of_canada:clothing_expenses:tax_law:;key=;rdclick=");
  EXPECT_TRUE(result->Success);
  EXPECT_EQ("adserver.adtechus.com", result->Host);
  EXPECT_EQ("/addyn/3.0/5214.1/3229440/0/-1/ADTECH;loc=100;alias=thestar.com_news_canada_134x156_1;size=134x156;kvcalais=lawyer:suzanne_c_t_:suzanne_c_t:quebec:taxation:taxation_in_the_united_states:tax_deduction:;kvng=revenu_qu_bec:suzanne_c_t_:supreme_court_of_canada:clothing_expenses:tax_law:;key=;rdclick=", result->Request);
  EXPECT_EQ(80, result->Port);
}

