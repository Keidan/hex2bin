/**
 * @file Helper.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <cstdint>
#include <vector>
#include <string>
#include <regex>
#include <exception>
#include <algorithm>
#include <sstream>
#include <iomanip>

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b
{
  class Helper
  {
    public:
      virtual ~Helper() = default;

      /**
       * @brief Splits a string according to the specified regex.
       * 
       * @param[in] in The input string.
       * @param[in] reg The regex.
       * @retval The result in a vector.
       */
      static auto split(std::string_view in, std::string_view reg) -> std::vector<std::string>
      {
        // passing -1 as the submatch index parameter performs splitting
        auto sreg = std::string(reg);
        std::regex re(sreg);
        std::string input(in);
        std::sregex_token_iterator first{input.begin(), input.end(), re, -1};
        std::sregex_token_iterator last;
        return {first, last};
      }

      /**
       * @brief Returns a fragment of the input line.
       * 
       * @param[in] line The input line.
       * @param[in] start Start index.
       * @param[in] limit Limit index.
       * @retval The fragment of the input line.
       */
      static auto getFragment(std::string_view line, std::uint32_t start, std::uint32_t limit) -> std::string
      {
        auto len = line.size();
        if(len < start)
        {
          len = 0UL;
        }
        const auto lim = (limit == 0 || limit > len) ? len : limit;
        return std::string(line.substr(std::min(start, static_cast<std::uint32_t>(len)), lim));
      }

      /**
       * @brief Searches for a string in another.
       * 
       * @param[in] ref The reference string.
       * @param[in] needle The string to search.
       * @param[in] ignoreCase True for case-insensitive.
       * @retval bool
       */
      static auto search(std::string_view ref, std::string_view needle, bool ignoreCase = false) -> bool
      {
        const auto found = std::ranges::search(
          ref, needle, [ignoreCase](const char c1, const char c2) { return ignoreCase ? (std::toupper(c1) == std::toupper(c2)) : (c1 == c2); });
        return !found.empty() && ref.end() != found.end();
      }

      /**
       * @brief Sets the value from a string.
       * 
       * @param[out] output Output value.
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      static auto setValueFromstring(std::uint32_t& output, std::string_view value, std::string& what) -> bool
      {
        try
        {
          auto sv = std::string(value);
          std::int32_t val;
          if(value.starts_with("0x"))
          {
            sv = sv.substr(2);
            val = hex2int<std::int32_t>(sv);
          }
          else
            val = std::stoi(sv);
          if(val < 0)
          {
            val = 0U;
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
       * @brief Converts a hexadecimal string to unsigned interger.
       * 
       * @param[in] hex Hexadecimal string.
       * @retval T
       */
      template <typename T>
      static auto hex2int(std::string_view hex) -> T
      {
        int t;
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> t;
        return static_cast<T>(t);
      }

      /**
       * @brief Converts an integer to a hexadecimal string.
       * 
       * @param[in] t integer.
       * @retval std::string
       */
      template <typename T>
      static auto int2hex(T t, char fill = '0', int width = 1) -> std::string
      {
        std::stringstream ss;
        ss << std::hex;
        ss << std::setfill(fill);
        ss << std::setw(width);
        ss << t;
        return ss.str();
      }
    private:
      Helper() = default;
  };
} // namespace h2b
