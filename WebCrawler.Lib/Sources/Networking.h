/* winsock.cpp
 * CPSC 463 Sample Code
 * by Dmitri Loguinov
 */

#include <stdio.h>

#include <string>
#include "ResponseParser.h"

namespace Networking
{
  class DNS
  {
  public:
    static void printDNSServer(void);
  };

  int InitWinsock();

  void DeInitWinsock();

  ResponseParseResult ConnectToUrl(std::string host, int port, std::string request);
}
