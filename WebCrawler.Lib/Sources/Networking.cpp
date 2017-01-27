/* Networking.cpp
 * CPSC 463 Sample Code
 * by Dmitri Loguinov
 * Modified by martin Fracker
 */

#include <stdio.h>

#include <signal.h>
#include <winsock2.h>
#include <string>
#include <sstream>
#include "Networking.h"
#include "common.h"
#include "IPHlpApi.h"
#include "ResponseParser.h"
#include "HTMLParserBase.h"
#include <memory>
#include <iostream>
#include <functional>

#define RESPONSE_CHUNK_SIZE 100
#define USER_AGENT_STRING "Cs463WebCrawler/1.2"
#define RESPONSE_TIMEOUT 10
#define DOWNLOAD_TIMEOUT 10
#define MAX_PAGE_SIZE 2097152
#define MAX_PAGE_SIZE_ROBOTS 16384
#define REQUEST_GET "GET"
#define REQUEST_HEAD "HEAD"

namespace Networking
{
  int InitWinsock()
  {
    WSADATA wsaData;

    //Initialize WinSock; once per program run
    WORD wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
      printf("WSAStartup error %d\n", WSAGetLastError());
      WSACleanup();
      return -1;
    }

    return 0;
  }

  void DeInitWinsock()
  {
    // call cleanup when done with everything and ready to exit program
    WSACleanup();
  }

  std::unordered_set<std::string> DNS::SeenHosts;
  std::unordered_set<DWORD> DNS::SeenIps;

  void DNS::printDNSServer(void)
  {
    // MSDN sample code
    FIXED_INFO* FixedInfo;
    ULONG ulOutBufLen;
    DWORD dwRetVal;
    IP_ADDR_STRING* pIPAddr;

    ulOutBufLen = sizeof(FIXED_INFO);
    FixedInfo = (FIXED_INFO *)GlobalAlloc(GPTR, sizeof(FIXED_INFO));
    ulOutBufLen = sizeof(FIXED_INFO);

    if (ERROR_BUFFER_OVERFLOW == GetNetworkParams(FixedInfo, &ulOutBufLen))
    {
      GlobalFree(FixedInfo);
      FixedInfo = (FIXED_INFO *)GlobalAlloc(GPTR, ulOutBufLen);
    }

    if (dwRetVal = GetNetworkParams(FixedInfo, &ulOutBufLen))
    {
      printf("failed with %08x\n", dwRetVal);
      std::exit(EXIT_FAILURE);
    }

    GlobalFree(FixedInfo);
  }

  bool DNS::MarkHostAsSeen(std::string host)
  {
    printf("\t  Checking host uniqueness... ");
    int prevSize = SeenHosts.size();
    SeenHosts.insert(host);
    bool unique = prevSize < SeenHosts.size();
    if (unique)
    {
      printf("passed\n");
    }
    else
    {
      printf("failed\n");
    }
    return unique;
  }

  bool DNS::MarkIpAsSeen(DWORD ip)
  {
    printf("\t  Checking ip uniqueness... ");
    int prevSize = SeenIps.size();
    SeenIps.insert(ip);
    bool unique = prevSize < SeenIps.size();
    if (unique)
    {
      printf("passed\n");
    }
    else
    {
      printf("failed\n");
    }
    return unique;
  }

  std::shared_ptr<ResponseParseResult> badParseResult(new ResponseParseResult);
  std::shared_ptr<ResponseParseResult> CloseSocketAndReturnBadParseResult(int socket)
  {
    closesocket(socket);
    return badParseResult;
  }

  // if response family is 2, then any response matching 2xx is successful, if 4 -> 4xx, if 5 -> 5xx, etc.
  // dumps a copy of the header into header pointer if nullptr is not passed
  std::shared_ptr<ResponseParseResult> RequestAndVerifyHeader(std::string message, std::string verb, std::string host, std::string request, sockaddr_in server, int successfulResponseFamily, int maxDownload)
  {
    // open a TCP socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
      printf("socket() generated error %d\n", WSAGetLastError());
      WSACleanup();
      return CloseSocketAndReturnBadParseResult(sock);
    }
    printf(message.c_str());
    DWORD t = timeGetTime();
    // connect to the server on port 80
    if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
      printf("failed with %d on connect\n", WSAGetLastError());
      return CloseSocketAndReturnBadParseResult(sock);
    }

    // build GET request
    std::ostringstream requestStream;
    requestStream << verb << " " << request << " HTTP/1.0\r\n"
      << "Host: " << host << "\r\n"
      << "User-agent: " << USER_AGENT_STRING << "\r\n"
      << "Connection: close\r\n"
      << "\r\n";
    auto tmp = requestStream.str();
    auto getRequest = tmp.c_str();

    // send HTTP requests here
    send(sock, getRequest, strlen(getRequest), 0);
    printf("done in %d ms\n", timeGetTime() - t);

    // collect response from server
    printf("\t  Loading... ");
    t = timeGetTime();
    std::ostringstream responseStream;
    int responseLength;
    int totalLength = 0;
    fd_set readfds;
    int selection;
    TIMEVAL timeout;
    timeout.tv_sec = RESPONSE_TIMEOUT;
    timeout.tv_usec = 0;
    int elapsedTime;
    do
    {
      char responseChunkBuffer[RESPONSE_CHUNK_SIZE];
      FD_ZERO(&readfds);
      FD_SET(sock, &readfds);
      selection = select(0, &readfds, nullptr, nullptr, &timeout);
      if (selection < 0)
      {
        printf("failed with %d on select\n", errno);
        return CloseSocketAndReturnBadParseResult(sock);
      }
      if (FD_ISSET(sock, &readfds))
      {
        responseLength = recv(sock, responseChunkBuffer, RESPONSE_CHUNK_SIZE, 0);
        if (responseLength < 0)
        {
          printf("failed with %d on recv\n", errno);
          return CloseSocketAndReturnBadParseResult(sock);
        }
        elapsedTime = timeGetTime() - t;
        if (responseLength > 0 && elapsedTime / 1000 > DOWNLOAD_TIMEOUT)
        {
          printf("failed with slow download\n");
          return CloseSocketAndReturnBadParseResult(sock);
        }
        auto responseChunk = std::string(responseChunkBuffer, responseLength);
        responseStream << responseChunk;
      }
      else
      {
        printf("response timed out\n");
        return CloseSocketAndReturnBadParseResult(sock);
      }
      totalLength += responseLength;
      if (totalLength > maxDownload)
      {
        printf("failed with exceeding max\n");
        return CloseSocketAndReturnBadParseResult(sock);
      }
    }
    while (responseLength > 0);
    auto response = responseStream.str();
    ResponseParser responseParser;
    auto responseParseResult = std::shared_ptr<ResponseParseResult>(responseParser.Parse(response));
    if (!responseParseResult->Success)
    {
      printf("failed with non-HTTP header\n");
      return CloseSocketAndReturnBadParseResult(sock);
    }
    printf("done in %d ms with %d bytes\n", timeGetTime() - t, response.length());

    // parse for links
    printf("\t  Verifying header... status code %d\n", responseParseResult->StatusCode);
    int responseFloor = successfulResponseFamily * 100;
    int responseCeiling = responseFloor + 100;
    if (responseParseResult->StatusCode >= responseFloor && responseParseResult->StatusCode < responseCeiling)
    {
      closesocket(sock);
      return responseParseResult;
    }
    // this line only matters for single threaded basic operation mode
    badParseResult->Header = responseParseResult->Header;
    return CloseSocketAndReturnBadParseResult(sock);
  }

  // dumps a copy of the page header into header pointer if nullptr is not passed
  bool ConnectToUrl(std::string host, int port, std::string request, std::string* header)
  {
    // string pointing to an HTTP server (DNS name or IP)
    auto str = host.c_str();

    if (!DNS::MarkHostAsSeen(host))
    {
      return false;
    }

    DNS::printDNSServer();

    // structure used in DNS lookups
    struct hostent* remote;

    // structure for connecting to server
    struct sockaddr_in server;
    DWORD IP = inet_addr(str);
    DWORD t;
    // first assume that the string is an IP address
    if (IP == INADDR_NONE)
    {
      printf("\t  Doing DNS... ");
      t = timeGetTime();
      // if not a valid IP, then do a DNS lookup
      if ((remote = gethostbyname(str)) == NULL)
      {
        printf("failed with unresolved name\n");
        return false;
      }
      // take the first IP address and copy into sin_addr
      memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
      std::string ip = inet_ntoa(server.sin_addr);
      printf("done in %d ms, found %s\n", timeGetTime() - t, ip.c_str());
    }
    else
    {
      // if a valid IP, directly drop its binary version into sin_addr
      server.sin_addr.S_un.S_addr = IP;
    }
    if (!DNS::MarkIpAsSeen(server.sin_addr.S_un.S_addr))
    {
      return false;
    }

    // setup the port # and protocol type
    server.sin_family = AF_INET;
    server.sin_port = htons(port); // host-to-network flips the byte order

    if (header == nullptr) {
      std::shared_ptr<ResponseParseResult> robotsParseResult;
      robotsParseResult = RequestAndVerifyHeader("\t  Connecting on robots... ", REQUEST_HEAD, host, "/robots.txt", server, 4, MAX_PAGE_SIZE_ROBOTS);
      if (!robotsParseResult->Success)
        return false;
    }

    std::shared_ptr<ResponseParseResult> responseParseResult;
    responseParseResult = RequestAndVerifyHeader("\t* Connecting on page... ", REQUEST_GET, host, request, server, 2, MAX_PAGE_SIZE);
    if (header != nullptr)
      header->replace(0, std::string::npos, responseParseResult->Header);

    if (responseParseResult->Success)
    {
      printf("\t+ Parsing page... ");
      t = timeGetTime();
      HTMLParserBase htmlParser;
      int linkCount;
      auto contentLength = responseParseResult->Content.length();
      char* content = new char[contentLength + 1];
      auto httpHost = "http://" + host;
      char* baseUrl = new char[httpHost.length() + 1];
      strcpy(content, responseParseResult->Content.c_str());
      strcpy(baseUrl, httpHost.c_str());
      htmlParser.Parse(content, contentLength, baseUrl, httpHost.length(), &linkCount);
      linkCount = max(linkCount, 0);
      printf("done in %d ms with %d links\n", timeGetTime() - t, linkCount);
      delete[] content;
      delete[] baseUrl;
    }

    return true;
  }
}
