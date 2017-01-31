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
#include <queue>
#include <mutex>

namespace SharedData
{
  std::queue<std::string> urls;
  std::mutex mutex;
};

void print_usage()
{
  std::cout << "Usage: WebCrawler [<threads> <textfile>]|<url>\n\n";
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

void collectUrlsToCrawl(std::ifstream& urlFile, std::shared_ptr<Arguments> arguments)
{
  std::string url;
  while (true)
  {
    std::getline(urlFile, url);
    if (urlFile.eof() || !urlFile)
      break;
    SharedData::mutex.lock();
    SharedData::urls.push(url);
    SharedData::mutex.unlock();
  }
}

void crawlUrls()
{
  while (true)
  {
    SharedData::mutex.lock();
    if (SharedData::urls.size() == 0)
    {
      SharedData::mutex.unlock();
      break;
    }
    auto url = SharedData::urls.front(); SharedData::urls.pop();
    SharedData::mutex.unlock();
    crawlUrl(url);
  }
}

std::shared_ptr<Arguments> parseArguments(int argc, char* argv[])
{
  auto arguments = new Arguments;
  if (argc < 2 || argc > 3)
  {
    print_usage();
    return nullptr;
  }
  try {
    arguments->NThreads = std::stoi(argv[1]);
  } catch (std::exception) {
    arguments->Url = argv[1];
    return std::shared_ptr<Arguments>(arguments);
  }
  arguments->FileName = argv[2];
  return std::shared_ptr<Arguments>(arguments);
}

int main(int argc, char* argv[])
{
  auto arguments = parseArguments(argc, argv);
  if (nullptr == arguments)
	  return 1;
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

  std::vector<std::thread> consumerThreads;

  printf("Collecting urls to crawl... ");
  // Collect urls
  collectUrlsToCrawl(urlFile, arguments);
  
  printf("done\n");

  printf("spawning threads...\n");
  // spawn consumer threads
  for (int i = 0; i < arguments->NThreads; ++i)
  {
    consumerThreads.push_back(std::thread(crawlUrls));
  }

  // wait for threads to exit
  for (auto& thread : consumerThreads)
  {
    thread.join();
  }

  Networking::DeInitWinsock();

  return 0;
}
