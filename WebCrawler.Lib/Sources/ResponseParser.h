// File: Response Parser.h
// Martin Fracker
// CSCE 463-500 Spring 2017
#pragma once
#include "ParseResultBase.h"
#include "IStringParser.h"
#include <regex>

class ResponseParseResult : public ParseResultBase
{
public:
  std::string Header;
  int StatusCode = 0;
  std::string Content;
};

class ResponseParser : public IStringParser
{
public:
  ResponseParseResult* Parse(std::string response) override;

private:
  static std::regex response_regex;
};
