/* Networking.h
 * CPSC 463 Sample Code
 * by Dmitri Loguinov
 * Modified by Martin Fracker
 */

#include <stdio.h>

#include <string>
#include "ResponseParser.h"
#include <unordered_set>

namespace Networking
{
  class DNS
  {
  public:
    static void printDNSServer(void);
    // mark host as having been seen
    // returns false if the host had been seen before
    static bool MarkHostAsSeen(std::string host);
    // mark ip as having been seen
    // returns false if the ip had been seen before
    static bool MarkIpAsSeen(unsigned long ip);
  private:
    static std::unordered_set<std::string> SeenHosts;
    static std::unordered_set<unsigned long> SeenIps;
  };

  int InitWinsock();

  void DeInitWinsock();

  bool ConnectToUrl(std::string host, int port, std::string request, std::string* header = nullptr);
}
