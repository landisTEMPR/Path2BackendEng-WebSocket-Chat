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
    static std::string pad(const std::vector<uint8_t>& messasge);
    static std::string sha1Encode(const std::vector<uint8_t>& data);

  private:
    // helper functions

};

#endif
