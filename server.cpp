//===============================================================
//
//  Author : Brysen Landis
//
//  Description : This is my implementation of websockets using TCP
//                and IPv4 IP addresses.
//
//  Overview :
//          The program runs a single-threaded TCP server that accepts
//          one client connection at a time. For each connection it:
//            1. Reads the raw HTTP request into a buffer.
//            2. Parses it into an HttpRequest struct (method, path, headers).
//            3. Checks whether the request is a WebSocket upgrade request
//               (Upgrade: websocket + Connection: Upgrade headers).
//            4. Sends back a plain HTTP 200 response (WebSocket handshake
//               response is not sent yet).
//            5. Closes the connection and loops back to accept() again.
//
//          NOTE: The block below the write() call (Parser/base64/pad tests)
//          is temporary scaffolding used to exercise the Parser and
//          SocketHandShake code while it's being built. It runs on every
//          request and should be removed once those components are wired
//          into the real handshake flow.
//
//  Documentation :
//          Standard library / syscall functions used:
//
//            - socket()
//                socket(int domain, int type, int protocol)
//                  domain   - address family to use, e.g. AF_INET (IPv4)
//                             or AF_INET6 (IPv6)
//                  type     - socket type, e.g. SOCK_STREAM (TCP) or
//                             SOCK_DGRAM (UDP)
//                  protocol - protocol to use; 0 lets the OS pick the
//                             default protocol for the given type
//                  returns  - a socket file descriptor (>= 0) on success,
//                             or -1 on error (errno is set)
//
//            - bind()
//                bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
//                  sockfd  - the socket file descriptor to bind
//                  addr    - pointer to a sockaddr struct (cast from a
//                            sockaddr_in for IPv4) containing the IP and port
//                            to bind the socket to
//                  addrlen - the size in bytes of the struct pointed to by addr
//                  returns - 0 on success, -1 on error (errno is set)
//
//            - listen()
//                listen(int sockfd, int backlog)
//                  sockfd  - the socket file descriptor to listen on
//                  backlog - the maximum number of pending connections the
//                            OS should queue up before refusing new ones
//                  returns - 0 on success, -1 on error (errno is set)
//
//            - accept()
//                accept(int socket, struct sockaddr *address, socklen_t *address_len)
//                  socket      - the listening socket file descriptor
//                  address     - either nullptr, or a sockaddr struct that
//                                will be filled in with the connecting
//                                client's address
//                  address_len - in/out socklen_t*: caller sets it to the
//                                size of the address buffer; the call sets
//                                it to the actual size of the address written
//                  returns     - a new file descriptor for the accepted
//                                connection on success, or -1 on error
//
//            - read()
//                read(int socket, void *buffer, size_t buffer_size)
//                  socket      - the file descriptor to read from
//                  buffer      - the buffer to read data into
//                  buffer_size - the maximum number of bytes to read
//                  returns     - the number of bytes read (0 means the peer
//                                closed the connection), or -1 on error
//
//            - write()
//                write(int socket, const void *buffer, size_t buffer_size)
//                  socket      - the file descriptor to write to
//                  buffer      - the data to send
//                  buffer_size - the number of bytes to send from buffer
//                  returns     - the number of bytes actually written, or
//                                -1 on error (a successful call may write
//                                fewer bytes than requested)
//
//          Project-specific functions used here (see their own headers
//          for full documentation):
//
//            - Parser::splitLines(const std::string& raw)
//                Splits a raw HTTP request string into individual lines
//                (split on CRLF). Used here only for a quick smoke test.
//
//            - Parser::parse(const std::string& raw)
//                Parses a raw HTTP request string into an HttpRequest
//                struct containing the method, path, and a header map.
//
//            - SocketHandShake::base64Encode(const std::vector<uint8_t>& data)
//                Encodes raw bytes as a base64 string. Will be used to
//                build the Sec-WebSocket-Accept header during the
//                WebSocket handshake.
//
//            - SocketHandShake::pad(const std::vector<uint8_t>& data)
//                Pads a byte sequence to the block size required by the
//                handshake/encoding step.
//
//===============================================================

#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "parser.h"
#include "socket-handshake.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

int main()
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);   // sockfd is short for socket file descriptor
  if (sockfd == -1)
  {
    std::cerr << "socket() failed\n";
    return 1;
  }
  std::cout << "Got socket fd: " << sockfd << '\n';

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = INADDR_ANY;

  int result = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
  if (result == -1)
  {
    std::cerr << "bind() failed\n";
    return 1;
  }
  std::cout << "bind() succeeded\n";

  int server_addr = listen(sockfd, 10);
  if (server_addr == -1)
  {
    std::cerr << "listen() failed\n";
    return 1;
  }
  std::cout << "listen() succeeded\n";

  while (1)
  {
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(sockfd, (struct sockaddr*)&client_addr,
                         &client_addr_len);
    if (client_fd == -1)
    {
      std::cerr << "accept() failed\n";
      continue;
    }
    std::cout << "Accepted client, fd: " << client_fd << '\n';

    char buffer[1024];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read == -1)
    {
      std::cerr << "read() failed\n";
      close(client_fd);
      continue;
    }
    buffer[bytes_read] = '\0';
    std::cout << "Read " << bytes_read << " bytes:\n" << buffer << '\n';

    std::string body = "WORKING HTTP GET AND POST SERVER";
    std::string resp1 = "HTTP/1.1 200 OK\r\n"
                        "Content-Length: " + std::to_string(body.size()) + "\r\n"
                        "\r\n" +
                        body;
    ssize_t bytes_written = write(client_fd, resp1.c_str(), resp1.size());
    if (bytes_written == -1)
    {
      std::cerr << "write() failed\n";
      close(client_fd);
      continue;
    }
    std::cout << "Wrote " << bytes_written << " bytes\n";

    close(client_fd);

    // TODO: remove — test/smoke-test code for Parser and SocketHandShake,
    // not part of the real request-handling flow.
    std::string parserTest = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    std::vector<std::string> lines = Parser::splitLines(parserTest);

    for (const std::string& line : lines)
    {
      std::cout << "[" << line << "]\n";
    }

    buffer[bytes_read] = '\0';
    HttpRequest req = Parser::parse(buffer);
    std::cout << "Method: " << req.method << ", Path: " << req.path << "\n";

    if (req.headers["Upgrade"] == "websocket" &&
        req.headers["Connection"].find("Upgrade") != std::string::npos)
    {
      std::cout << "Upgraded\n";
    }
    std::vector<uint8_t> testData = {'M', 'a', 'n'};
    std::string encoded = SocketHandShake::base64Encode(testData);
    std::cout << "Encoded: [" << encoded << "]\n";

    std::vector<uint8_t> test2 = {'M', 'a'};
    std::cout << "Encoded 'Ma': [" << SocketHandShake::base64Encode(test2) << "]\n";

    std::vector<uint8_t> test1 = {'M'};
    std::cout << "Encoded 'M': [" << SocketHandShake::base64Encode(test1) << "]\n";

    std::vector<uint8_t> abcTest = {'a', 'b', 'c'};
    std::vector<uint8_t> paddedResult = SocketHandShake::pad(abcTest);
    std::cout << "Padded size: " << paddedResult.size() << "\n";
    for (size_t i = 0; i < paddedResult.size(); i++)
    {
      printf("%02x ", paddedResult[i]);
    }
    std::cout << "\n";
  }

  close(sockfd);
  return 0;
}
