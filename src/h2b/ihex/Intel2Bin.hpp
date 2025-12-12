/**
 * @file IntelHex.hpp
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

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b::ihex
{
  class Intel2Bin
  {
    public:
      explicit Intel2Bin(utils::Files* files, Parameters& params);
      virtual ~Intel2Bin() = default;

      /**
       * @brief Sets the current address.
       * 
       * @param[in] currentAddress The current address.
       */
      auto setCurrentAddress(std::uint32_t currentAddress) -> void;

      /**
       * @brief Parses a line.
       * 
       * @param[in] input The input line.
       * @param[out] line The decoded lines.
       * @param[in] number The line number.
       * @retval false on error
       */
      static auto parseLine(std::string_view input, Line& line, std::uint32_t number = 1) -> bool;

      /**
       * @brief Preliminary line validation.
       * 
       * @param[in] input Line to be validated.
       * @param[in] number Line number.
       * @retval false on error.
       */
      static auto parseLineValidate(std::string_view input, std::uint32_t number) -> bool;

      /**
       * @brief Gets the line type.
       * 
       * @param[in] line The associated line.
       * @param[in] number Line number.
       * @param[in] tt The type read.
       * @retval false on error.
       */
      static auto parseLineGetType(Line& line, std::uint32_t number, std::uint8_t tt) -> bool;

      /**
       * @brief Displays a summary of the entry.
       * 
       * @param[in] number Number of lines.
       * @param[in] writes Number of written data.
       */
      auto printSummary(std::uint32_t number, std::uint32_t writes) const -> void;

      /**
       * @brief Extracting data from a string.
       * 
       * @param[in] input The line to be analyzed.
       * @param[in,out] line The line containing the information and where to put the data.
       * @param[in,out] offset The offset.
       */
      static auto extractDataFromString(std::string_view input, Line& line, std::size_t& offset) -> void;

      /**
       * @brief Data padding.
       * 
       * @param[in] length Padding length.
       */
      auto applyPadding(std::uint32_t length) -> void;

      /**
       * @brief Write data (if present) to output file.
       * 
       * @param[in] line The decoded line.
       * @param[in] number The line number.
       * @param[out] writes Number of written data.
       * @retval false on error.
       */
      auto processData(const Line& line, std::uint32_t number, std::uint32_t& writes) -> bool;

      /**
       * @brief Processes the address or segment address.
       * 
       * @param[in] line The decoded line.
       * @param[in] prev The previous (segment) address.
       * @param[in] isSegment Is segment?
       * @param[in] shiftH Shift position (high).
       * @param[in] shiftL Shift position (low).
       * @retval std::uint32_t
       */
      auto processAddressOrSegment(const Line& line, std::uint32_t& prev, bool isSegment, int shiftH, int shiftL = 0) -> std::uint32_t;

      /**
       * @brief Processes the starts linear address.
       * 
       * @param[in] line The decoded line.
       * @param[in] number The line number.
       */
      auto processStartLinear(const Line& line, std::uint32_t number) -> void;

      /**
       * @brief Processes the end of file.
       * 
       * @param[in] summary Print summary.
       * @param[in] number Number of lines.
       * @param[in] length Number of written data.
       */
      auto processEndOfFile(bool summary, std::uint32_t number, std::uint32_t length) -> void;

    private:
      utils::Files* m_files;
      Parameters& m_params;
      std::uint8_t m_startLinearSize = 0U;
      std::uint32_t m_currentAddress = 0U;
      std::uint32_t m_fullAddress = 0U;
  };
} // namespace h2b::ihex
