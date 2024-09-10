#include "Utils.h"
#include <random>
#include <cfloat>
#include <vector>
#include <string>
#include <cstdint>



namespace duckutils
{

  void getRandomBytes(int length, uint8_t *bytes)
  {
    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    ;
    int i;
    for (i = 0; i < length; i++)
    {
      bytes[i] = digits[getRandomNumber(36)];
    }
  }
  int getRandomNumber(int range)
  {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(1, std::nextafter(range, DBL_MAX));

    for (int i = 0; i < 16; ++i)
    {
      //std::cout << dist(mt) << "\n";
    }
    int num = dist(mt);
    return num;
  }

  // Note: This function is not thread safe
  std::string convertToHex(uint8_t *data, int size)
  {
    std::string buf = ""; // static to avoid memory leak
    buf.clear();
    buf.reserve(size * 2); // 2 digit hex
    const char *cs = "0123456789ABCDEF";
    for (int i = 0; i < size; i++)
    {
      uint8_t val = data[i];
      buf += cs[(val >> 4) & 0x0F];
      buf += cs[val & 0x0F];
    }
    return buf;
  }
  std::vector<uint8_t> convertFromHex(const std::string& hexString) {
    // Ensure the input string has an even length
    if (hexString.length() % 2 != 0) {
        throw std::invalid_argument("Hexadecimal string must have an even length.");
    }

    std::vector<uint8_t> bytes;
    bytes.reserve(hexString.length() / 2); // Reserve space for bytes

    // Function to convert a single hexadecimal character to its numeric value
    auto hexCharToValue = [](char c) -> uint8_t {
        if (c >= '0' && c <= '9') {
            return c - '0';
        } else if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }
        throw std::invalid_argument("Invalid hexadecimal character.");
    };

    for (size_t i = 0; i < hexString.length(); i += 2) {
        // Convert each pair of hex characters to a byte
        uint8_t highNibble = hexCharToValue(hexString[i]);
        uint8_t lowNibble = hexCharToValue(hexString[i + 1]);
        bytes.push_back((highNibble << 4) | lowNibble);
    }

    return bytes;
}
  uint32_t toUint32(std::vector<uint8_t> data)
  {
    uint32_t value = 0;

    value |= data[0] << 24;
    value |= data[1] << 16;
    value |= data[2] << 8;
    value |= data[3];
    return value;
  }
  uint32_t toUint32(const uint8_t *data)
  {
    uint32_t value = 0;

    value |= data[0] << 24;
    value |= data[1] << 16;
    value |= data[2] << 8;
    value |= data[3];
    return value;
  }
  
  std::vector<uint8_t> convertStringToVector (std::string line) 
  {
    
    std::vector<uint8_t> buffer;
    for (char str_char : line)
        buffer.push_back(uint8_t(str_char));
    return buffer;
  } 


  std::string convertVectorToString (std::vector<uint8_t> buffer) 
  {
      std::string str = "";
      for (int i = 0; i < buffer.size(); i++) {
          str += buffer[i];
      }
      return str;
  }

  std::vector <uint8_t> convert32BitToVector(uint32_t data) 
  {
    std::vector<uint8_t> newData;
    for(int i=0; i < 4; i++)
    {
      newData.push_back((data & (0x000000FF << (i*8))) >> (i*8));
    }
    return newData;
  }
  void printVector(std::vector<uint8_t> vec)
  {
    std::cout << "data: ";
    for (int i = 0; i < vec.size(); i++)
    {
      std::cout << vec.at(i);
    }
    std::cout << std::endl;
    
  }
  uint32_t countNumBytes(std::vector<uint8_t> buff)
  {
    return buff.size();
  }
  
}