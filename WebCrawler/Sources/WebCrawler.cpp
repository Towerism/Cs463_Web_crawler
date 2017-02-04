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
#include "Stats.h"
#include <unordered_set>
#include <queue>
#include <mutex>
#include <atomic>
#include <windows.h>
#include "printing.h"

#define BYTES_PER_MEGABYTE 1048576

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
  UrlParser urlParser;
  printIfNoStats("URL: %s\n", url.c_str());
  auto urlParseResult = std::unique_ptr<UrlParseResult>(urlParser.Parse(url));

  if (!urlParseResult->Success)
    return false;

  auto result = Networking::ConnectToUrl(urlParseResult->Host, urlParseResult->Port, urlParseResult->Request, header);
  return result;
}

void reportStats()
{
  using namespace SharedData::Stats;
  int t = timeGetTime();
  do {
    using namespace std::chrono_literals;
    SharedData::mutex.lock();
    pendingUrls(SharedData::urls.size());
    SharedData::mutex.unlock();
    printf("[%3d] %4d Q %6d E %7d H %6d D %6d I %5d R %5d C %5d L %4d\n", elapsedTime(), aliveThreads(), pendingUrls(), extractedUrls(), uniqueUrls(), dnsLookups(), uniqueIps(), nonRobotUrls(), crawledUrls(), linksFound());
    std::this_thread::sleep_for(2s);
    elapsedTime((timeGetTime() - t) / 1000);
  } while (trackStats());
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
    SharedData::Stats::incrementExtractedUrls();
    SharedData::mutex.unlock();
    crawlUrl(url);
  }
  SharedData::Stats::decrementAliveThreads();
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
    SharedData::Stats::trackStats(false);
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

  // Collect urls
  collectUrlsToCrawl(urlFile, arguments);

  // start stats thread
  std::thread(reportStats).detach();

  // spawn consumer threads
  for (int i = 0; i < arguments->NThreads; ++i)
  {
    consumerThreads.push_back(std::thread(crawlUrls));
    SharedData::Stats::incrementAliveThreads();
  }

  // wait for threads to exit
  for (auto& thread : consumerThreads)
  {
    thread.join();
  }
  SharedData::Stats::trackStats(false);

  Networking::DeInitWinsock();

  using namespace SharedData::Stats;
  printf("Extracted %d URLs @ %.1f/s\n", extractedUrls(), double(extractedUrls()) / elapsedTime());
  printf("Looked up %d DNS names @ %.1f/s\n", dnsLookups(), double(dnsLookups()) / elapsedTime());
  printf("Downloaded %d robots @ %.1f/s\n", nonRobotUrls(), double(nonRobotUrls()) / elapsedTime());
  printf("Crawled %d pages @ %.1f/s (%.2f MB)\n", crawledUrls(), double(crawledUrls()) / elapsedTime(), double(crawledUrlsSize()) / BYTES_PER_MEGABYTE);
  printf("Parsed %d links @ %.1f/s\n", linksFound(), double(linksFound()) / elapsedTime());
  printf("HTTP codes: 2xx = %d, 3xx = %d, 4xx = %d, 5xx = %d, other = %d\n", 0, 0, 0, 0, 0);

  return 0;
}
