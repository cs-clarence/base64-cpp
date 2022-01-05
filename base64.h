#include <algorithm>
#include <array>
#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace base64
{
  constexpr const char *__index_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

  inline uint8_t
  __extract_b64_val(uint8_t bytemask, uint8_t byte, int8_t byteshift)
  {
    if (byteshift >= 0) return (bytemask & byte) >> byteshift;
    else return (bytemask & byte) << std::abs(byteshift);
  }

  template<class TChar = char>
  std::enable_if_t<std::is_integral_v<TChar>, std::basic_string<TChar>>
  encode(const std::vector<uint8_t> &bytes)
  {
    constexpr uint8_t __bytemasks[] {
      0b11111100u, // 1st b64 from 1st byte
      0b00000011u, // 2nd b64 p1 from 1st byte
      0b11110000u, // 2nd b64 p2 from 2nd byte
      0b00001111u, // 3rd b64 p1 from 2nd byte
      0b11000000u, // 3rd b64 p2 from 3rd byte
      0b00111111u, // 4th b64 from 3rd byte
    };

    constexpr int8_t __byteshifts[] {2, -4, 4, -2, 6, 0};

    std::basic_string<TChar> buffer;

    uint8_t temp = 0;
    for (uint64_t i = 0, end = bytes.size(); i < end; ++i)
    {
      uint8_t b64byteval = 0;

      switch (i % 3)
      {
      case 0:
        b64byteval
          = __extract_b64_val(__bytemasks[0], bytes[i], __byteshifts[0]);
        temp = __extract_b64_val(__bytemasks[1], bytes[i], __byteshifts[1]);
        break;

      case 1:
        b64byteval
          = __extract_b64_val(__bytemasks[2], bytes[i], __byteshifts[2]);
        b64byteval |= temp;
        temp = __extract_b64_val(__bytemasks[3], bytes[i], __byteshifts[3]);
        break;

      case 2:
        b64byteval
          = __extract_b64_val(__bytemasks[4], bytes[i], __byteshifts[4]);
        b64byteval |= temp;
        buffer.push_back(__index_table[b64byteval]);

        b64byteval
          = __extract_b64_val(__bytemasks[5], bytes[i], __byteshifts[5]);
        break;
      }

      buffer.push_back(__index_table[b64byteval]);

      if ((i == (end - 1)) && (bytes.size() % 3 != 0))
        buffer.push_back(__index_table[temp]);
    }

    if (size_t diff = buffer.size() % 4; diff != 0)
    {
      size_t initial_size = buffer.size();
      size_t final_size = initial_size + (4 - diff);

      for (size_t i = initial_size; i < final_size; ++i) buffer.push_back('=');
    }

    return buffer;
  }

  template<class TChar = char>
  std::enable_if_t<std::is_integral_v<TChar>, std::vector<uint8_t>>
  decode(const std::basic_string<TChar> &b64string)
  {
    static std::unordered_map<char, uint8_t> char_table;
    static bool map_initialized = false;

    if (!map_initialized)
    {
      for (size_t i = 0; __index_table[i]; ++i)
        char_table.insert(std::pair {__index_table[i], i});

      map_initialized = true;
    }

    std::vector<uint8_t> buffer;

    uint8_t temp = 0;
    for (size_t i = 0, end = b64string.size(); i < end && b64string[i] != '=';
         ++i)
    {
      uint8_t b64 = char_table[b64string[i]];
      uint8_t byteval = 0;
      bool pushbyte = false;

      switch (i % 4)
      {
      case 0: temp = b64 << 2; break; // get first 6 bits of byte 1
      case 1:
        byteval = (b64 & 0b0011'0000u) >> 4; // get last 2 bits of byte 1
        byteval |= temp;                     // combine the bits
        buffer.push_back(byteval);           // push byte 1
        temp = (b64 & 0b0000'1111u) << 4;    // get first 4 bits of byte 2
        break;
      case 2:
        byteval = (b64 & 0b0011'1100u) >> 2; // get first 4 bits of byte 2
        byteval |= temp;                     // get last 4 bits of byte 2
        temp = (b64 & 0b0000'0011u) << 6;    // get first 2 bits of byte 3
        buffer.push_back(byteval);           // push byte 2
        break;
      case 3:
        byteval = (b64 & 0b0011'1111u); // get last 6 bits of byte 3
        byteval |= temp;                // combine the bits of byte 3
        buffer.push_back(byteval);      // push byte 3
        break;
      default: break;
      }
    }
    return buffer;
  }

} // namespace base64
