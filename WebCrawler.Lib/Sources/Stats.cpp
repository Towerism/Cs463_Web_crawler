// File: Stats.cpp
// Martin Fracker
// CSCE 463-500 Spring 2017

#include "Stats.h"

namespace SharedData {
  namespace Stats {
    AtomicInt _elapsedTime = 0;
    AtomicInt _aliveThreads = 0;
    AtomicInt _pendingUrls = 0;
    AtomicInt _extractedUrls = 0;
    AtomicInt _uniqueUrls = 0;
    AtomicInt _dnsLookups = 0;
    AtomicInt _uniqueIps = 0;
    AtomicInt _nonRobotUrls = 0;
    AtomicInt _crawledUrls = 0;
    AtomicInt _crawledUrlsSize = 0;
    AtomicInt _linksFound = 0;
    AtomicInt _responses2xx = 0;
    AtomicInt _responses3xx = 0;
    AtomicInt _responses4xx = 0;
    AtomicInt _responses5xx = 0;
    AtomicInt _responsesOther = 0;
    AtomicBool _trackStats = true;

    int elapsedTime() { return _elapsedTime.load();  }
    int aliveThreads() { return _aliveThreads.load(); }
    int pendingUrls() { return _pendingUrls.load();  }
    int extractedUrls() { return _extractedUrls.load(); }
    int uniqueUrls() { return _uniqueUrls.load(); }
    int dnsLookups() { return _dnsLookups.load(); }
    int uniqueIps() { return _uniqueIps.load(); }
    int nonRobotUrls() { return _nonRobotUrls.load(); }
    int crawledUrls() { return _crawledUrls.load(); }
    int crawledUrlsSize() { return _crawledUrlsSize.load(); }
    int linksFound() { return _linksFound.load(); }
    int responses2xx() { return _responses2xx.load(); }
    int responses3xx() { return _responses3xx.load(); }
    int responses4xx() { return _responses4xx.load(); }
    int responses5xx() { return _responses5xx.load(); }
    int responsesOther() { return _responsesOther.load(); }
    bool trackStats() { return _trackStats.load(); }

    void elapsedTime(int set) { _elapsedTime = set; }
    void incrementAliveThreads() { ++_aliveThreads; }
    void decrementAliveThreads() { --_aliveThreads; }
    void pendingUrls(int set) { _pendingUrls = set; }
    void incrementExtractedUrls() { ++_extractedUrls; }
    void incrementUniqueUrls() { ++_uniqueUrls; }
    void incrementDnsLookups() { ++_dnsLookups; }
    void incrementUniqueIps() { ++_uniqueIps; }
    void incrementNonRobotUrls() { ++_nonRobotUrls; }
    void incrementCrawledUrls() { ++_crawledUrls; }
    void incrementCrawledUrlsSizeBy(int amount) { _crawledUrlsSize += amount; }
    void incrementLinksFoundBy(int amount) { _linksFound += amount; }
    void incrementResponses2xx() { ++_responses2xx; }
    void incrementResponses3xx() { ++_responses3xx; }
    void incrementResponses4xx() { ++_responses4xx; }
    void incrementResponses5xx() { ++_responses5xx; }
    void incrementResponsesOther() { ++_responsesOther; }
    void trackStats(bool set) { _trackStats = set; }
  }
}