/**
 * @file Common.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <cstdint>
#include <vector>

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b::ihex
{

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
} // namespace h2b::ihex
