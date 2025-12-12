/**
 * @file Hex2Bin.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include <h2b/utils/Files.hpp>
#include <h2b/utils/Helper.hpp>
#include <iostream>
#include <fstream>
#include <limits>
#include <cctype>
#include <string>

/* Usings -------------------------------------------------------------------*/
using namespace h2b::utils;
/* Constants ----------------------------------------------------------------*/

/* Public functions ---------------------------------------------------------*/
/**
 * @brief Closes the files.
 */
auto Files::close() -> void
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
 * 
 * @param[in] path The file path.
 * @retval False if error, otherwise true.
 * @retval OpenResult.
 */
auto Files::openInput(std::string_view path) -> OpenResult
{
  return openFile<std::ifstream>(m_input, path, std::ios::in | std::ios::binary);
}

/**
 * @brief Opens the output file.
 * 
 * @param[in] path The file path.
 * @retval False if error, otherwise true.
 * @retval OpenResult.
 */
auto Files::openOutput(std::string_view path) -> OpenResult
{
  return openFile<std::ofstream>(m_output, path, std::ios::out | std::ios::binary);
}

/**
 * @brief Test if the files are open.
 * 
 * @retval OpenStatus.
 */
auto Files::isFilesOpen() const -> OpenStatus
{
  using enum OpenStatus;
  if(!m_input.is_open() || !m_output.is_open())
  {
    if(!m_input.is_open() && !m_output.is_open())
    {
      return AllClosed;
    }
    if(!m_input.is_open())
    {
      return InputClosed;
    }
    return OutputClosed;
  }
  return AllOpened;
}

/**
 * @brief Returns the input stream.
 * 
 * @retval std::ifstream&
 */
auto Files::input() -> std::ifstream&
{
  return m_input;
}

/**
 * @brief Returns the output stream.
 * 
 * @retval std::ofstream&
 */
auto Files::output() -> std::ofstream&
{
  return m_output;
}

/**
 * @brief Flushes the output stream.
 */
auto Files::flush() -> void
{
  m_output.flush();
}

/**
 * @brief Gets the file size from the input stream.
 * 
 * @retval std::streamsize
 */
auto Files::sizeIn() -> std::streamsize
{
  m_input.ignore(std::numeric_limits<std::streamsize>::max());
  auto length = m_input.gcount();
  m_input.clear(); //  Since ignore will have set eof.
  m_input.seekg(0U, std::ios_base::beg);
  return length;
}

/**
 * @brief Rewind the input stream.
 */
auto Files::rewindIn() -> void
{
  m_input.clear();
  m_input.seekg(0);
}

/**
 * @brief Advance the input stream.
 * 
 * @param[in] length Number of bytes.
 */
auto Files::advanceIn(std::uint32_t length) -> void
{
  m_input.seekg(length, std::ios::cur);
}

/**
 * @brief Reads the stream.
 * 
 * @param[out] output Where to store read data.
 * @param[in] len Read len. 
 */
auto Files::read(char* output, std::uint32_t len) -> long long
{
  m_input.read(&output[0], len);
  return m_input.gcount();
}

/**
 * @brief Reads a line from the input stream.
 * 
 * @param[out] output Where to store read data.
 */
auto Files::getline(std::string& output) -> std::istream&
{
  return std::getline(m_input, output);
}

/* Private functions --------------------------------------------------------*/
/**
 * @brief Opens a file.
 * 
 * @param[in,out] stream The file stream.
 * @param[in] path The file path.
 * @param[in] mode The opening mode.
 * @retval False if error, otherwise true.
 * @retval OpenResult.
 */
template <class Stream>
auto Files::openFile(Stream& stream, std::string_view path, std::ios_base::openmode mode) const -> OpenResult
{
  using enum OpenResult;
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