#include "base64.h"

int main(int argc, const char **argv)
{
  std::vector<uint8_t> bytearray {
    'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '!'};

  const std::string encoded = base64::encode(bytearray);
  std::cout << "encoded size: " << encoded.size() << std::endl;
  std::cout << "encoded string: " << encoded << std::endl;
  std::vector<uint8_t> decoded = base64::decode(encoded);

  std::cout << "Decoded matched: " << (bytearray == decoded) << std::endl;
}
