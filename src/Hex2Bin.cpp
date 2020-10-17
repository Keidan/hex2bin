/*
 * @file Hex2Bin.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "Hex2Bin.hpp"
#include <iostream>
#include <fstream>
#include <exception>
#include <regex>
#include <vector>
#include <limits>
#include <cctype>
#include <cstdio>
#include <cstring>

/* Usings -------------------------------------------------------------------*/
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using std::uint32_t;
using std::regex;
using std::sregex_token_iterator;
using std::exception;

/* Public functions ---------------------------------------------------------*/

Hex2Bin::Hex2Bin() : m_start(0), m_limit(0), m_output(), m_input()
{
}

Hex2Bin::~Hex2Bin()
{
  if(m_input.is_open())
  {
    m_input.close();
  }
  if(m_output.is_open())
  {
    m_output.close();
  }
}

/**
 * @brief Opens the input file.
 * @param[in] path The file path.
 * @retval False if error, otherwise true.
 * @retval Hex2BinOpenResult.
 */
auto Hex2Bin::openInput(const string& path) -> Hex2BinOpenResult
{
  if(path.empty())
  {
    return Hex2BinOpenResult::ERROR;
  }
  if(!m_input.is_open())
  {
    m_input.open(path, std::ios::in|std::ios::binary);
    if(!m_input.is_open() || m_input.fail())
    {
      return Hex2BinOpenResult::ERROR;
    }
    return Hex2BinOpenResult::SUCCESS;
  }
  else
  {
    return Hex2BinOpenResult::ALREADY;
  }
}
    
/**
 * @brief Opens the input file.
 * @param[in] path The file path.
 * @retval False if error, otherwise true.
 * @retval Hex2BinOpenResult.
 */
auto Hex2Bin::openOutput(const string& path) -> Hex2BinOpenResult
{
  if(path.empty())
  {
    return Hex2BinOpenResult::ERROR;
  }
  if(!m_output.is_open())
  {
    m_output.open(path, std::ios::out|std::ios::binary);
    if(!m_output.is_open() || m_output.fail())
    {
      return Hex2BinOpenResult::ERROR;
    }
    return Hex2BinOpenResult::SUCCESS;
  }
  else
  {
    return Hex2BinOpenResult::ALREADY;
  }
}

/**
 * @brief Sets the starting value.
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto Hex2Bin::setStart(const string& value, string &what) -> bool
{
  try
  {
    m_start = std::stoi(value);
    if(m_start < 0)
    {
      m_start = 0;
    }
  }
  catch (const exception& e)
  {
    what = e.what();
    return false;
  }
  return true;
}

/**
 * @brief Tests whether the start value is valid or not.
 * @retval True if it is valid, otherwise false.
 */
auto Hex2Bin::isValidStart() -> bool
{
  return m_start != 0;
}

/**
 * @brief Returns the start value.
 * @retval std::uint32_t
 */
auto Hex2Bin::getStart() -> std::uint32_t
{
  return m_start;
}

/**
 * @brief Sets the limit value.
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto Hex2Bin::setLimit(const string& value, string &what) -> bool
{
  try
  {
    m_limit = std::stoi(value);
    if(m_limit< 0)
    {
      m_limit = 0;
    }
  }
  catch (const exception& e)
  {
    what = e.what();
    return false;
  }
  return true;
}

/**
 * @brief Tests whether the limit value is valid or not.
 * @retval True if it is valid, otherwise false.
 */
auto Hex2Bin::isValidLimit() -> bool
{
  return m_limit != 0;
}

/**
 * @brief Returns the limit value.
 * @retval std::uint32_t
 */
auto Hex2Bin::getLimit() -> std::uint32_t
{
  return m_limit;
}

/**
 * @brief Test if the files are open.
 * @retval Hex2BinIsOpen.
 */
auto Hex2Bin::isFilesOpen() -> Hex2BinIsOpen
{
    if(!m_input.is_open() || !m_output.is_open())
    {
      if(!m_input.is_open() && !m_output.is_open())
      {
	return Hex2BinIsOpen::BOTH;
      }
      else if(!m_input.is_open())
      {
	return Hex2BinIsOpen::INPUT;
      }
      else
      {
	return Hex2BinIsOpen::OUTPUT;
      }
    }
    return Hex2BinIsOpen::SUCCESS;
}

/**
 * @brief Only extracts words from "start" to "limit".
 */
auto Hex2Bin::extractOnly() -> void
{
  string line;

  while (std::getline(m_input, line)) {
    auto fragment = getFragment(line);
    if(!fragment.empty() && fragment.at(fragment.size() - 1) != '\n') fragment += "\n";
    m_output << fragment;
  }
  m_output.flush();
}

/**
 * @brief Extracts and without converting all printable characters.
 * @param[in] output The output file.
 * @param[in] input The input file.
 * @param[in] start The start offset.
 * @param[in] limit The limit per lines.
 */
auto Hex2Bin::extractNoPrint() -> void
{
  string line;
  
  while (std::getline(m_input, line)) {
    auto fragment = getFragment(line);
    auto tokens = split(fragment, "\\s+");
    for(vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
      m_output << (char)std::stol((*it), nullptr, 16);
    }
  }
  m_output.flush();
}

/**
 * @brief Extracts and converts all printable characters.
 * @retval Returns false on error, true else.
 */
auto Hex2Bin::extractPrint() -> bool
{
  char cc[2];
  uint8_t *buf, c;
  auto offset = 0L;
  /* get the file size */

  m_input.ignore(std::numeric_limits<std::streamsize>::max());
  auto length = m_input.gcount();
  m_input.clear();   //  Since ignore will have set eof.
  m_input.seekg(0, std::ios_base::beg);

  /* temp buffer */
  buf = new uint8_t[length];
  if(buf == nullptr)
  {
    return false;
  }
  auto i = 0L;
  while(offset < length)
  {
    m_input.read((char*)&c, 1);
    if(!m_input)
    {
      break;
    }
    offset++;
    if(std::isalnum(c))
    {
      buf[i++] = c;
    }
  }
  length = i;
  /* write the datas */
  for(i = 0; i < length; i+=2)
  {
    std::memcpy(cc, buf + i, 2);
    m_output << (char)std::stol(cc, nullptr, 16);
  }
  m_output.flush();
  delete [] buf;
  return true;
}

/* Private functions --------------------------------------------------------*/
/**
 * @brief Splits a string according to the specified regex
 * @param[in] in The input string.
 * @param[in] reg The regex.
 * @retval The result in a vector.
 */
auto Hex2Bin::split(const string& in, const string &reg) -> vector<string>
{
  // passing -1 as the submatch index parameter performs splitting
  regex re(reg);
  sregex_token_iterator first{in.begin(), in.end(), re, -1}, last;
  return {first, last};
  
}
    
/**
 * @brief Returns a fragment of the input line.
 * @param[in] line The input line.
 * @retval The fragment of the input line.
 */
auto Hex2Bin::getFragment(const string &line) -> string
{
  auto len = 0UL, lim = 0UL;
  len = line.size();
  if(len < m_start)
    len = 0UL;
  lim = (m_limit > len) ? len : m_limit;
  return line.substr(m_start, lim);
}
