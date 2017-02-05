// File: ResponseParser.cpp
// Martin Fracker
// CSCE 463-500 Spring 2017
#include "ResponseParser.h"
#include "Networking.h"

// Used regex101.com to build this regex and visualize the match.
std::regex ResponseParser::response_regex = std::regex(R"((HTTP\/1.(0|1)\s*(\d{3})[\s\S]*?\r\n\s*?\r\n))");

ResponseParseResult* ResponseParser::Parse(std::string response)
{
  auto result = new ResponseParseResult;
  if (response.length() > MAX_PAGE_SIZE) {
    result->Success = false;
    return result;
  }
  std::smatch match_results;
  result->Success = std::regex_search(response, match_results, response_regex);
  if (!result->Success)
    return result;
  result->Header = match_results[1];
  result->StatusCode = std::stoi(match_results[3]);

  // parsing for content is a bit weird
  // content can be arbitrarily long, plus there is no need to feed it through the regex library
  // in some edge cases the string is too complex for the library to handle and it throws a regex_error
  auto length = match_results[0].length();
  std::string content(response);
  content.erase(0, length);
  result->Content = content;

  return result;
}
