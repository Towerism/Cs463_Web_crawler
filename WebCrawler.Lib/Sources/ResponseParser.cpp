// File: ResponseParser.cpp
// Martin Fracker
// CSCE 463-500 Spring 2017
#include "ResponseParser.h"

// Used regex101.com to build this regex and visualize the match.
std::regex ResponseParser::response_regex = std::regex(R"((HTTP\/1.(0|1)\s*(\d{3})[\s\S]*?\r\n\s*\r\n)([\s\S]*)?)");

ResponseParseResult* ResponseParser::Parse(std::string response)
{
  auto result = new ResponseParseResult;
  if (response.length() > 2000)
    return result;
  std::smatch match_results;
  result->Success = std::regex_match(response, match_results, response_regex);
  if (!result->Success)
    return result;
  result->Header = match_results[1];
  result->StatusCode = std::stoi(match_results[3]);
  result->Content = match_results[4];
  return result;
}
