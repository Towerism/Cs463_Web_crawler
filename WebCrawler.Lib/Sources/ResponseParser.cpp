#include "ResponseParser.h"

// Used regex101.com to build this regex and visualize the match.
std::regex ResponseParser::response_regex = std::regex(R"((HTTP\/1.0\s*(\d{3})\s*(\w*\s*)*\s*\\r\\n((\s*.*)\s*\\r\\n)*?\s*(Content-Length:\s*(\d*)\\r\\n\s*)?\s*\\r\\n)(\s*((.*\s?)*))?)");

ResponseParseResult* ResponseParser::Parse(std::string response)
{
  auto result = new ResponseParseResult;
  std::smatch match_results;
  result->Success = std::regex_match(response, match_results, response_regex);
  if (!result->Success)
    return result;
  result->Header = match_results[1];
  result->StatusCode = std::stoi(match_results[2]);
  if (match_results[6].length() > 0)
    result->ContentLength = std::stoi(match_results[7]);
  result->Content = match_results[9];
  return result;
}
