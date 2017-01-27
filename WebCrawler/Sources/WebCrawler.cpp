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
  std::cout << "Usage: WebCrawler [<threads> <textfile>]|<url>\n\n";
  std::cout << "\tAnything other than 1 thread is not currently supported.\n";
  std::cout << "\tValid url pattern: http://Host[:port][/path][?query][#fragment]\n";
}

// returns whether the url was crawled successfully, dumping a possibly existent header if nullptr is not passed
bool crawlUrl(std::string url, std::string* header = nullptr)
{
  printf("URL: %s\n", url.c_str());
  UrlParser urlParser;
  auto urlParseResult = std::unique_ptr<UrlParseResult>(urlParser.Parse(url));

  if (!urlParseResult->Success)
    return false;

  return Networking::ConnectToUrl(urlParseResult->Host, urlParseResult->Port, urlParseResult->Request, header);
}

struct Arguments
{
  std::string Url;
  int NThreads;
  std::string FileName;
};

std::unique_ptr<Arguments> parseArguments(int argc, char* argv[])
{
  auto arguments = new Arguments;
  if (argc < 2 || argc > 3)
  {
    print_usage();
    return nullptr;
  }
  int nThreads;
  try {
    arguments->NThreads = std::stoi(argv[1]);
  } catch (std::exception) {
    arguments->Url = argv[1];
    return std::unique_ptr<Arguments>(arguments);
  }
  if (arguments->NThreads != 1)
  {
    print_usage();
    return nullptr;
  }
  arguments->FileName = argv[2];
  return std::unique_ptr<Arguments>(arguments);
}

int main(int argc, char* argv[])
{
  auto arguments = parseArguments(argc, argv);
  bool basicOperation = arguments->Url != "";
  std::ifstream urlFile(arguments->FileName);
  if (Networking::InitWinsock() != 0)
    return 1;
  if (basicOperation)
  {
    std::string header;
    auto success = crawlUrl(arguments->Url, &header);
    if (success)
      std::cout << "\n------------------------------------------\n" << header;
    return 0;
  }
  if (!urlFile)
  {
    std::cout << "File " << arguments->FileName << " does not exist." << std::endl;
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

  for (auto url : urls)
  {
    crawlUrl(url);
  }

  Networking::DeInitWinsock();

  return 0;
}
