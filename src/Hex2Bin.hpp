/*
 * @file Hex2Bin.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#ifndef __HEX2BIN_HPP__
#define __HEX2BIN_HPP__

/* Includes -----------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include <vector>

/* Usings -------------------------------------------------------------------*/
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using std::int32_t;
using std::uint32_t;


/* Public defines -----------------------------------------------------------*/
#ifndef DEFAULT_START
#define DEFAULT_START 0
#endif /* DEFAULT_START */
#ifndef DEFAULT_LIMIT
#define DEFAULT_LIMIT 0
#endif /* DEFAULT_LIMIT */

/* Public class -------------------------------------------------------------*/

enum class Hex2BinIsOpen : std::uint8_t
{
  SUCCESS = 0,
  BOTH = 1,
  INPUT = 2,
  OUTPUT = 3
};
enum class Hex2BinOpenResult : std::uint8_t
{
  SUCCESS = 0,
  ERROR = 1,
  ALREADY = 2,
};

class Hex2Bin
{
  public:
    Hex2Bin();
    virtual ~Hex2Bin();

    /**
     * @brief Opens the input file.
     * @param[in] path The file path.
     * @retval Hex2BinOpenResult.
     */
    auto openInput(const std::string& path) -> Hex2BinOpenResult;
    
    /**
     * @brief Opens the input file.
     * @param[in] path The file path.
     * @retval Hex2BinOpenResult.
     */
    auto openOutput(const std::string& path) -> Hex2BinOpenResult;

    /**
     * @brief Sets the starting value.
     * @param[in] value Integer value in string format.
     * @param[out] what The cause of the error (if the function returns false).
     * @retval False if error, otherwise true.
     */
    auto setStart(const std::string& value, std::string &what) -> bool;

    /**
     * @brief Tests whether the start value is valid or not.
     * @retval True if it is valid, otherwise false.
     */
    auto isValidStart() -> bool;

    /**
     * @brief Returns the start value.
     * @retval std::uint32_t
     */
    auto getStart() -> std::uint32_t;

    /**
     * @brief Sets the limit value.
     * @param[in] value Integer value in string format.
     * @param[out] what The cause of the error (if the function returns false).
     * @retval False if error, otherwise true.
     */
    auto setLimit(const std::string& value, std::string &what) -> bool;
    
    /**
     * @brief Tests whether the limit value is valid or not.
     * @retval True if it is valid, otherwise false.
     */
    auto isValidLimit() -> bool;

    /**
     * @brief Returns the limit value.
     * @retval std::uint32_t
     */
    auto getLimit() -> std::uint32_t;

    /**
     * @brief Test if the files are open.
     * @retval Hex2BinIsOpen.
     */
    auto isFilesOpen() -> Hex2BinIsOpen;

    /**
     * @brief Only extracts words from "start" to "limit".
     */
    auto extractOnly() -> void;

    /**
     * @brief Extracts and without converting all printable characters.
     * @param[in] output The output file.
     * @param[in] input The input file.
     * @param[in] start The start offset.
     * @param[in] limit The limit per lines.
     * @retval False on error.
     */
    auto extractNoPrint() -> bool;

    /**
     * @brief Extracts and converts all printable characters.
     * @retval Returns false on error, true else.
     */
    auto extractPrint() -> bool;


  private:
    std::uint32_t m_start;
    std::uint32_t m_limit;
    std::ofstream m_output;
    std::ifstream m_input;

    /**
     * @brief Splits a string according to the specified regex
     * @param[in] in The input string.
     * @param[in] reg The regex.
     * @retval The result in a vector.
     */
    auto split(const std::string& in, const std::string &reg) -> std::vector<std::string>;
    
    /**
     * @brief Returns a fragment of the input line.
     * @param[in] line The input line.
     * @retval The fragment of the input line.
     */
    auto getFragment(const std::string &line) -> std::string;

    /**
     * @brief Searches for a string in another.
     * @param[in] ref The reference string.
     * @param[in] needle The string to search.
     * @param[in] ignoreCase True for case-insensitive.
     * @retval bool
     */
    auto search(const std::string &ref, const std::string &needle, bool ignoreCase = false) -> bool;

    /**
     * @brief Validates the line and displays an error message if the validation fails.
     * @param[in] line The reference line.
     * @param[in] s The string to validate.
     * @retval bool
     */
    auto validateHexAndLogOnError(const std::string &line, const std::string &s) -> bool;

};

#endif /* __HEX2BIN_HPP__ */
