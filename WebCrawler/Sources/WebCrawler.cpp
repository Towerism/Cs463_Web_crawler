// File: WebCrawler.cpp
// Martin Fracker
// CSCE 463-500 Spring 2017

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <fstream>
#include "UrlParser.h"
#include "Networking.h"
#include <unordered_set>

void print_usage()
{
  std::cout << "Usage: WebCrawler <threads> <textfile>\n\n";
  std::cout << "Anything other than 1 thread is not currently supported.\n";
}

void crawlUrl(std::string url)
{
  printf("URL: %s\n", url.c_str());
  UrlParser urlParser;
  auto urlParseResult = std::unique_ptr<UrlParseResult>(urlParser.Parse(url));

  if (!urlParseResult->Success)
    return;

  Networking::ConnectToUrl(urlParseResult->Host, urlParseResult->Port, urlParseResult->Request);
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    print_usage();
    return 1;
  }
  int nThreads;
  try {
    nThreads = std::stoi(argv[1]);
  } catch (std::exception) {
    print_usage();
    return 1;
  }
  if (nThreads != 1)
  {
    print_usage();
    return 1;
  }
  std::string fileName(argv[2]);
  std::ifstream urlFile(fileName);
  if (!urlFile)
  {
    std::cout << "File " << fileName << " does not exist." << std::endl;
    return 1;
  }
  std::string line;
  std::vector<std::string> urls;
  while (true)
  {
    std::getline(urlFile, line);
    if (urlFile.eof())
      break;
    urls.push_back(line);
  }
  if (Networking::InitWinsock() != 0)
    return 1;

  for (auto url : urls)
  {
    crawlUrl(url);
  }

  Networking::DeInitWinsock();

  return 0;
}
