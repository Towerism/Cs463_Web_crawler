#include "UrlParser.h"

// http:///Host[:port][/path][?query][#fragment]
// match[0] is the whole url
// match[1] is the Host
// match[2] is the port including colon
// match[3] is the port excluding colon
// match[4] is the Request
// match[5] is the path
// match[6] is the query
// match[7] is the fragment
std::regex UrlParser::url_regex = std::regex("http:\\/\\/(\\.?[^:?#/]*)(:(\\d*))?((\\/[^\\?#]*)*(\\?[^#]*)?)(#.*)?");

UrlParseResult* UrlParser::Parse(std::string url)
{
  auto result = new UrlParseResult;
  std::smatch match_results;
  result->Success = regex_match(url, match_results, url_regex);
  result->Host = match_results[1];
  if (match_results[3].length() > 0)
    result->Port = stoi(match_results[3]);
  if (match_results[4].length() > 0)
    result->Request = match_results[4];
  return result;
}
