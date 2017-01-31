// File: UrlParser.cpp
// Martin Fracker
// CSCE 463-500 Spring 2017

#include "UrlParser.h"
#include "HTMLParserBase.h"

// http://Host[:port][/path][?query][#fragment]
REGEX_NAMESPACE::regex UrlParser::url_regex = REGEX_NAMESPACE::regex(R"(http:\/\/(\.?[^:?#\/]*)(:(\d*))?([^\?#]*(\?[^#]*)?)(#.*)?)");

UrlParseResult* UrlParser::Parse(std::string url)
{
  auto result = new UrlParseResult;
  REGEX_NAMESPACE::smatch match_results;
  result->Success = regex_match(url, match_results, url_regex);
  result->Host = match_results[1];
  if (match_results[3].length() > 0)
    result->Port = stoi(match_results[3]);
  if (match_results[4].length() > 0)
    result->Request = match_results[4];
  //printf("\t  Parsing URL... ");
  if (result->Host.length() > MAX_HOST_LEN)
  {
    //printf("failed with long host length\n");
    return SetSuccessFalseAndReturn(result);
  }
  if (result->Request.length() > MAX_REQUEST_LEN)
  {
    //printf("failed with long request length\n");
    return SetSuccessFalseAndReturn(result);
  }
  if (result->Success) {
    //printf("host %s, port %d\n", result->Host.c_str(), result->Port);
  }
  else
  {
    //printf("failed with invalid scheme or port\n");
  }
  return result;
}

UrlParseResult* UrlParser::SetSuccessFalseAndReturn(UrlParseResult* result)
{
  result->Success = false;
  return result;
}

