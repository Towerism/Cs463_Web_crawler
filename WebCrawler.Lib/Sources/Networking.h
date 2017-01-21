/* winsock.cpp
 * CPSC 463 Sample Code
 * by Dmitri Loguinov
 */

#include <stdio.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string>

namespace Networking {

  int InitWinsock();

  void DeInitWinsock();

  std::string ConnectToUrl(std::string host, int port, std::string request);

}
