// File: WebCrawler.cpp
// Martin Fracker
// CSCE 463-500 Spring 2017

#include "stdafx.h"
#include <iostream>
#include <memory>
#include "UrlParser.h"

void print_usage()
{
  std::cout << "Usage: WebCrawler <url>\n\n"
    << "A valid URL is of the form:\n"
    << "  scheme://host[:port][/path][?query][#fragment]\n";
}

int main(int argc, char* argv[])
{
  if (argc < 2) {
    print_usage();
    return 1;
  }
  std::string url(argv[1]);
  UrlParser urlParser;
  auto parseResult = std::unique_ptr<UrlParseResult>(urlParser.Parse(url));
  
  if (!parseResult->Success) {
    print_usage();
    return 1;
  }
  return 0;
}
