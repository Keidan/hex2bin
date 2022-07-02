/*
 * @file Hex2Bin.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "Hex2Bin.hpp"
#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <limits>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <exception>

/* Usings -------------------------------------------------------------------*/
using namespace h2b;

/* Constants ----------------------------------------------------------------*/
static constexpr auto* HEX = "0123456789abcdef\0";

/* Public functions ---------------------------------------------------------*/

/**
 * @brief Closes the files.
 */
auto Hex2Bin::close() -> void
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
auto Hex2Bin::openInput(const std::string& path) -> Hex2BinOpenResult
{
  return openFile<std::ifstream>(m_input, path, std::ios::in | std::ios::binary);
}

/**
 * @brief Opens the output file.
 * @param[in] path The file path.
 * @retval False if error, otherwise true.
 * @retval Hex2BinOpenResult.
 */
auto Hex2Bin::openOutput(const std::string& path) -> Hex2BinOpenResult
{
  return openFile<std::ofstream>(m_output, path, std::ios::out | std::ios::binary);
}

/**
 * @brief Sets the starting value.
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto Hex2Bin::setStart(const std::string& value, std::string& what) -> bool
{
  return setValueFromstring(m_start, value, what);
}

/**
 * @brief Tests whether the start value is valid or not.
 * @retval True if it is valid, otherwise false.
 */
auto Hex2Bin::isValidStart() const -> bool
{
  return m_start != 0;
}

/**
 * @brief Returns the start value.
 * @retval std::uint32_t
 */
auto Hex2Bin::getStart() const -> std::uint32_t
{
  return m_start;
}

/**
 * @brief Sets the limit value.
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto Hex2Bin::setLimit(const std::string& value, std::string& what) -> bool
{
  return setValueFromstring(m_limit, value, what);
}

/**
 * @brief Tests whether the limit value is valid or not.
 * @retval True if it is valid, otherwise false.
 */
auto Hex2Bin::isValidLimit() const -> bool
{
  return m_limit != 0;
}

/**
 * @brief Returns the limit value.
 * @retval std::uint32_t
 */
auto Hex2Bin::getLimit() const -> std::uint32_t
{
  return m_limit;
}

/**
 * @brief Test if the files are open.
 * @retval Hex2BinIsOpen.
 */
auto Hex2Bin::isFilesOpen() const -> Hex2BinIsOpen
{
  if(!m_input.is_open() || !m_output.is_open())
  {
    if(!m_input.is_open() && !m_output.is_open())
    {
      return Hex2BinIsOpen::Both;
    }
    if(!m_input.is_open())
    {
      return Hex2BinIsOpen::Input;
    }
    return Hex2BinIsOpen::Output;
  }
  return Hex2BinIsOpen::Success;
}

/**
 * @brief Only extracts words from "start" to "limit".
 */
auto Hex2Bin::extractOnly() -> void
{
  std::string line;

  while(std::getline(m_input, line))
  {
    auto fragment = getFragment(line);
    if(!fragment.empty() && fragment.at(fragment.size() - 1) != '\n') fragment += "\n";
    m_output << fragment;
  }
  m_output.flush();
}

/**
 * @brief Extracts and without converting all printable characters.
 * @retval False on error.
 */
auto Hex2Bin::extractNoPrint() -> bool
{
  std::string line;
  auto error = false;
  while(std::getline(m_input, line))
  {
    if(line.empty())
    {
      std::cerr << "Empty line ignored" << std::endl;
      continue;
    }
    auto fragment = getFragment(line);

    const auto idxSpace = fragment.find(' ');
    if(idxSpace != std::string::npos)
    {
      extractNoPrintSpaceFound(fragment, error);
    }
    else
    {
      if(fragment.length() % 2)
      {
        std::cerr << "The following line must have an even number of characters:" << std::endl;
        std::cerr << "Line: '" << fragment << "'" << std::endl;
        error = true;
        continue;
      }
      extractNoPrintNoSpaceFound(fragment, error);
    }
  }
  m_output.flush();
  return !error;
}

/**
 * @brief Extracts and converts all printable characters.
 * @retval Returns false on error, true else.
 */
auto Hex2Bin::extractPrint() -> bool
{
  char c;
  auto offset = 0L;
  /* get the file size */

  m_input.ignore(std::numeric_limits<std::streamsize>::max());
  auto length = m_input.gcount();
  m_input.clear();   //  Since ignore will have set eof.
  m_input.seekg(0, std::ios_base::beg);

  /* temp buffer */
  std::vector<uint8_t> buf{};
  buf.reserve(length);
  auto i = 0L;
  while(offset < length)
  {
    m_input.read(&c, 1);
    if(!m_input)
    {
      break;
    }
    offset++;
    if(std::isalnum(c))
    {
      buf[i] = static_cast<uint8_t>(c);
      i++;
    }
  }
  length = i;
  /* write the datas */
  for(i = 0; i < length; i += 2)
  {
    std::string cc{
      static_cast<char>(buf[i]),
      static_cast<char>(buf[i + 1]),
      0
    };
    m_output << static_cast<char>(std::stol(cc.c_str(), nullptr, 16));
  }
  m_output.flush();
  return true;
}

/* Private functions --------------------------------------------------------*/
/**
 * @brief Splits a string according to the specified regex
 * @param[in] in The input string.
 * @param[in] reg The regex.
 * @retval The result in a vector.
 */
auto Hex2Bin::split(const std::string& in, const std::string& reg) -> std::vector<std::string>
{
  // passing -1 as the submatch index parameter performs splitting
  std::regex re(reg);
  std::sregex_token_iterator first{in.begin(), in.end(), re, -1};
  std::sregex_token_iterator last;
  return {first, last};
}

/**
 * @brief Returns a fragment of the input line.
 * @param[in] line The input line.
 * @retval The fragment of the input line.
 */
auto Hex2Bin::getFragment(const std::string& line) const -> std::string
{
  auto len = line.size();
  if(len < m_start)
  {
    len = 0UL;
  }
  const auto lim = (m_limit == 0 || m_limit > len) ? len : m_limit;
  return line.substr(std::min(m_start, static_cast<std::uint32_t>(len)), lim);
}

/**
 * @brief Searches for a string in another.
 * @param[in] ref The reference string.
 * @param[in] needle The string to search.
 * @param[in] ignoreCase True for case-insensitive.
 * @retval bool
 */
auto Hex2Bin::search(const std::string& ref, const std::string& needle, bool ignoreCase) const -> bool
{
  const auto it = std::search(ref.begin(), ref.end(), needle.begin(), needle.end(),
                              [ignoreCase](const char c1, const char c2)
  {
    return ignoreCase ? (std::toupper(c1) == std::toupper(c2)) : (c1 == c2);
  });
  return it != ref.end();
}

/**
 * @brief Validates the line and displays an error message if the validation fails.
 * @param[in] line The reference line.
 * @param[in] s The string to validate.
 * @retval bool
 */
auto Hex2Bin::validateHexAndLogOnError(const std::string& line, const std::string& s) const -> bool
{
  if(s.length() == 1)
  {
    if(!search(HEX, s, true))
    {
      std::cerr << "Character '" << s << "' is not compatible with hexadecimal conversion." << std::endl;
      std::cerr << "Cancel line processing:" << std::endl;
      std::cerr << "Line: '" << line << "'" << std::endl;
      return false;
    }
  }
  else
  {
    for(const auto& c : s)
    {
      auto temp = std::string(1, c);
      if(!search(HEX, temp, true))
      {
        std::cerr << "Character '" << temp << "' is not compatible with hexadecimal conversion." << std::endl;
        std::cerr << "Cancel line processing:" << std::endl;
        std::cerr << "Line: '" << line << "'" << std::endl;
        return false;
      }
    }
  }
  return true;
}

/**
 * @brief Opens a file.
 * @param[in,out] stream The file stream.
 * @param[in] path The file path.
 * @param[in] mode The opening mode.
 * @retval False if error, otherwise true.
 * @retval Hex2BinOpenResult.
 */
template<class Stream>
auto Hex2Bin::openFile(Stream& stream, const std::string& path, std::ios_base::openmode mode) const -> Hex2BinOpenResult
{
  if(path.empty())
  {
    return Hex2BinOpenResult::Error;
  }
  if(!stream.is_open())
  {
    stream.open(path, mode);
    if(!stream.is_open() || stream.fail())
    {
      return Hex2BinOpenResult::Error;
    }
    return Hex2BinOpenResult::Success;
  }
  else
  {
    return Hex2BinOpenResult::Already;
  }
}

/**
 * @brief Sets the value from a string.
 * @param[out] output Output value.
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto Hex2Bin::setValueFromstring(std::uint32_t& output, const std::string& value, std::string& what) -> bool
{
  try
  {
    auto val = std::stoi(value);
    if(val < 0)
    {
      val = 0;
    }
    output = val;
  }
  catch(const std::invalid_argument& e)
  {
    what = std::string("invalid_argument: ") + e.what();
    return false;
  }
  catch(const std::out_of_range& e)
  {
    what = std::string("out_of_range: ") + e.what();
    return false;
  }
  return true;
}

/**
 * @brief Extracts and without converting all printable characters (sub loop 1).
 * @param[in] fragment A fragment of the input line.
 * @param[out] error Error?
 */
auto Hex2Bin::extractNoPrintSpaceFound(const std::string& fragment, bool& error) -> void
{
  auto tokens = split(fragment, "\\s+");
  for(const auto& token : tokens)
  {
    if(!validateHexAndLogOnError(fragment, token))
    {
      error = true;
      continue;
    }
    m_output << static_cast<char>(std::stol(token, nullptr, 16));
  }
}

/**
 * @brief Extracts and without converting all printable characters (sub loop 2).
 * @param[in] fragment A fragment of the input line.
 * @param[out] error Error?
 */
auto Hex2Bin::extractNoPrintNoSpaceFound(const std::string& fragment, bool& error) -> void
{
  for(auto i = 0U; i < fragment.length(); i += 2)
  {
    auto s1 = std::string(1, fragment[i]);
    auto s2 = std::string(1, fragment[i + 1]);
    if(!validateHexAndLogOnError(fragment, s1) || !validateHexAndLogOnError(fragment, s2))
    {
      error = true;
      break;
    }
    m_output << static_cast<char>(std::stol(s1 + s2, nullptr, 16));
  }
}

