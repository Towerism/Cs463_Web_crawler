// File: Stats.h
// Martin Fracker
// CSCE 463-500 Spring 2017
#pragma once
#include <atomic>

namespace SharedData
{
  typedef std::atomic<int> AtomicInt;
  typedef std::atomic<int> AtomicBool;
  namespace Stats
  {
    // getters
    int elapsedTime();
    int aliveThreads();
    int pendingUrls();
    int extractedUrls();
    int uniqueUrls();
    int dnsLookups();
    int uniqueIps();
    int nonRobotUrls();
    int crawledUrls();
    int crawledUrlsSize();
    int linksFound();
    int responses2xx();
    int responses3xx();
    int responses4xx();
    int responses5xx();
    int responsesOther();
    bool trackStats();

    // setters
    void elapsedTime(int set);
    void incrementAliveThreads();
    void decrementAliveThreads();
    void pendingUrls(int set);
    void incrementExtractedUrls();
    void incrementUniqueUrls();
    void incrementDnsLookups();
    void incrementUniqueIps();
    void incrementNonRobotUrls();
    void incrementCrawledUrls();
    void incrementCrawledUrlsSizeBy(int amount);
    void incrementLinksFoundBy(int amount);
    void incrementResponses2xx();
    void incrementResponses3xx();
    void incrementResponses4xx();
    void incrementResponses5xx();
    void incrementResponsesOther();
    void trackStats(bool set);
  };
};
