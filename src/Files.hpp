/**
 * @file Files.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <cstdint>
#include <fstream>
#include <vector>
#include "config.h"

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b
{
  class Files
  {
    public:
      enum class OpenStatus : std::uint8_t
      {
        AllOpened = 0,
        AllClosed = 1,
        OutputClosed = 2,
        InputClosed = 3
      };

      enum class OpenResult : std::uint8_t
      {
        Success = 0,
        Error = 1,
        Already = 2,
      };

      Files() = default;

      /**
       * @brief Opens the input file.
       * 
       * @param[in] path The file path.
       * @retval OpenResult.
       */
      auto openInput(std::string_view path) -> OpenResult;

      /**
       * @brief Opens the output file.
       * 
       * @param[in] path The file path.
       * @retval OpenResult.
       */
      auto openOutput(std::string_view path) -> OpenResult;

      /**
       * @brief Closes the files.
       */
      auto close() -> void;

      /**
       * @brief Test if the files are open.
       * 
       * @retval OpenStatus.
       */
      auto isFilesOpen() const -> OpenStatus;

      /**
       * @brief Returns the input stream.
       * 
       * @retval std::ifstream&
       */
      auto input() -> std::ifstream&;

      /**
       * @brief Returns the output stream.
       * 
       * @retval std::ofstream&
       */
      auto output() -> std::ofstream&;

      /**
       * @brief Flushes the output stream.
       */
      auto flush() -> void;

      /**
       * @brief Gets the file size from the input stream.
       * 
       * @retval std::streamsize
       */
      auto sizeIn() -> std::streamsize;

      /**
       * @brief Rewind the input stream.
       */
      auto rewindIn() -> void;

      /**
       * @brief Advance the input stream.
       * 
       * @param[in] length Number of bytes.
       */
      auto advanceIn(std::uint32_t length) -> void;

      /**
       * @brief Reads the stream.
       * 
       * @param[out] output Where to store read data.
       * @param[in] len Read len. 
       */
      auto read(char* output, std::uint32_t len) -> long long;

      /**
       * @brief Reads a line from the input stream.
       * 
       * @param[out] output Where to store read data.
       */
      auto getline(std::string& output) -> std::istream&;
    private:
      std::ofstream m_output{};
      std::ifstream m_input{};

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
      auto openFile(Stream& stream, std::string_view path, std::ios_base::openmode mode) const -> OpenResult;
  };
} // namespace h2b
