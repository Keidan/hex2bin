/**
 * @file Parameters.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <cstdint>
#include <string>
#include "../config.h"

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b::ihex
{

  class Parameters
  {
    public:
      Parameters() = default;

      /**
       * @brief Sets the width (used for writing).
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto width(std::string_view value, std::string& what) -> bool;

      /**
       * @brief Gets the width (used for writing).
       * 
       * @retval std::uint32_t.
       */
      auto width() const -> std::uint32_t;

      /**
       * @brief Sets the padding.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto padding(std::string_view value, std::string& what) -> bool;

      /**
       * @brief Gets the padding.
       * 
       * @retval std::uint8_t.
       */
      auto padding() const -> std::uint8_t;

      /**
       * @brief Sets the padding width.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto paddingWidth(std::string_view value, std::string& what) -> bool;

      /**
       * @brief Gets the padding width.
       * 
       * @retval std::uint8_t.
       */
      auto paddingWidth() const -> std::uint8_t;

      /**
       * @brief Sets the address offset.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto offset(std::string_view value, std::string& what) -> bool;

      /**
       * @brief Gets the address offset.
       * 
       * @retval std::uint32_t.
       */
      auto addressOffset() const -> std::uint32_t;

      /**
       * @brief Gets/Sets the address offset.
       * 
       * @retval std::uint32_t&.
       */
      auto addressOffset() -> std::uint32_t&;

      /**
       * @brief Sets the start linear.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto linear(std::string_view value, std::string& what) -> bool;

      /**
       * @brief Gets the start linear.
       * 
       * @retval std::uint32_t.
       */
      auto linear() const -> std::uint32_t;

      /**
       * @brief Gets/sets the start linear.
       * 
       * @retval std::uint32_t&.
       */
      auto linear() -> std::uint32_t&;

      /**
       * @brief Use of the "extended segment" block instead of "extended linear" for the offset parameter.
       */
      auto segment() -> void;

      /**
       * @brief Is "extended segment" block used instead of "extended linear" for the offset parameter.
       * 
       * @retval bool
       */
      auto useSegment() const -> bool;

      /**
       * @brief Is startLinear found.
       * 
       * @retval bool
       */
      auto startLinearFound() const -> bool;

      /**
       * @brief Is startLinear found.
       */
      auto startLinearFound() -> bool&;

    private:
      std::uint32_t m_addrOffset = 0U;
      std::uint32_t m_width = DEFAULT_WIDTH;
      std::uint32_t m_startLinear = 0U;
      std::uint8_t m_padding = 0xFFU;
      std::uint8_t m_paddingWidth = 0U;
      bool m_useSegment = false;
      bool m_startLinearFound = false;
  };
} // namespace h2b::ihex
