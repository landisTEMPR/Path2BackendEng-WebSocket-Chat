#include <iostream>
#include "socket-handshake.h"

std::string SocketHandShake::base64Encode(const std::vector<uint8_t>& data)
{
  static const std::string alphabet =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  std::string result;

  size_t i = 0;
  
  for (i = 0; i + 3 <= data.size(); i += 3)
  {
    uint8_t b0 = data[i];
    uint8_t b1 = data[i + 1];
    uint8_t b2 = data[i + 2];
    
    uint8_t group1 = b0 >> 2;                        // keeps top 6 bits of b0
                            
    uint8_t group2 = ((b0 & 0x03) << 4) | (b1 >> 4); // isolates bottom 2 bits of b0
                                                     // shift to left by 4 bits
                                                     // then isolate top 4 bits of b1
                                                     // then finally OR the results 
                                                     
    uint8_t group3 = ((b1 & 0x0F) << 2) | (b2 >> 6);  // isolates bottom 4 of and combines
                                                     // top 2 bits of b2

    uint8_t group4 = b2 & 0x3F;

    result += alphabet[group1];
    result += alphabet[group2];
    result += alphabet[group3];
    result += alphabet[group4];
  }

  size_t remaining = data.size() - i;

  if (remaining == 1)
  {
    uint8_t b0 = data[i];
    uint8_t group1 = b0 >> 2;
    uint8_t group2 = ((b0 & 0x03) << 4);

    result += alphabet[group1];
    result += alphabet[group2];
    result += "=="; // 2 byte padding for when the bytes arent a even number
  }
  else if (remaining == 2) // 1 byte padding when the bytes arent a even number
  {
    uint8_t b0 = data[i];
    uint8_t b1 = data[i + 1];
    uint8_t group1 = b0 >> 2;
    uint8_t group2 = ((b0 & 0x03) << 4) | (b1 >> 4);
    uint8_t group3 = ((b1 & 0x0F) << 2);
     
    result += alphabet[group1];
    result += alphabet[group2];
    result += alphabet[group3];
    result += "=";

  }
  return result;
}
