/**
 * @file IntelHex.cpp
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
/* Includes -----------------------------------------------------------------*/
#include <h2b/IntelHex.hpp>
#include <h2b/utils/Helper.hpp>
#include <h2b/ihex/Tools.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <cctype>
#include <array>
/* Usings -------------------------------------------------------------------*/
using namespace h2b;

using utils::Files;
using ihex::Line;
using ihex::RecordType;

/* Constants ----------------------------------------------------------------*/

/* Public functions ---------------------------------------------------------*/
IntelHex::IntelHex(Files* files)
  : m_files(files)
  , m_bin2Intel(files, *this)
  , m_intel2Bin(files, *this)
{
}

/**
 * @brief Convert intel to binary.
 * 
 * @param[in] summary Print summary.
 * @retval false on error.
 */
auto IntelHex::intel2bin(bool summary) -> bool
{
  using enum RecordType;
  std::string input;
  std::uint32_t number = 0U;
  std::uint32_t writes = 0U;
  std::uint32_t prevAddress = 0U;
  std::uint32_t prevSegment = 0U;
  Line line{};
  startLinearFound() = false;
  auto leave = false;
  while(!leave && m_files->getline(input))
  {
    number++;
    if(!ihex::Intel2Bin::parseLine(input, line, number))
      leave = true;
    else
    {
      if(ExtendedLinear == line.type && !line.data.empty())
        m_intel2Bin.setCurrentAddress(m_intel2Bin.processAddressOrSegment(line, prevAddress, false, 24, 16));
      else if(ExtendedSegment == line.type && !line.data.empty())
        m_intel2Bin.setCurrentAddress(m_intel2Bin.processAddressOrSegment(line, prevSegment, true, 8));
      else if(Data == line.type && !line.data.empty())
        leave = !m_intel2Bin.processData(line, number, writes);
      else if(StartLinear == line.type && !line.data.empty())
        m_intel2Bin.processStartLinear(line, number);
      else if(EndOfFile == line.type)
      {
        m_intel2Bin.processEndOfFile(summary, number, writes);
        return true;
      }
    }
  }
  return false;
}

/**
 * @brief Convert binary to intel.
 * 
 * @param[in] summary Print summary.
 * @retval false on error.
 */
auto IntelHex::bin2intel(bool summary) -> bool
{
  if(paddingWidth())
    m_bin2Intel.fetchPadding();
  if(useSegment())
    m_bin2Intel.writeSegment(addressOffset());
  else
    m_bin2Intel.writeAddress(addressOffset());
  std::uint32_t writes;
  bool ret;
  if(paddingWidth() && !m_bin2Intel.paddings().empty())
    ret = m_bin2Intel.writeDataWithPadding(writes);
  else
    ret = m_bin2Intel.writeDataWithoutPadding(writes);
  m_bin2Intel.writeStartLinear();
  m_bin2Intel.writeEndOfFile();
  if(summary)
    m_bin2Intel.printSummary(writes);
  return ret;
}
