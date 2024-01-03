/*
 * @file Hex2Bin.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "Hex2Bin.hpp"
#include "Helper.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <cctype>

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
auto Hex2Bin::openInput(std::string_view path) -> Hex2BinOpenResult
{
  return openFile<std::ifstream>(m_input, path, std::ios::in | std::ios::binary);
}

/**
 * @brief Opens the output file.
 * @param[in] path The file path.
 * @retval False if error, otherwise true.
 * @retval Hex2BinOpenResult.
 */
auto Hex2Bin::openOutput(std::string_view path) -> Hex2BinOpenResult
{
  return openFile<std::ofstream>(m_output, path, std::ios::out | std::ios::binary);
}

/**
 * @brief Sets the starting value.
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto Hex2Bin::setStart(std::string_view value, std::string& what) -> bool
{
  return Helper::setValueFromstring(m_start, value, what);
}

/**
 * @brief Tests whether the start value is valid or not.
 * @retval True if it is valid, otherwise false.
 */
auto Hex2Bin::isValidStart() const -> bool
{
  return 0U != m_start;
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
auto Hex2Bin::setLimit(std::string_view value, std::string& what) -> bool
{
  return Helper::setValueFromstring(m_limit, value, what);
}

/**
 * @brief Tests whether the limit value is valid or not.
 * @retval True if it is valid, otherwise false.
 */
auto Hex2Bin::isValidLimit() const -> bool
{
  return 0U != m_limit;
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
  using enum Hex2BinIsOpen;
  if(!m_input.is_open() || !m_output.is_open())
  {
    if(!m_input.is_open() && !m_output.is_open())
    {
      return Both;
    }
    if(!m_input.is_open())
    {
      return Input;
    }
    return Output;
  }
  return Success;
}

/**
 * @brief Only extracts words from "start" to "limit".
 */
auto Hex2Bin::extractOnly() -> void
{
  std::string line;

  while(std::getline(m_input, line))
  {
    auto fragment = Helper::getFragment(line, m_start, m_limit);
    if(!fragment.empty() && fragment.at(fragment.size() - 1U) != '\n')
      fragment += "\n";
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
    auto fragment = Helper::getFragment(line, m_start, m_limit);
    ;

    const auto idxSpace = fragment.find(' ');
    if(std::string::npos != idxSpace)
    {
      extractNoPrintSpaceFound(fragment, error);
    }
    else
    {
      if(fragment.length() % 2U)
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
  m_input.clear(); //  Since ignore will have set eof.
  m_input.seekg(0U, std::ios_base::beg);

  /* temp buffer */
  std::vector<uint8_t> buf{};
  buf.reserve(static_cast<std::size_t>(length));
  auto i = 0L;
  while(offset < length)
  {
    m_input.read(&c, 1U);
    if(!m_input)
    {
      break;
    }
    offset++;
    if(std::isalnum(c))
    {
      buf.emplace_back(static_cast<uint8_t>(c));
      i++;
    }
  }
  length = i;
  /* write the data */
  for(i = 0U; i < length; i += 2U)
  {
    std::string cc{static_cast<char>(buf[i]), static_cast<char>(buf[i + 1U]), 0};
    m_output << static_cast<char>(std::stol(cc.c_str(), nullptr, 16U));
  }
  m_output.flush();
  return true;
}

/* Private functions --------------------------------------------------------*/
/**
 * @brief Validates the line and displays an error message if the validation fails.
 * @param[in] line The reference line.
 * @param[in] s The string to validate.
 * @retval bool
 */
auto Hex2Bin::validateHexAndLogOnError(std::string_view line, std::string_view s) const -> bool
{
  if(1 == s.length())
  {
    if(!Helper::search(HEX, s, true))
    {
      std::cerr << "0 Character '" << s << "' is not compatible with hexadecimal conversion." << std::endl;
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
      if(!Helper::search(HEX, temp, true))
      {
        std::cerr << "1 Character '" << temp << "' is not compatible with hexadecimal conversion." << std::endl;
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
template <class Stream>
auto Hex2Bin::openFile(Stream& stream, std::string_view path, std::ios_base::openmode mode) const -> Hex2BinOpenResult
{
  using enum Hex2BinOpenResult;
  if(path.empty())
  {
    return Error;
  }
  if(!stream.is_open())
  {
    auto p = std::string(path);
    stream.open(p, mode);
    if(!stream.is_open() || stream.fail())
    {
      return Error;
    }
    return Success;
  }
  else
  {
    return Already;
  }
}

/**
 * @brief Extracts and without converting all printable characters (sub loop 1).
 * @param[in] fragment A fragment of the input line.
 * @param[out] error Error?
 */
auto Hex2Bin::extractNoPrintSpaceFound(std::string_view fragment, bool& error) -> void
{
  auto tokens = Helper::split(fragment, "\\s+");
  for(const auto& token : tokens)
  {
    if(!validateHexAndLogOnError(fragment, token))
    {
      error = true;
      continue;
    }
    m_output << static_cast<char>(std::stol(token, nullptr, 16U));
  }
}

/**
 * @brief Extracts and without converting all printable characters (sub loop 2).
 * @param[in] fragment A fragment of the input line.
 * @param[out] error Error?
 */
auto Hex2Bin::extractNoPrintNoSpaceFound(std::string_view fragment, bool& error) -> void
{
  for(auto i = 0U; i < fragment.length(); i += 2U)
  {
    auto s1 = std::string(1, fragment[i]);
    auto s2 = std::string(1, fragment[i + 1U]);
    if(!validateHexAndLogOnError(fragment, s1) || !validateHexAndLogOnError(fragment, s2))
    {
      error = true;
      break;
    }
    m_output << static_cast<char>(std::stol(s1 + s2, nullptr, 16U));
  }
}
