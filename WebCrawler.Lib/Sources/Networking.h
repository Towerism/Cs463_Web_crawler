/* winsock.cpp
 * CPSC 463 Sample Code
 * by Dmitri Loguinov
 */

#include <stdio.h>

#include <string>

namespace Networking
{
  class DNS
  {
  public:
    static void printDNSServer(void);
  };

  int InitWinsock();

  void DeInitWinsock();

  std::string ConnectToUrl(std::string host, int port, std::string request);
}
