// File: UrlParser.h
// Martin Fracker
// CSCE 463-500 Spring 2017

#pragma once

#ifdef USE_BOOST_FOR_REGEX
#include <boost/regex.hpp>
#define REGEX_NAMESPACE boost
#else
#include <regex>
#define REGEX_NAMESPACE std
#endif

#include "ParseResultBase.h"
#include "IStringParser.h"

class UrlParseResult : public ParseResultBase
{
public:
  std::string Host;
  std::string Request = "/";
  int Port = 80;
};

class UrlParser : public IStringParser
{
public:
  UrlParseResult* Parse(std::string url) override;

private:
  static REGEX_NAMESPACE::regex url_regex;

  static UrlParseResult* SetSuccessFalseAndReturn(UrlParseResult* result);
};
