/**
 * @file Intel2Bin.cpp
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
#include <h2b/ihex/Intel2Bin.hpp>
#include <h2b/utils/Helper.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <cctype>
#include <array>
/* Usings -------------------------------------------------------------------*/
using namespace h2b::ihex;

using h2b::utils::Files;
using h2b::utils::Helper;

/* Constants ----------------------------------------------------------------*/

template <typename T>
static constexpr auto abs(T x)
{
  return x < 0 ? -x : x;
}

/* Public functions ---------------------------------------------------------*/

Intel2Bin::Intel2Bin(utils::Files* files, Parameters& params)
  : m_files(files)
  , m_params(params)
{
}

/**
 * @brief Sets the current address.
 * 
 * @param[in] currentAddress The current address.
 */
auto Intel2Bin::setCurrentAddress(std::uint32_t currentAddress) -> void
{
  m_currentAddress = currentAddress;
}

/**
 * @brief Parses a line.
 * 
 * @param[in] input The input line.
 * @param[out] line The decoded lines.
 * @param[in] number The line number.
 * @retval false on error
 */
auto Intel2Bin::parseLine(std::string_view input, Line& line, std::uint32_t number) -> bool
{
  using enum RecordType;
  line = {};
  if(!parseLineValidate(input, number))
    return false;
  std::size_t offset = 1;
  line.length = Helper::hex2int<std::uint8_t>(input.substr(offset, LL_LEN));
  offset += LL_LEN;
  line.address = Helper::hex2int<std::uint16_t>(input.substr(offset, AAAA_LEN));
  offset += AAAA_LEN;
  auto tt = Helper::hex2int<std::uint8_t>(input.substr(offset, TT_LEN));
  offset += TT_LEN;
  if(!parseLineGetType(line, number, tt))
    return false;

  if(Data == line.type && 0 == line.length)
  {
    std::cerr << "Line " << number << " indicates that it contains data, but the data is not present." << std::endl;
    return false;
  }
  else if(Data == line.type && (offset + 2 + (line.length * 2)) > input.size())
  {
    std::cerr << "Line " << number << " indicates that it contains data, but there isn't enough space." << std::endl;
    return false;
  }
  else if(0 != line.length)
    extractDataFromString(input, line, offset);
  line.checksum = Helper::hex2int<std::uint8_t>(input.substr(offset, CC_LEN));

  /* Validate checksum. */
  auto sum = Tools::evalCRC(line);
  auto ret = true;
  if(sum != line.checksum)
  {
    std::cerr << "Line " << number << " contains an invalid checksum." << std::endl;
    ret = false;
  }
  return ret;
}

/**
 * @brief Preliminary line validation.
 * 
 * @param[in] input Line to be validated.
 * @param[in] number Line number.
 * @retval false on error.
 */
auto Intel2Bin::parseLineValidate(std::string_view input, std::uint32_t number) -> bool
{
  if(MIN_LINE_SIZE > input.size())
  {
    std::cerr << "Line " << number << " is smaller than the minimum frame size." << std::endl;
    return false;
  }
  else if(START_CHAR != input.at(0))
  {
    std::cerr << "Line " << number << " does not begin with ':'." << std::endl;
    return false;
  }
  return true;
}

/**
 * @brief Gets the line type.
 * 
 * @param[in] line The associated line.
 * @param[in] number Line number.
 * @param[in] tt The type read.
 * @retval false on error.
 */
auto Intel2Bin::parseLineGetType(Line& line, std::uint32_t number, std::uint8_t tt) -> bool
{
  using enum RecordType;
  if(static_cast<std::uint8_t>(Data) != tt && static_cast<std::uint8_t>(EndOfFile) != tt && static_cast<std::uint8_t>(ExtendedSegment) != tt &&
     static_cast<std::uint8_t>(ExtendedLinear) != tt && static_cast<std::uint8_t>(StartLinear) != tt)
  {
    std::cerr << "Line " << number << " contains an invalid record type." << std::endl;
    return false;
  }
  line.type = static_cast<RecordType>(tt);
  return true;
}

/**
 * @brief Displays a summary of the entry.
 * 
 * @param[in] number Number of lines.
 * @param[in] writes Number of written data.
 */
auto Intel2Bin::printSummary(std::uint32_t number, std::uint32_t writes) const -> void
{
  std::cout << "Intel HEX to binary." << std::endl;
  std::cout << std::dec << number << " lines parsed." << std::endl;
  std::cout << "Address offset 0x" << Helper::int2hex(m_params.addressOffset(), '0', 8) << "." << std::endl;
  if(m_params.startLinearFound())
    std::cout << "The main function is at address 0x" << Helper::int2hex(m_params.linear(), '0', m_startLinearSize * 2) << "." << std::endl;
  else
    std::cout << "The main function was not contained in the written data." << std::endl;
  std::cout << std::dec << writes << " bytes written." << std::endl;
}

/**
 * @brief Extracting data from a string.
 * 
 * @param[in] input The line to be analyzed.
 * @param[in,out] line The line containing the information and where to put the data.
 * @param[in,out] offset The offset.
 */
auto Intel2Bin::extractDataFromString(std::string_view input, Line& line, std::size_t& offset) -> void
{
  for(std::uint8_t i = 0U; i < (line.length * 2); i += DD_LEN)
  {
    auto dd = Helper::hex2int<std::uint8_t>(input.substr(offset, DD_LEN));
    line.data.emplace_back(dd);
    offset += DD_LEN;
  }
}

/**
 * @brief Data padding.
 * 
 * @param[in] length Padding length.
 */
auto Intel2Bin::applyPadding(std::uint32_t length) -> void
{
  for(std::uint32_t i = 0; i < length; i++)
    m_files->output() << m_params.padding();
}

/**
 * @brief Write data (if present) to output file.
 * 
 * @param[in] line The decoded line.
 * @param[in] number The line number.
 * @param[out] writes Number of written data.
 * @retval false on error.
 */
auto Intel2Bin::processData(const Line& line, std::uint32_t number, std::uint32_t& writes) -> bool
{
  auto length = line.length;
  std::uint32_t offset = 0U;
  if(m_currentAddress < m_params.addressOffset())
  {
    auto total = m_currentAddress + line.length;
    if(total > m_params.addressOffset())
    {
      /* we are in the current packet. */
      offset = total - m_params.addressOffset();
      length -= offset;
    }
    else
    {
#if 1
      std::cerr << "Off-range address on line " << number;
      std::cerr << ", address: 0x" << Helper::int2hex(m_currentAddress);
      std::cerr << ", offset: 0x" << Helper::int2hex(m_params.addressOffset()) << std::endl;
#endif
      m_currentAddress += line.length;
      /* next packet*/
      return true;
    }
  }
  else if(!m_params.addressOffset())
    m_params.addressOffset() = m_currentAddress;

  std::vector<char> v;
  std::ranges::copy(line.data, std::back_inserter(v));
  if(!m_files->output().write(v.data() + offset, length))
  {
    std::cerr << "Unable to write line " << std::dec << number << " to output file." << std::endl;
    return false;
  }
  m_currentAddress += line.length;
  writes += length;
  return true;
}

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
auto Intel2Bin::processAddressOrSegment(const Line& line, std::uint32_t& prev, bool isSegment, int shiftH, int shiftL) -> std::uint32_t
{
  auto bck = prev;
  m_fullAddress &= ~prev;

  if(!isSegment)
    prev = (line.data[0] << shiftH) | (line.data[1] << shiftL);
  else
    prev = (((line.data[0] << shiftH) | line.data[1]) << 4);
  m_fullAddress += prev;
  if(bck && (bck - prev) > ADDR_LIMIT)
  {
    auto length = abs(static_cast<std::int32_t>(bck - prev));
    applyPadding(length);
  }
  return m_fullAddress;
}

/**
 * @brief Processes the starts linear address.
 * 
 * @param[in] line The decoded line.
 * @param[in] number The line number.
 */
auto Intel2Bin::processStartLinear(const Line& line, std::uint32_t number) -> void
{
  m_startLinearSize = static_cast<std::uint8_t>(line.data.size());
  if(1U == line.data.size())
    m_params.linear() = line.data.at(0);
  else if(2U == line.data.size())
    m_params.linear() = Helper::toU16(line.data.data());
  else if(4U == line.data.size())
    m_params.linear() = Helper::toU32(line.data.data());
  else
  {
    std::cerr << "Line " << number << " contains a StartLinear field whose data is not supported." << std::endl;
    return;
  }
  m_params.startLinearFound() = true;
}

/**
 * @brief Processes the end of file.
 * 
 * @param[in] summary Print summary.
 * @param[in] number Number of lines.
 * @param[in] length Number of written data.
 */
auto Intel2Bin::processEndOfFile(bool summary, std::uint32_t number, std::uint32_t length) -> void
{
  m_files->flush();
  if(summary)
    printSummary(number, length);
}
