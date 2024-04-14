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
#include "Files.hpp"
#include "ihex/Parameters.hpp"
#include "ihex/Common.hpp"
#include <map>

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b
{
  class IntelHex : public ihex::Parameters
  {
    public:

      explicit IntelHex(Files* files);

      /**
       * @brief Convert intel to binary.
       * 
       * @param[in] summary Print summary.
       * @retval false on error.
       */
      auto intel2bin(bool summary = true) -> bool;

      /**
       * @brief Convert binary to intel.
       * 
       * @param[in] summary Print summary.
       * @retval false on error.
       */
      auto bin2intel(bool summary = true) -> bool;

      /**
       * @brief Gets the files pointer.
       * 
       * @retval Files*
       */
      auto files() const -> Files*;

      /**
       * @brief Parses a line.
       * 
       * @param[in] line The input line.
       * @param[out] line The decoded line.
       * @param[in] number The line number.
       * @retval false on error
       */
      static auto parseLine(std::string_view input, ihex::Line& line, std::uint32_t number = 1) -> bool;

      /**
       * @brief Converts a Line object to an intel line.
       * 
       * @param[in] line The line.
       * @retval std::string
       */
      static auto convertLine(const ihex::Line& line) -> std::string;

      /**
       * @brief Calculates the CRC.
       * 
       * @param[in] line The current line.
       * @retval The CRC.
       */
      static auto evalCRC(const ihex::Line& line) -> std::uint8_t;

    private:
      Files* m_files;
      std::uint8_t m_startLinearSize = 0U;
      std::uint32_t m_prevAddress = 0U;
      std::uint32_t m_prevSegment = 0U;
      std::uint32_t m_currentAddress = 0U;
      std::uint32_t m_fullAddress = 0U;
      std::map<std::uint32_t, std::uint32_t> m_paddings{};

      /* H2B -------------------------------------------------------------*/
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
      static auto parseLineGetType(ihex::Line& line, std::uint32_t number, std::uint8_t tt) -> bool;

      /**
       * @brief Displays a summary of the entry.
       * 
       * @param[in] number Number of lines.
       * @param[in] length Number of written data.
       */
      auto printSummaryH2B(std::uint32_t number, std::uint32_t length) const -> void;

      /**
       * @brief Extracting data from a string.
       * 
       * @param[in] input The line to be analyzed.
       * @param[in,out] line The line containing the information and where to put the data.
       * @param[in,out] offset The offset.
       */
      static auto extractDataFromString(std::string_view input, ihex::Line& line, std::size_t& offset) -> void;

      /**
       * @brief Data padding.
       * 
       * @param[in] length Padding length.
       */
      auto applyPadding(std::uint32_t length) -> void;
      /* H2B Process -----------------------------------------------------*/
      /**
       * @brief Write data (if present) to output file.
       * 
       * @param[in] line The decoded line.
       * @param[in] number The line number.
       * @param[out] writes Number of written data.
       * @retval false on error.
       */
      auto processData(const ihex::Line& line, std::uint32_t number, std::uint32_t& writes) -> bool;

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
      auto processAddressOrSegment(const ihex::Line& line, std::uint32_t& prev, bool isSegment, int shiftH, int shiftL = 0) -> std::uint32_t;

      /**
       * @brief Processes the starts linear address.
       * 
       * @param[in] line The decoded line.
       * @param[in] number The line number.
       */
      auto processStartLinear(const ihex::Line& line, std::uint32_t number) -> void;

      /**
       * @brief Processes the end of file.
       * 
       * @param[in] summary Print summary.
       * @param[in] number Number of lines.
       * @param[in] writes Number of written data.
       */
      auto processEndOfFile(bool summary, std::uint32_t number, std::uint32_t writes) -> void;

      /* B2H -------------------------------------------------------------*/
      /**
       * @brief Displays a summary of the entry.
       * 
       * @param[in] length Number of written data.
       */
      auto printSummaryB2H(std::uint32_t length) const -> void;

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
      /* B2H Writes ------------------------------------------------------*/
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
  };
} // namespace h2b
