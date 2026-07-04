//===============================================================
//
//  Author : Brysen Landis
//   
//  Description : This is my implementation of websockets using TCP
//                and IPv4 IP addresses.
//
//
//  Documentation :
//          Functions:
//            - socket() accepts 3 parameters 
//                socket(int domain, int type, int protocol)
//                  domain - what kind of addresses will this use. IPv4 or IPv6
//                  type - what kind of connection do we want. TCP or UDP
//                  protocol - just set to 0. idk what this does.
//
//            - bind()
//                bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
//                  sockfd - the socket we're binding to
//                  (struct sockaddr*)&addr - a pointer to a struct 
//                                            containing the IP & port
//                  sizeof(addr) - the size in bytes of the structed we passed in
//            
//            - listen() 
//                listen() has two parameters for it's signature
//                  sockfd - the name of the socket we're using
//                  backlog - the maximum number of pending connections the OS
//                            should queue up
//            
//            - accept()
//                accept() has 3 parameters for it's signature
//                  socket - see above
//                  address - either nullptr or a sockaddr struct of the address of the
//                            connecting socket
//                  address_len - socklen_t struct where the input specifies the length
//                                of the sockaddr struct
//                  
//
//===============================================================

#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include <iostream>

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
    std::cerr << "listen() fialed\n";
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

    close(client_fd);
  }
  close(sockfd);
  return 0;
}



