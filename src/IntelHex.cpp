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
#include "IntelHex.hpp"
#include "Helper.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <cctype>
#include <array>
/* Usings -------------------------------------------------------------------*/
using namespace h2b;

static constexpr auto START_CHAR = ':';
static constexpr std::size_t LL_LEN = 2U;
static constexpr std::size_t AAAA_LEN = 4U;
static constexpr std::size_t TT_LEN = 2U;
static constexpr std::size_t DD_LEN = 2U;
static constexpr std::size_t CC_LEN = 2U;
static constexpr std::size_t MIN_LINE_SIZE = LL_LEN + AAAA_LEN + TT_LEN + CC_LEN;
static constexpr std::size_t BUFFER_SIZE = 4096;
static constexpr std::uint32_t ADDR_LIMIT = (0xFFFFU + 1);

template<typename T>
static constexpr auto abs(T x)
{
  return x < 0 ? -x : x;
}

IntelHex::IntelHex(Files* files)
  : m_files(files)
{
}

/**
 * @brief Sets the width (used for writing).
 * 
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto IntelHex::width(std::string_view value, std::string& what) -> bool
{
  auto ret = Helper::setValueFromstring(m_width, value, what);
  if(ret)
  {
    if(!m_width)
      what = "The value cannot be equal to 0.";
    else if(m_width > 0xFF)
      what = "The value cannot exceed 255 (0xFF).";
    ret = what.empty();
  }
  return ret;
}

/**
 * @brief Gets the width (used for writing).
 * 
 * @retval std::uint32_t.
 */
auto IntelHex::width() const -> std::uint32_t
{
  return m_width;
}

/**
 * @brief Sets the padding.
 * 
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto IntelHex::padding(std::string_view value, std::string& what) -> bool
{
  std::uint32_t padding;
  auto ret = Helper::setValueFromstring(padding, value, what);
  if(ret)
  {
    if(padding > 0xFF)
      what = "The padding value cannot exceed 255 (0xFF).";
    else
      m_padding = static_cast<std::uint8_t>(padding);
    ret = what.empty();
  }
  return ret;
}

/**
 * @brief Gets the padding.
 * 
 * @retval std::uint8_t.
 */
auto IntelHex::padding() const -> std::uint8_t
{
  return m_padding;
}

/**
 * @brief Sets the padding width.
 * 
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto IntelHex::paddingWidth(std::string_view value, std::string& what) -> bool
{
  std::uint32_t padding;
  auto ret = Helper::setValueFromstring(padding, value, what);
  if(ret)
  {
    if(padding > 0xFF)
      what = "The padding width value cannot exceed 255 (0xFF).";
    else
      m_paddingWidth = padding;
    ret = what.empty();
  }
  return ret;
}

/**
 * @brief Gets the padding width.
 * 
 * @retval std::uint8_t.
 */
auto IntelHex::paddingWidth() const -> std::uint8_t
{
  return m_paddingWidth;
}

/**
 * @brief Sets the address offset.
 * 
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto IntelHex::offset(std::string_view value, std::string& what) -> bool
{
  return Helper::setValueFromstring(m_addrOffset, value, what);
}

/**
 * @brief Gets the address offset.
 * 
 * @retval std::uint32_t.
 */
auto IntelHex::offset() const -> std::uint32_t
{
  return m_addrOffset;
}

/**
 * @brief Sets the start linear.
 * 
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto IntelHex::linear(std::string_view value, std::string& what) -> bool
{
  auto r = Helper::setValueFromstring(m_startLinear, value, what);
  return (m_startLinearFound = r);
}

/**
 * @brief Gets the start linear.
 * 
 * @retval std::uint32_t.
 */
auto IntelHex::linear() const -> std::uint32_t
{
  return m_startLinear;
}

/**
 * @brief Use of the "extended segment" block instead of "extended linear" for the offset parameter.
 */
auto IntelHex::segment() -> void
{
  m_useSegment = true;
}

/**
 * @brief Gets the files pointer.
 * 
 * @retval Files*
 */
auto IntelHex::files() const -> Files*
{
  return m_files;
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
  Line line{};
  m_startLinearFound = false;
  while(m_files->getline(input))
  {
    number++;
    if(!parseLine(input, line, number))
      break;
    if(ExtendedLinear == line.type && !line.data.empty())
    {
      m_currentAddress = processAddressOrSegment(line, m_prevAddress, false, 24, 16);
    }
    else if(ExtendedSegment == line.type && !line.data.empty())
    {
      m_currentAddress = processAddressOrSegment(line, m_prevAddress, true, 8);
    }
    else if(Data == line.type && !line.data.empty())
    {
      if(!processData(line, number, writes))
        break;
      m_currentAddress += line.length;
    }
    else if(StartLinear == line.type && !line.data.empty())
      processStartLinear(line, number);
    else if(EndOfFile == line.type)
    {
      processEndOfFile(summary, number, writes);
      return true;
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
  if(m_paddingWidth)
    fetchPadding();
  if(m_useSegment)
    writeSegment(m_addrOffset);
  else
    writeAddress(m_addrOffset);
  std::uint32_t writes;
  bool ret;
  if(m_paddingWidth && !m_paddings.empty())
    ret = writeDataWithPadding(writes);
  else
    ret = writeDataWithoutPadding(writes);
  writeStartLinear();
  writeEndOfFile();
  if(summary)
    printSummaryB2H(writes);
  return ret;
}

/**
 * @brief Parses a line.
 * 
 * @param[in] input The input line.
 * @param[out] line The decoded lines.
 * @param[in] number The line number.
 * @retval false on error
 */
auto IntelHex::parseLine(std::string_view input, Line& line, std::uint32_t number) -> bool
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
  {
    extractDataFromString(input, line, offset);
  }
  line.checksum = Helper::hex2int<std::uint8_t>(input.substr(offset, CC_LEN));

  /* Validate checksum. */
  auto sum = evalCRC(line);
  auto ret = true;
  if(sum != line.checksum)
  {
    std::cerr << "Line " << number << " contains an invalid checksum." << std::endl;
    ret = false;
  }
  return ret;
}

/**
 * @brief Converts a Line object to an intel line.
 * 
 * @param[in] line The line.
 * @retval std::string
 */
auto IntelHex::convertLine(const Line& line) -> std::string
{
  std::uint32_t address = line.address;
  if(RecordType::Data != line.type)
    address = 0U;
  std::stringstream ss;
  ss << ":";
  ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << line.data.size();
  ss << std::uppercase << std::hex << std::setfill('0') << std::setw(4) << address;
  ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << +static_cast<std::uint8_t>(line.type);
  for(const auto& dd : line.data)
    ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << +dd;
  ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << +line.checksum;
  ss << "\n";
  return ss.str();
}

/**
 * @brief Calculates the CRC.
 * 
 * @param[in] line The current line.
 * @retval The CRC.
 */
auto IntelHex::evalCRC(const Line& line) -> std::uint8_t
{
  std::uint8_t sum = line.length;
  sum += static_cast<std::uint8_t>(line.type) + (line.address & 0xFFU) + ((line.address >> 8) & 0xFFU);
  for(const auto& c : line.data)
    sum += c;
  sum = ~sum + 1;
  return sum;
}

/* H2B -------------------------------------------------------------*/
/**
 * @brief Preliminary line validation.
 * 
 * @param[in] input Line to be validated.
 * @param[in] number Line number.
 * @retval false on error.
 */
auto IntelHex::parseLineValidate(std::string_view input, std::uint32_t number) -> bool
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
auto IntelHex::parseLineGetType(Line& line, std::uint32_t number, std::uint8_t tt) -> bool
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
auto IntelHex::printSummaryH2B(std::uint32_t number, std::uint32_t writes) -> void
{
  std::cout << "Intel HEX to binary." << std::endl;
  std::cout << std::dec << number << " lines parsed." << std::endl;
  std::cout << "Address offset 0x" << std::hex << std::setfill('0') << std::setw(8) << m_addrOffset << "." << std::endl;
  if(m_startLinearFound)
    std::cout << "The main function is at address 0x" << std::hex << std::setfill('0') << std::setw(m_startLinearSize * 2) << m_startLinear << "." << std::endl;
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
auto IntelHex::extractDataFromString(std::string_view input, Line& line, std::size_t& offset) -> void
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
auto IntelHex::applyPadding(std::uint32_t length) -> void
{
  for(std::uint32_t i = 0; i < length; i++)
    m_files->output() << m_padding;
}

/* H2B Process -----------------------------------------------------*/
/**
 * @brief Write data (if present) to output file.
 * 
 * @param[in] line The decoded line.
 * @param[in] number The line number.
 * @param[out] writes Number of written data.
 * @retval false on error.
 */
auto IntelHex::processData(const Line& line, std::uint32_t number, std::uint32_t& writes) -> bool
{
  auto length = line.length;
  std::uint32_t offset = 0U;
  if(m_currentAddress < m_addrOffset)
  {
    auto total = m_currentAddress + line.length;
    if(total > m_addrOffset)
    {
      /* we are in the current packet. */
      offset = total - m_addrOffset;
      length -= offset;
    }
    else
    {
#if 1
      std::cerr << "Off-range address on line " << std::dec << number;
      std::cerr << ", address: 0x" << std::hex << m_currentAddress;
      std::cerr << ", offset: 0x" << std::hex << m_addrOffset << std::endl;
#endif
      /* next packet*/
      return true;
    }
  }
  else if(!m_addrOffset)
    m_addrOffset = m_currentAddress;
  if(!m_files->output().write(reinterpret_cast<const char*>(line.data.data() + offset), length))
  {
    std::cerr << "Unable to write line " << std::dec << number << " to output file." << std::endl;
    return false;
  }
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
auto IntelHex::processAddressOrSegment(const Line& line, std::uint32_t& prev, bool isSegment, std::uint8_t shiftH, std::uint8_t shiftL) -> std::uint32_t
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
auto IntelHex::processStartLinear(const Line& line, std::uint32_t number) -> void
{
  m_startLinearSize = static_cast<std::uint8_t>(line.data.size());
  if(1U == line.data.size())
    m_startLinear = line.data.at(0);
  else if(2U == line.data.size())
    m_startLinear = (line.data.at(0) << 8U) | line.data.at(1U);
  else if(4U == line.data.size())
    m_startLinear = (line.data.at(0) << 24U) | (line.data.at(1) << 16U) | (line.data.at(2) << 8U) | line.data.at(3);
  else
  {
    std::cerr << "Line " << number << " contains a StartLinear field whose data is not supported." << std::endl;
    return;
  }
  m_startLinearFound = true;
}

/**
 * @brief Processes the end of file.
 * 
 * @param[in] summary Print summary.
 * @param[in] number Number of lines.
 * @param[in] length Number of written data.
 */
auto IntelHex::processEndOfFile(bool summary, std::uint32_t number, std::uint32_t length) -> void
{
  m_files->flush();
  if(summary)
    printSummaryH2B(number, length);
}

/* B2H -------------------------------------------------------------*/
/**
 * @brief Displays a summary of the entry.
 * 
 * @param[in] length Number of written data.
 */
auto IntelHex::printSummaryB2H(std::uint32_t length) const -> void
{
  std::cout << "Binary to Intel HEX." << std::endl;
  std::cout << "Address offset 0x" << std::hex << std::setfill('0') << std::setw(8) << m_addrOffset << "." << std::endl;
  if(m_startLinearFound)
    std::cout << "The main function is at address 0x" << std::hex << std::setfill('0') << std::setw(m_startLinearSize * 2) << m_startLinear << "." << std::endl;
  if(m_paddingWidth)
    std::cout << std::dec << m_paddings.size() << " padding found." << std::endl;
  std::cout << std::dec << length << " bytes written." << std::endl;
}

/**
 * @brief Fetches the various paddings found in the file.
 */
auto IntelHex::fetchPadding() -> void
{
  std::array<char, BUFFER_SIZE> arr{};
  auto length = m_files->sizeIn();
  std::uint32_t pstart = 0U;
  std::uint32_t plen = 0U;
  while(length > 0)
  {
    auto r = m_files->read(&arr[0], BUFFER_SIZE);
    if(!r)
      break;
    auto reads = static_cast<std::uint32_t>(r);
    length -= reads;
    for(std::uint32_t i = 0; i < r; i++)
    {
      if(arr[i] == static_cast<char>(m_padding))
      {
        if(!pstart)
          pstart = i;
        plen++;
      }
      else
      {
        if(plen >= m_paddingWidth)
          m_paddings[pstart] = plen;
        pstart = 0U;
        plen = 0U;
      }
    }
  }
  m_files->rewindIn();
}

/* B2H Writes ------------------------------------------------------*/
/**
 * @brief Writes the address.
 * 
 * @param[in] address The address.
 */
auto IntelHex::writeAddress(std::uint32_t address) -> void
{
  Line line{};
  line.address = 0U;
  line.type = RecordType::ExtendedLinear;
  auto addr = address;
  line.data.emplace_back((addr >> 24) & 0xFFU);
  line.data.emplace_back((addr >> 16) & 0xFFU);
  line.length = static_cast<std::uint8_t>(line.data.size());
  line.checksum = evalCRC(line);
  m_files->output() << convertLine(line);
}

/**
 * @brief Writes the segment address.
 * 
 * @param[in] address The address.
 */
auto IntelHex::writeSegment(std::uint32_t address) -> void
{
  Line line{};
  line.address = 0U;
  line.type = RecordType::ExtendedSegment;
  auto addr = (address && address < 0xFFFFU) ? address : (address >> 4);
  line.data.emplace_back((addr >> 8) & 0xFFU);
  line.data.emplace_back((addr >> 16) & 0xFFU);
  line.length = static_cast<std::uint8_t>(line.data.size());
  line.checksum = evalCRC(line);
  m_files->output() << convertLine(line);
}

/**
 * @brief Write data (if present) to output file.
 * 
 * @param[out] writes Number of written data.
 * @retval false on error.
 */
auto IntelHex::writeDataWithoutPadding(std::uint32_t& writes) -> bool
{
  writes = 0U;
  std::array<char, BUFFER_SIZE> arr{};
  auto length = m_files->sizeIn();
  m_currentAddress = 0U;
  m_fullAddress = m_addrOffset;
  while(length > 0)
  {
    auto r = m_files->read(&arr[0], BUFFER_SIZE);
    if(!r)
      return false;
    writeDataSegments(&arr[0], writes, static_cast<std::uint32_t>(r), length);
  }
  return true;
}

/**
 * @brief Write data (if present) to output file.
 * 
 * @param[out] writes Number of written data.
 * @retval false on error.
 */
auto IntelHex::writeDataWithPadding(std::uint32_t& writes) -> bool
{
  writes = 0U;
  std::array<char, BUFFER_SIZE> arr{};
  auto length = m_files->sizeIn();
  m_currentAddress = 0U;
  m_fullAddress = m_addrOffset;

  std::vector<std::uint32_t> keys;
  
  for(const auto& [key, value] : m_paddings)
    keys.push_back(key);
  std::ranges::sort(keys);
  std::uint32_t next = 0;
  while(length > 0)
  {
    if(next >= keys.size())
    {
      auto r = m_files->read(&arr[0], BUFFER_SIZE);
      if(!r)
        return false;
      writeDataSegments(&arr[0], writes, static_cast<std::uint32_t>(r), length);
    }
    else
    {
      auto start = keys[next];
      if(writes < start)
      {
        auto r = m_files->read(&arr[0], start);
        if(!r)
          return false;
        writeDataSegments(&arr[0], writes, static_cast<std::uint32_t>(r), length);
      }
      else
      {
        next++;
        auto plen = m_paddings[start];
        m_files->advanceIn(plen);
        length -= plen;
        m_currentAddress += plen;
        m_fullAddress += plen;
        if(m_useSegment)
          writeSegment(m_fullAddress);
        else
          writeAddress(m_fullAddress);
        m_currentAddress = 0U;
      }
    }
  }
  return true;
}

/**
 * @brief Write data segments to output file.
 * 
 * @param[in] data The data to be written.
 * @param[out] writes Number of written data.
 * @param[in] reads The segment length.
 * @param[out] length The total length.
 */
auto IntelHex::writeDataSegments(const char* data, std::uint32_t& writes, std::uint32_t reads, std::streamsize& length) -> void
{
  auto blocks = reads / m_width;
  auto rem = reads % m_width;
  length -= reads;
  writes += reads;
  std::uint32_t offset = 0;
  for(std::uint32_t i = 0; i < blocks; i++)
  {
    rewriteExtended();
    writeDataSegment(data, m_width, offset);
    offset += m_width;
  }
  if(rem)
    writeDataSegment(data, rem, offset);
}

/**
 * @brief Write data segment to output file.
 * 
 * @param[in] data The data to be written.
 * @param[in] segmentLength The segment length.
 * @param[in] offset The data offset.
 */
auto IntelHex::writeDataSegment(const char* data, std::uint32_t segmentLength, std::uint32_t offset) -> void
{
  Line line{};
  std::copy(&data[offset], &data[offset + segmentLength], std::back_inserter(line.data));
  line.address = static_cast<std::uint16_t>(m_currentAddress);
  m_currentAddress += segmentLength;
  line.type = RecordType::Data;
  line.length = static_cast<std::uint8_t>(line.data.size());
  line.checksum = evalCRC(line);
  m_files->output() << convertLine(line);
}

/**
 * @brief Rewrites the extended part (linear or segment).
 */
auto IntelHex::rewriteExtended() -> void
{
  if(ADDR_LIMIT == m_currentAddress)
  {
    m_fullAddress += ADDR_LIMIT;
    if(m_useSegment)
      writeSegment(m_fullAddress);
    else
      writeAddress(m_fullAddress);
    m_currentAddress = 0U;
  }
}

/**
 * @brief Writes the starts linear address.
 */
auto IntelHex::writeStartLinear() -> void
{
  if(m_startLinearFound)
  {
    Line line{};
    line.address = 0U;
    line.type = RecordType::StartLinear;
    if(m_startLinear > 0xFFFFU)
    {
      line.data.emplace_back(((m_startLinear & 0xFF000000U) >> 24));
      line.data.emplace_back(((m_startLinear & 0x00FF0000U) >> 16));
      line.data.emplace_back(((m_startLinear & 0x0000FF00U) >> 8));
      line.data.emplace_back((m_startLinear & 0x000000FFU));
    }
    else if(m_startLinear > 0xFFU)
    {
      line.data.emplace_back(((m_startLinear & 0x0000FF00U) >> 8));
      line.data.emplace_back((m_startLinear & 0x000000FFU));
    }
    else
      line.data.emplace_back((m_startLinear & 0x000000FFU));
    line.length = static_cast<std::uint8_t>(line.data.size());
    line.checksum = evalCRC(line);
    m_files->output() << convertLine(line);
  }
}

/**
 * @brief Writes the end of file.
 */
auto IntelHex::writeEndOfFile() -> void
{
  Line line{};
  line.type = RecordType::EndOfFile;
  line.checksum = evalCRC(line);
  m_files->output() << convertLine(line);
}