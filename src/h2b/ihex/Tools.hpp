/**
 * @file Types.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <cstdint>
#include <vector>
#include <h2b/utils/Helper.hpp>

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b::ihex
{
  static constexpr auto START_CHAR = ':';
  static constexpr std::size_t LL_LEN = 2U;
  static constexpr std::size_t AAAA_LEN = 4U;
  static constexpr std::size_t TT_LEN = 2U;
  static constexpr std::size_t DD_LEN = 2U;
  static constexpr std::size_t CC_LEN = 2U;
  static constexpr std::size_t MIN_LINE_SIZE = LL_LEN + AAAA_LEN + TT_LEN + CC_LEN;
  static constexpr std::size_t BUFFER_SIZE = 4096;
  static constexpr std::uint32_t ADDR_LIMIT = (0xFFFFU + 1);

  enum class RecordType : std::uint8_t
  {
    Data = 0x00,
    EndOfFile = 0x01,
    ExtendedSegment = 0x02,
    ExtendedLinear = 0x04,
    StartLinear = 0x05,
  };

  using Buffer = std::vector<std::uint8_t>;

  struct Line
  {
      std::uint8_t length = 0;
      std::uint16_t address = 0;
      RecordType type;
      Buffer data{};
      std::uint8_t checksum = 0;
  };

  class Tools
  {
    public:
      virtual ~Tools() = default;

      /**
       * @brief Converts a Line object to an intel line.
       * 
       * @param[in] line The line.
       * @retval std::string
       */
      static auto convertLine(const Line& line) -> std::string
      {
        std::uint32_t address = line.address;
        if(RecordType::Data != line.type)
          address = 0U;
        std::stringstream ss;
        ss << ":";
        ss << utils::Helper::int2hex(line.data.size(), '0', 2);
        ss << utils::Helper::int2hex(address, '0', 4);
        ss << utils::Helper::int2hex(+static_cast<std::uint8_t>(line.type), '0', 2);
        for(const auto& dd : line.data)
          ss << utils::Helper::int2hex(+dd, '0', 2);
        ss << utils::Helper::int2hex(+line.checksum, '0', 2);
        ss << "\n";
        return ss.str();
      }

      /**
       * @brief Calculates the CRC.
       * 
       * @param[in] line The current line.
       * @retval The CRC.
       */
      static auto evalCRC(const Line& line) -> std::uint8_t
      {
        auto sum = line.length;
        sum += static_cast<std::uint8_t>(line.type) + (line.address & 0xFFU) + ((line.address >> 8) & 0xFFU);
        for(const auto& c : line.data)
          sum += c;
        sum = ~sum + 1;
        return sum;
      }

    private:
      Tools() = default;
  };
} // namespace h2b::ihex
