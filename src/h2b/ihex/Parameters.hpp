/**
 * @file Parameters.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <cstdint>
#include <string>
#include <config.h>
#include <h2b/utils/Helper.hpp>

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b::ihex
{

  class Parameters
  {
    public:
      Parameters() = default;
      virtual ~Parameters() = default;

      /**
       * @brief Sets the width (used for writing).
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto width(std::string_view value, std::string& what) -> bool
      {
        auto ret = utils::Helper::setValueFromstring(m_width, value, what);
        if(ret)
        {
          if(!m_width)
            what = "The value cannot be equal to 0.";
          else if(m_width > 0xFF)
            what = "The value cannot exceed 255 (0xFF).";
          ret = what.empty();
        }
        return ret;
      }

      /**
       * @brief Gets the width (used for writing).
       * 
       * @retval std::uint32_t.
       */
      auto width() const -> std::uint32_t
      {
        return m_width;
      }

      /**
       * @brief Sets the padding.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto padding(std::string_view value, std::string& what) -> bool
      {
        std::uint32_t padding;
        auto ret = utils::Helper::setValueFromstring(padding, value, what);
        if(ret)
        {
          if(padding > 0xFF)
            what = "The padding value cannot exceed 255 (0xFF).";
          else
            m_padding = static_cast<std::uint8_t>(padding);
          ret = what.empty();
        }
        return ret;
      }

      /**
       * @brief Gets the padding.
       * 
       * @retval std::uint8_t.
       */
      auto padding() const -> std::uint8_t
      {
        return m_padding;
      }

      /**
       * @brief Sets the padding width.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto paddingWidth(std::string_view value, std::string& what) -> bool
      {
        std::uint32_t padding;
        auto ret = utils::Helper::setValueFromstring(padding, value, what);
        if(ret)
        {
          if(padding > 0xFF)
            what = "The padding width value cannot exceed 255 (0xFF).";
          else
            m_paddingWidth = static_cast<std::uint8_t>(padding);
          ret = what.empty();
        }
        return ret;
      }

      /**
       * @brief Gets the padding width.
       * 
       * @retval std::uint8_t.
       */
      auto paddingWidth() const -> std::uint8_t
      {
        return m_paddingWidth;
      }

      /**
       * @brief Sets the address offset.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto offset(std::string_view value, std::string& what) -> bool
      {
        return utils::Helper::setValueFromstring(m_addrOffset, value, what);
      }

      /**
       * @brief Gets the address offset.
       * 
       * @retval std::uint32_t.
       */
      auto addressOffset() const -> std::uint32_t
      {
        return m_addrOffset;
      }

      /**
       * @brief Gets/Sets the address offset.
       * 
       * @retval std::uint32_t&.
       */
      auto addressOffset() -> std::uint32_t&
      {
        return m_addrOffset;
      }

      /**
       * @brief Sets the start linear.
       * 
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      auto linear(std::string_view value, std::string& what) -> bool
      {
        auto r = utils::Helper::setValueFromstring(m_startLinear, value, what);
        return (m_startLinearFound = r);
      }

      /**
       * @brief Gets the start linear.
       * 
       * @retval std::uint32_t.
       */
      auto linear() const -> std::uint32_t
      {
        return m_startLinear;
      }

      /**
       * @brief Gets/sets the start linear.
       * 
       * @retval std::uint32_t&.
       */
      auto linear() -> std::uint32_t&
      {
        return m_startLinear;
      }

      /**
       * @brief Use of the "extended segment" block instead of "extended linear" for the offset parameter.
       */
      auto segment() -> void
      {
        m_useSegment = true;
      }

      /**
       * @brief Is "extended segment" block used instead of "extended linear" for the offset parameter.
       * 
       * @retval bool
       */
      auto useSegment() const -> bool
      {
        return m_useSegment;
      }

      /**
       * @brief Is startLinear found.
       * 
       * @retval bool
       */
      auto startLinearFound() const -> bool
      {
        return m_startLinearFound;
      }

      /**
       * @brief Is startLinear found.
       */
      auto startLinearFound() -> bool&
      {
        return m_startLinearFound;
      }

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
