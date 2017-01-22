/* winsock.cpp
 * CPSC 463 Sample Code
 * by Dmitri Loguinov
 */

#include <stdio.h>

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

#define RESPONSE_CHUNK_SIZE 100
#define USER_AGENT_STRING "Cs463WebCrawler/1.1"
#define RESPONSE_TIMEOUT 10

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

  void DNS::printDNSServer(void)
  {
    printf("\t  Doing DNS... ");
    DWORD t = timeGetTime();
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
    else
    {
      printf("done in %d ms, found %s\n", timeGetTime() - t, FixedInfo->DnsServerList.IpAddress.String);
    }

    GlobalFree(FixedInfo);
  }

  ResponseParseResult ConnectToUrl(std::string host, int port, std::string request)
  {
    // string pointing to an HTTP server (DNS name or IP)
    auto str = host.c_str();

    // open a TCP socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
      printf("socket() generated error %d\n", WSAGetLastError());
      WSACleanup();
      closesocket(sock);
      std::exit(EXIT_FAILURE);
    }

    // structure used in DNS lookups
    struct hostent* remote;

    // structure for connecting to server
    struct sockaddr_in server;

    printf("\t* Connecting on page... ");
    DWORD t = timeGetTime();
    // first assume that the string is an IP address
    DWORD IP = inet_addr(str);
    if (IP == INADDR_NONE)
    {
      // if not a valid IP, then do a DNS lookup
      if ((remote = gethostbyname(str)) == NULL)
      {
        printf("failed with invalid url\n");
        closesocket(sock);
        std::exit(EXIT_FAILURE);
      }
      else // take the first IP address and copy into sin_addr
        memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
    }
    else
    {
      // if a valid IP, directly drop its binary version into sin_addr
      server.sin_addr.S_un.S_addr = IP;
    }

    // setup the port # and protocol type
    server.sin_family = AF_INET;
    server.sin_port = htons(port); // host-to-network flips the byte order

    // connect to the server on port 80
    if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
      printf("failed with :w%d\n", WSAGetLastError());
      closesocket(sock);
      std::exit(EXIT_FAILURE);
    }

    // build HEAD request
    std::ostringstream requestStream;
    requestStream << "GET " << request << " HTTP/1.0\r\n"
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
    fd_set readfds;
    int selection;
    TIMEVAL timeout;
    do
    {
      char responseChunkBuffer[RESPONSE_CHUNK_SIZE];
      FD_ZERO(&readfds);
      FD_SET(sock, &readfds);
      timeout.tv_sec = RESPONSE_TIMEOUT;
      selection = select(0, &readfds, nullptr, nullptr, nullptr);
      if (selection < 0)
      {
        printf("failed with %d on select\n", errno);
        std::exit(EXIT_FAILURE);
      }
      if (FD_ISSET(sock, &readfds)) {
        responseLength = recv(sock, responseChunkBuffer, RESPONSE_CHUNK_SIZE, 0);
        if (responseLength < 0) {
          printf("failed with %d on recv\n", errno);
          closesocket(sock);
          std::exit(EXIT_FAILURE);
        }
        auto time = timeGetTime() - t;
        auto responseChunk = std::string(responseChunkBuffer, responseLength);
        responseStream << responseChunk;
      }
    }
    while (responseLength > 0);
    auto response = responseStream.str();
    ResponseParser responseParser;
    auto responseParseResult = std::unique_ptr<ResponseParseResult>(responseParser.Parse(response));
    if (!responseParseResult->Success)
    {
      printf("failed with non-HTTP header\n");
      closesocket(sock);
      std::exit(EXIT_FAILURE);
    }
    printf("done in %d ms with %d bytes\n", timeGetTime() - t, response.length());

    // parse for links
    printf("\t  Verifying header... status code %d\n", responseParseResult->StatusCode);
    if (responseParseResult->StatusCode == 200) {
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

    // close the socket to this server; open again for the next one
    closesocket(sock);

    return *responseParseResult;
  }
}
