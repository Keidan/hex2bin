/**
 * @file Bin2Intel.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 * 
 * See https://developer.arm.com/documentation/ka003292/latest/
 * :llaaaatt[dd...]cc
 * ':' is the colon that starts every Intel HEX record.
 * 'll' is the record-length field that represents the number of data bytes (dd) in the record.
 * 'aaaa' is the address field that represents the starting address for subsequent data in the record.
 * 'tt' is the field that represents the HEX record type, which may be one of the following:
 *    00 - data record.
 *    01 - end-of-file record.
 *    02 - extended segment address record.
 *    04 - extended linear address record.
 *    05 - start linear address record (MDK-ARM only).
 * 'dd' is a data field that represents one byte of data. 
 *      A record may have multiple data bytes.
 *      The number of data bytes in the record must match the number specified by the ll field.
 * 'cc' is the checksum field that represents the checksum of the record.
 *      The checksum is calculated by summing the values of all hexadecimal digit pairs in the record modulo 256
 *      and taking the two's complement.
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <h2b/utils/Files.hpp>
#include <h2b/ihex/Parameters.hpp>
#include <h2b/ihex/Tools.hpp>
#include <map>

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b::ihex
{
  class Bin2Intel
  {
    public:
      explicit Bin2Intel(utils::Files* m_files, const Parameters& params);
      virtual ~Bin2Intel() = default;

      /**
       * @brief Displays a summary of the entry.
       * 
       * @param[in] length Number of written data.
       */
      auto printSummary(std::uint32_t length) const -> void;

      /**
       * @brief Fetches the various paddings found in the file.
       */
      auto fetchPadding() -> void;

      /**
       * @brief Extracts padding info.
       * 
       * @param[in] c The current char.
       * @param[in] i The index.
       * @param[out] pstart Padding start.
       * @param[out] plen Padding length.
       */
      auto extactPaddingForFetch(std::uint8_t c, std::uint32_t i, std::uint32_t& pstart, std::uint32_t& plen) -> void;

      /**
       * @brief Writes the address.
       * 
       * @param[in] address The address.
       */
      auto writeAddress(std::uint32_t address) -> void;

      /**
       * @brief Writes the segment address.
       * 
       * @param[in] address The address.
       */
      auto writeSegment(std::uint32_t address) -> void;

      /**
       * @brief Write data (if present) to output file.
       * 
       * @param[out] writes Number of written data.
       * @retval false on error.
       */
      auto writeDataWithoutPadding(std::uint32_t& writes) -> bool;

      /**
       * @brief Write data (if present) to output file.
       * 
       * @param[out] writes Number of written data.
       * @retval false on error.
       */
      auto writeDataWithPadding(std::uint32_t& writes) -> bool;

      auto writeDataWithPaddingPartial(std::uint32_t& writes, std::streamsize& length, std::uint32_t& next, char* data, std::uint32_t start) -> bool;

      /**
       * @brief Write data segments to output file.
       * 
       * @param[in] data The data to be written.
       * @param[out] writes Number of written data.
       * @param[in] reads The segment length.
       * @param[out] length The total length.
       */
      auto writeDataSegments(const char* data, std::uint32_t& writes, std::uint32_t reads, std::streamsize& length) -> void;

      /**
       * @brief Write data segment to output file.
       * 
       * @param[in] data The data to be written.
       * @param[in] segmentLength The segment length.
       * @param[in] offset The data offset.
       */
      auto writeDataSegment(const char* data, std::uint32_t segmentLength, std::uint32_t offset) -> void;

      /**
       * @brief Rewrites the extended part (linear or segment).
       */
      auto rewriteExtended() -> void;

      /**
       * @brief Writes the starts linear address.
       */
      auto writeStartLinear() -> void;

      /**
       * @brief Writes the end of file.
       */
      auto writeEndOfFile() -> void;

      /**
       * @brief Returns the paddings.
       * 
       * @retval const std::map<std::uint32_t, std::uint32_t>&
       */
      auto paddings() const -> const std::map<std::uint32_t, std::uint32_t>&;

    private:
      utils::Files* m_files;
      const Parameters& m_params;
      std::uint32_t m_currentAddress = 0U;
      std::uint32_t m_fullAddress = 0U;
      std::map<std::uint32_t, std::uint32_t> m_paddings{};
  };
} // namespace h2b::ihex
