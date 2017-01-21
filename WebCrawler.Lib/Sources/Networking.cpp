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

#define RESPONSE_CHUNK_SIZE 8192
#define USER_AGENT_STRING "Cs463WebCrawler/1.1"

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
      printf("Call to GetNetworkParams failed. Return Value: %08x\n", dwRetVal);
    }
    else
    {
      printf("done in %d ms, found %s\n", timeGetTime() - t, FixedInfo->DnsServerList.IpAddress.String);
    }

    GlobalFree(FixedInfo);
  }

  std::string ConnectToUrl(std::string host, int port, std::string request)
  {
    // string pointing to an HTTP server (DNS name or IP)
    auto str = host.c_str();
    std::string header = "";

    // open a TCP socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
      printf("socket() generated error %d\n", WSAGetLastError());
      WSACleanup();
      return header;
    }

    // structure used in DNS lookups
    struct hostent* remote;

    // structure for connecting to server
    struct sockaddr_in server;

    // first assume that the string is an IP address
    DWORD IP = inet_addr(str);
    if (IP == INADDR_NONE)
    {
      // if not a valid IP, then do a DNS lookup
      if ((remote = gethostbyname(str)) == NULL)
      {
        printf("Invalid string: neither FQDN, nor IP address\n");
        return header;
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
    printf("\t* Connecting on page... ");
    DWORD t = timeGetTime();
    if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
      printf("Connection error: %d\n", WSAGetLastError());
      return header;
    }
    printf("done in %d ms\n", timeGetTime() - t);

    // build HEAD request
    std::ostringstream requestStream;
    requestStream << "HEAD " << request << " HTTP/1.0\r\n"
      << "Host: " << host << "\r\n"
      << "User-agent: " << USER_AGENT_STRING << "\r\n"
      << "Connection: close\r\n"
      << "\r\n";
    auto headRequest = requestStream.str().c_str();

    // send HTTP requests here
    send(sock, headRequest, requestStream.str().length(), 0);

    // collect response from server
    std::ostringstream responseStream;
    int responseLength;
    do
    {
      char responseChunkBuffer[RESPONSE_CHUNK_SIZE];
      responseLength = recv(sock, responseChunkBuffer, RESPONSE_CHUNK_SIZE, 0);
      auto responseChunk = std::string(responseChunkBuffer, responseLength);
      responseStream << responseChunk;
    }
    while (responseLength > 0);
    header = responseStream.str();

    // close the socket to this server; open again for the next one
    closesocket(sock);

    return header;
  }
}
