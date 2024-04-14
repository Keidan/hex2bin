/**
 * @file Parameters.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "Parameters.hpp"
#include "../Helper.hpp"
/* Usings -------------------------------------------------------------------*/
using namespace h2b::ihex;

/**
 * @brief Sets the width (used for writing).
 * 
 * @param[in] value Integer value in string format.
 * @param[out] what The cause of the error (if the function returns false).
 * @retval False if error, otherwise true.
 */
auto Parameters::width(std::string_view value, std::string& what) -> bool
{
  auto ret = Helper::setValueFromstring(m_width, value, what);
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
auto Parameters::width() const -> std::uint32_t
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
auto Parameters::padding(std::string_view value, std::string& what) -> bool
{
  std::uint32_t padding;
  auto ret = Helper::setValueFromstring(padding, value, what);
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
auto Parameters::padding() const -> std::uint8_t
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
auto Parameters::paddingWidth(std::string_view value, std::string& what) -> bool
{
  std::uint32_t padding;
  auto ret = Helper::setValueFromstring(padding, value, what);
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
auto Parameters::paddingWidth() const -> std::uint8_t
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
auto Parameters::offset(std::string_view value, std::string& what) -> bool
{
  return Helper::setValueFromstring(m_addrOffset, value, what);
}

/**
 * @brief Gets the address offset.
 * 
 * @retval std::uint32_t.
 */
auto Parameters::addressOffset() const -> std::uint32_t
{
  return m_addrOffset;
}

/**
 * @brief Gets/Sets the address offset.
 * 
 * @retval std::uint32_t&.
 */
auto Parameters::addressOffset() -> std::uint32_t&
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
auto Parameters::linear(std::string_view value, std::string& what) -> bool
{
  auto r = Helper::setValueFromstring(m_startLinear, value, what);
  return (m_startLinearFound = r);
}

/**
 * @brief Gets the start linear.
 * 
 * @retval std::uint32_t.
 */
auto Parameters::linear() const -> std::uint32_t
{
  return m_startLinear;
}

/**
 * @brief Gets/sets the start linear.
 * 
 * @retval std::uint32_t&.
 */
auto Parameters::linear() -> std::uint32_t&
{
  return m_startLinear;
}

/**
 * @brief Use of the "extended segment" block instead of "extended linear" for the offset parameter.
 */
auto Parameters::segment() -> void
{
  m_useSegment = true;
}

/**
 * @brief Is "extended segment" block used instead of "extended linear" for the offset parameter.
 * 
 * @retval bool
 */
auto Parameters::useSegment() const -> bool
{
  return m_useSegment;
}

/**
 * @brief Is startLinear found.
 * 
 * @retval bool
 */
auto Parameters::startLinearFound() const -> bool
{
  return m_startLinearFound;
}

/**
 * @brief Is startLinear found.
 */
auto Parameters::startLinearFound() -> bool&
{
  return m_startLinearFound;
}