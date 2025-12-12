/**
 * @file Bin2Intel.cpp
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
#include <h2b/ihex/Bin2Intel.hpp>
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

/* Public functions ---------------------------------------------------------*/

Bin2Intel::Bin2Intel(const std::unique_ptr<utils::Files>& files, const Parameters& params)
  : m_files(files)
  , m_params(params)
{
}

/**
 * @brief Displays a summary of the entry.
 * 
 * @param[in] length Number of written data.
 */
auto Bin2Intel::printSummary(std::uint32_t length) const -> void
{
  std::cout << "Binary to Intel HEX." << std::endl;
  std::cout << "Address offset 0x" << Helper::int2hex(m_params.addressOffset(), '0', 8) << "." << std::endl;
  if(m_params.paddingWidth())
    std::cout << std::dec << m_paddings.size() << " padding found." << std::endl;
  std::cout << std::dec << length << " bytes written." << std::endl;
}

/**
 * @brief Fetches the various paddings found in the file.
 */
auto Bin2Intel::fetchPadding() -> void
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
      extactPaddingForFetch(arr[i], i, pstart, plen);
  }
  m_files->rewindIn();
}

/**
 * @brief Extracts padding info.
 * 
 * @param[in] c The current char.
 * @param[in] i The index.
 * @param[out] pstart Padding start.
 * @param[out] plen Padding length.
 */
auto Bin2Intel::extactPaddingForFetch(std::uint8_t c, std::uint32_t i, std::uint32_t& pstart, std::uint32_t& plen) -> void
{
  if(c == m_params.padding())
  {
    if(!pstart)
      pstart = i;
    plen++;
  }
  else
  {
    if(plen >= m_params.paddingWidth())
      m_paddings[pstart] = plen;
    pstart = 0U;
    plen = 0U;
  }
}

/**
 * @brief Writes the address.
 * 
 * @param[in] address The address.
 */
auto Bin2Intel::writeAddress(std::uint32_t address) -> void
{
  Line line{};
  line.address = 0U;
  line.type = RecordType::ExtendedLinear;
  auto addr = address;
  line.data.emplace_back((addr >> 24) & 0xFFU);
  line.data.emplace_back((addr >> 16) & 0xFFU);
  line.length = static_cast<std::uint8_t>(line.data.size());
  line.checksum = Tools::evalCRC(line);
  m_files->output() << Tools::convertLine(line);
}

/**
 * @brief Writes the segment address.
 * 
 * @param[in] address The address.
 */
auto Bin2Intel::writeSegment(std::uint32_t address) -> void
{
  Line line{};
  line.address = 0U;
  line.type = RecordType::ExtendedSegment;
  auto addr = (address && address < 0xFFFFU) ? address : (address >> 4);
  line.data.emplace_back((addr >> 8) & 0xFFU);
  line.data.emplace_back((addr >> 16) & 0xFFU);
  line.length = static_cast<std::uint8_t>(line.data.size());
  line.checksum = Tools::evalCRC(line);
  m_files->output() << Tools::convertLine(line);
}

/**
 * @brief Write data (if present) to output file.
 * 
 * @param[out] writes Number of written data.
 * @retval false on error.
 */
auto Bin2Intel::writeDataWithoutPadding(std::uint32_t& writes) -> bool
{
  writes = 0U;
  std::array<char, BUFFER_SIZE> arr{};
  auto length = m_files->sizeIn();
  m_currentAddress = 0U;
  m_fullAddress = m_params.addressOffset();
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
auto Bin2Intel::writeDataWithPadding(std::uint32_t& writes) -> bool
{
  writes = 0U;
  std::array<char, BUFFER_SIZE> arr{};
  auto length = m_files->sizeIn();
  m_currentAddress = 0U;
  m_fullAddress = m_params.addressOffset();

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
      if(!writeDataWithPaddingPartial(writes, length, next, &arr[0], keys[next]))
        return false;
    }
  }
  return true;
}

auto Bin2Intel::writeDataWithPaddingPartial(std::uint32_t& writes, std::streamsize& length, std::uint32_t& next, char* data, std::uint32_t start) -> bool
{
  if(writes < start)
  {
    auto r = m_files->read(data, start);
    if(!r)
      return false;
    writeDataSegments(data, writes, static_cast<std::uint32_t>(r), length);
  }
  else
  {
    next++;
    auto plen = m_paddings[start];
    m_files->advanceIn(plen);
    length -= plen;
    m_currentAddress += plen;
    m_fullAddress += plen;
    if(m_params.useSegment())
      writeSegment(m_fullAddress);
    else
      writeAddress(m_fullAddress);
    m_currentAddress = 0U;
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
auto Bin2Intel::writeDataSegments(const char* data, std::uint32_t& writes, std::uint32_t reads, std::streamsize& length) -> void
{
  auto blocks = reads / m_params.width();
  auto rem = reads % m_params.width();
  length -= reads;
  writes += reads;
  std::uint32_t offset = 0;
  for(std::uint32_t i = 0; i < blocks; i++)
  {
    rewriteExtended();
    writeDataSegment(data, m_params.width(), offset);
    offset += m_params.width();
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
auto Bin2Intel::writeDataSegment(const char* data, std::uint32_t segmentLength, std::uint32_t offset) -> void
{
  Line line{};
  std::copy(&data[offset], &data[offset + segmentLength], std::back_inserter(line.data));
  line.address = static_cast<std::uint16_t>(m_currentAddress);
  m_currentAddress += segmentLength;
  line.type = RecordType::Data;
  line.length = static_cast<std::uint8_t>(line.data.size());
  line.checksum = Tools::evalCRC(line);
  m_files->output() << Tools::convertLine(line);
}

/**
 * @brief Rewrites the extended part (linear or segment).
 */
auto Bin2Intel::rewriteExtended() -> void
{
  if(ADDR_LIMIT == m_currentAddress)
  {
    m_fullAddress += ADDR_LIMIT;
    if(m_params.useSegment())
      writeSegment(m_fullAddress);
    else
      writeAddress(m_fullAddress);
    m_currentAddress = 0U;
  }
}

/**
 * @brief Writes the starts linear address.
 */
auto Bin2Intel::writeStartLinear() -> void
{
  if(m_params.startLinearFound())
  {
    Line line{};
    line.address = 0U;
    line.type = RecordType::StartLinear;
    if(m_params.linear() > 0xFFFFU)
    {
      line.data.emplace_back((m_params.linear() & 0xFF000000U) >> 24);
      line.data.emplace_back((m_params.linear() & 0x00FF0000U) >> 16);
      line.data.emplace_back((m_params.linear() & 0x0000FF00U) >> 8);
      line.data.emplace_back(m_params.linear() & 0x000000FFU);
    }
    else if(m_params.linear() > 0xFFU)
    {
      line.data.emplace_back((m_params.linear() & 0x0000FF00U) >> 8);
      line.data.emplace_back(m_params.linear() & 0x000000FFU);
    }
    else
      line.data.emplace_back(m_params.linear() & 0x000000FFU);
    line.length = static_cast<std::uint8_t>(line.data.size());
    line.checksum = Tools::evalCRC(line);
    m_files->output() << Tools::convertLine(line);
  }
}

/**
 * @brief Writes the end of file.
 */
auto Bin2Intel::writeEndOfFile() -> void
{
  Line line{};
  line.type = RecordType::EndOfFile;
  line.checksum = Tools::evalCRC(line);
  m_files->output() << Tools::convertLine(line);
}

/**
 * @brief Returns the paddings.
 * 
 * @retval const std::map<std::uint32_t, std::uint32_t>&
 */
auto Bin2Intel::paddings() const -> const std::map<std::uint32_t, std::uint32_t>&
{
  return m_paddings;
}