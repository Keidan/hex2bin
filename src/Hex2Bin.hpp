/*
 * @file Hex2Bin.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#ifndef __HEX2BIN_HPP__
#define __HEX2BIN_HPP__

/* Includes -----------------------------------------------------------------*/
#include <cstdint>
#include <fstream>
#include <vector>
#include "config.h"

/* Public defines -----------------------------------------------------------*/
#ifndef DEFAULT_START
#  define DEFAULT_START 0
#endif /* DEFAULT_START */
#ifndef DEFAULT_LIMIT
#  define DEFAULT_LIMIT 0
#endif /* DEFAULT_LIMIT */

/* Public class -------------------------------------------------------------*/
namespace h2b
{
  enum class Hex2BinIsOpen : std::uint8_t
  {
    Success = 0,
    Both = 1,
    Input = 2,
    Output = 3
  };

  enum class Hex2BinOpenResult : std::uint8_t
  {
    Success = 0,
    Error = 1,
    Already = 2,
  };

  class Hex2Bin
  {
    public:
      Hex2Bin() = default;

      /**
       * @brief Opens the input file.
       * @param[in] path The file path.
       * @retval Hex2BinOpenResult.
       */
      auto openInput(std::string_view path) -> Hex2BinOpenResult;

      /**
       * @brief Opens the output file.
       * @param[in] path The file path.
       * @retval Hex2BinOpenResult.
       */
      auto openOutput(std::string_view path) -> Hex2BinOpenResult;

      /**
       * @brief Closes the files.
       */
      auto close() -> void;

      /**
       * @brief Sets the starting value.
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto setStart(std::string_view value, std::string& what) -> bool;

      /**
       * @brief Tests whether the start value is valid or not.
       * @retval True if it is valid, otherwise false.
       */
      auto isValidStart() const -> bool;

      /**
       * @brief Returns the start value.
       * @retval std::uint32_t
       */
      auto getStart() const -> std::uint32_t;

      /**
       * @brief Sets the limit value.
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto setLimit(std::string_view value, std::string& what) -> bool;

      /**
       * @brief Tests whether the limit value is valid or not.
       * @retval True if it is valid, otherwise false.
       */
      auto isValidLimit() const -> bool;

      /**
       * @brief Returns the limit value.
       * @retval std::uint32_t
       */
      auto getLimit() const -> std::uint32_t;

      /**
       * @brief Test if the files are open.
       * @retval Hex2BinIsOpen.
       */
      auto isFilesOpen() const -> Hex2BinIsOpen;

      /**
       * @brief Only extracts words from "start" to "limit".
       */
      auto extractOnly() -> void;

      /**
       * @brief Extracts and without converting all printable characters.
       * @retval False on error.
       */
      auto extractNoPrint() -> bool;

      /**
       * @brief Extracts and converts all printable characters.
       * @retval Returns false on error, true else.
       */
      auto extractPrint() -> bool;

    private:
      std::uint32_t m_start = DEFAULT_START;
      std::uint32_t m_limit = DEFAULT_LIMIT;
      std::ofstream m_output{};
      std::ifstream m_input{};

      /**
       * @brief Splits a string according to the specified regex
       * @param[in] in The input string.
       * @param[in] reg The regex.
       * @retval The result in a vector.
       */
      static auto split(std::string_view in, std::string_view reg) -> std::vector<std::string>;

      /**
       * @brief Returns a fragment of the input line.
       * @param[in] line The input line.
       * @retval The fragment of the input line.
       */
      auto getFragment(std::string_view line) const -> std::string;

      /**
       * @brief Searches for a string in another.
       * @param[in] ref The reference string.
       * @param[in] needle The string to search.
       * @param[in] ignoreCase True for case-insensitive.
       * @retval bool
       */
      auto search(std::string_view ref, std::string_view needle, bool ignoreCase = false) const -> bool;

      /**
       * @brief Validates the line and displays an error message if the validation fails.
       * @param[in] line The reference line.
       * @param[in] s The string to validate.
       * @retval bool
       */
      auto validateHexAndLogOnError(std::string_view line, std::string_view s) const -> bool;

      /**
       * @brief Opens a file.
       * @param[in,out] stream The file stream.
       * @param[in] path The file path.
       * @param[in] mode The opening mode.
       * @retval False if error, otherwise true.
       * @retval Hex2BinOpenResult.
       */
      template <class Stream>
      auto openFile(Stream& stream, std::string_view path, std::ios_base::openmode mode) const -> Hex2BinOpenResult;

      /**
       * @brief Sets the value from a string.
       * @param[out] output Output value.
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      static auto setValueFromstring(std::uint32_t& output, std::string_view value, std::string& what) -> bool;

      /**
       * @brief Extracts and without converting all printable characters (sub loop 1).
       * @param[in] fragment A fragment of the input line.
       * @param[out] error Error?
       */
      auto extractNoPrintSpaceFound(std::string_view fragment, bool& error) -> void;

      /**
       * @brief Extracts and without converting all printable characters (sub loop 2).
       * @param[in] fragment A fragment of the input line.
       * @param[out] error Error?
       */
      auto extractNoPrintNoSpaceFound(std::string_view fragment, bool& error) -> void;
  };
} // namespace h2b
#endif /* __HEX2BIN_HPP__ */
