/**
 * @file Hex2Bin.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <cstdint>
#include <vector>
#include <memory>
#include <config.h>
#include <h2b/utils/Files.hpp>

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
  class Hex2Bin
  {
    public:
      explicit Hex2Bin(utils::Files* files);
      virtual ~Hex2Bin() = default;

      /**
       * @brief Sets the starting value.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto start(std::string_view value, std::string& what) -> bool;

      /**
       * @brief Tests whether the start value is valid or not.
       * 
       * @retval True if it is valid, otherwise false.
       */
      auto isStart() const -> bool;

      /**
       * @brief Returns the start value.
       * 
       * @retval std::uint32_t
       */
      auto start() const -> std::uint32_t;

      /**
       * @brief Sets the limit value.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto limit(std::string_view value, std::string& what) -> bool;

      /**
       * @brief Tests whether the limit value is valid or not.
       * 
       * @retval True if it is valid, otherwise false.
       */
      auto isLimit() const -> bool;

      /**
       * @brief Returns the limit value.
       * 
       * @retval std::uint32_t
       */
      auto limit() const -> std::uint32_t;

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
       * 
       * @retval Returns false on error, true else.
       */
      auto extractPrint() -> bool;

    private:
      utils::Files* m_files;
      std::uint32_t m_start = DEFAULT_START;
      std::uint32_t m_limit = DEFAULT_LIMIT;

      /**
       * @brief Validates the line and displays an error message if the validation fails.
       * 
       * @param[in] line The reference line.
       * @param[in] s The string to validate.
       * @retval bool
       */
      auto validateHexAndLogOnError(std::string_view line, std::string_view s) const -> bool;

      /**
       * @brief Extracts and without converting all printable characters (sub loop 1).
       * 
       * @param[in] fragment A fragment of the input line.
       * @param[out] error Error?
       */
      auto extractNoPrintSpaceFound(std::string_view fragment, bool& error) -> void;

      /**
       * @brief Extracts and without converting all printable characters (sub loop 2).
       * 
       * @param[in] fragment A fragment of the input line.
       * @param[out] error Error?
       */
      auto extractNoPrintNoSpaceFound(std::string_view fragment, bool& error) -> void;
  };
} // namespace h2b
