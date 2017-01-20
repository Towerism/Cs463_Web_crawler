// File: UrlParser.h
// Martin Fracker
// CSCE 463-500 Spring 2017
#pragma once
#include <regex>
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
  static std::regex url_regex;
};
