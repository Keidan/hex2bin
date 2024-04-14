/**
 * @file Hex2Bin.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "Hex2Bin.hpp"
#include "Helper.hpp"
#include <iostream>
#include <cctype>

/* Usings -------------------------------------------------------------------*/
using namespace h2b;

/* Constants ----------------------------------------------------------------*/
static constexpr auto* HEX = "0123456789abcdef\0";

/* Public functions ---------------------------------------------------------*/

Hex2Bin::Hex2Bin(Files* files)
  : m_files(files)
{
}

/**
 * @brief Sets the starting value.
 * 
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto Hex2Bin::start(std::string_view value, std::string& what) -> bool
{
  return Helper::setValueFromstring(m_start, value, what);
}

/**
 * @brief Tests whether the start value is valid or not.
 * 
 * @retval True if it is valid, otherwise false.
 */
auto Hex2Bin::isStart() const -> bool
{
  return 0U != m_start;
}

/**
 * @brief Returns the start value.
 * 
 * @retval std::uint32_t
 */
auto Hex2Bin::start() const -> std::uint32_t
{
  return m_start;
}

/**
 * @brief Sets the limit value.
 * 
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto Hex2Bin::limit(std::string_view value, std::string& what) -> bool
{
  return Helper::setValueFromstring(m_limit, value, what);
}

/**
 * @brief Tests whether the limit value is valid or not.
 * 
 * @retval True if it is valid, otherwise false.
 */
auto Hex2Bin::isLimit() const -> bool
{
  return 0U != m_limit;
}

/**
 * @brief Returns the limit value.
 * 
 * @retval std::uint32_t
 */
auto Hex2Bin::limit() const -> std::uint32_t
{
  return m_limit;
}

/**
 * @brief Only extracts words from "start" to "limit".
 */
auto Hex2Bin::extractOnly() -> void
{
  std::string line;

  while(m_files->getline(line))
  {
    auto fragment = Helper::getFragment(line, m_start, m_limit);
    if(!fragment.empty() && fragment.at(fragment.size() - 1U) != '\n')
      fragment += "\n";
    m_files->output() << fragment;
  }
  m_files->output().flush();
}

/**
 * @brief Extracts and without converting all printable characters.
 * 
 * @retval False on error.
 */
auto Hex2Bin::extractNoPrint() -> bool
{
  std::string line;
  auto error = false;
  while(m_files->getline(line))
  {
    if(line.empty())
    {
      std::cerr << "Empty line ignored" << std::endl;
      continue;
    }
    auto fragment = Helper::getFragment(line, m_start, m_limit);

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
  m_files->flush();
  return !error;
}

/**
 * @brief Extracts and converts all printable characters.
 * 
 * @retval Returns false on error, true else.
 */
auto Hex2Bin::extractPrint() -> bool
{
  char c;
  auto offset = 0L;
  /* get the file size */
  auto length = m_files->sizeIn();

  /* temp buffer */
  std::vector<uint8_t> buf{};
  buf.reserve(static_cast<std::size_t>(length));
  auto i = 0L;
  while(offset < length)
  {
    auto& in = m_files->input();
    in.read(&c, 1U);
    if(!in)
      break;
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
    m_files->output() << static_cast<char>(std::stol(cc.c_str(), nullptr, 16U));
  }
  m_files->flush();
  return true;
}

/**
 * @brief Gets the files pointer.
 * 
 * @retval Files*
 */
auto Hex2Bin::files() const -> Files*
{
  return m_files;
}

/* Private functions --------------------------------------------------------*/
/**
 * @brief Validates the line and displays an error message if the validation fails.
 * 
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
 * @brief Extracts and without converting all printable characters (sub loop 1).
 * 
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
    m_files->output() << static_cast<char>(std::stol(token, nullptr, 16U));
  }
}

/**
 * @brief Extracts and without converting all printable characters (sub loop 2).
 * 
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
    m_files->output() << static_cast<char>(std::stol(s1 + s2, nullptr, 16U));
  }
}
