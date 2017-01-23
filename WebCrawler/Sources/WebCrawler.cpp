// File: WebCrawler.cpp
// Martin Fracker
// CSCE 463-500 Spring 2017

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <memory>
#include "UrlParser.h"
#include "Networking.h"

void print_usage()
{
  std::cout << "Usage: WebCrawler <url>\n\n"
    << "A valid URL is of the form:\n"
    << "  scheme://host[:port][/path][?query][#fragment]\n";
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    print_usage();
    return 1;
  }
  std::string url(argv[1]);
  printf("URL: %s\n", url.c_str());
  UrlParser urlParser;
  auto urlParseResult = std::unique_ptr<UrlParseResult>(urlParser.Parse(url));

  if (!urlParseResult->Success)
  {
    print_usage();
    return 1;
  }
  if (Networking::InitWinsock() != 0)
    return 1;

  Networking::DNS::printDNSServer();
  auto responseParseResult = Networking::ConnectToUrl(urlParseResult->Host, urlParseResult->Port, urlParseResult->Request);
  printf("\n-------------------------------------\n");
  std::cout << responseParseResult.Header;

  Networking::DeInitWinsock();

  return 0;
}
