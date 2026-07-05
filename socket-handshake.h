#ifndef SOCKET_HANDSHAKE_H
#define SOCKET_HANDSHAKE_H
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

class SocketHandShake
{
  public:
    static std::string base64Encode(const std::vector<uint8_t>& data);
    // TODO: SHA-1 encoding

  private:
    // helper functions

};

#endif
