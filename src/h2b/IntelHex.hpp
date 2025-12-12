/**
 * @file IntelHex.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 * 
 * See https://developer.arm.com/documentation/ka003292/latest/
 * :llaaaatt[dd...]cc
 * ':' is the colon that starts every Intel HEX record.
 * 'll' is the record-length field that represents the number of data bytes (dd) in the record.
 * 'aaaa' is the address field that represents the starting address for subsequent data in the record.
 * 'tt' is the field that represents the HEX record type, which may be one of the following:
 *    00 - data record.
 *    01 - end-of-file record.
 *    02 - extended segment address record.
 *    04 - extended linear address record.
 *    05 - start linear address record (MDK-ARM only).
 * 'dd' is a data field that represents one byte of data. 
 *      A record may have multiple data bytes.
 *      The number of data bytes in the record must match the number specified by the ll field.
 * 'cc' is the checksum field that represents the checksum of the record.
 *      The checksum is calculated by summing the values of all hexadecimal digit pairs in the record modulo 256
 *      and taking the two's complement.
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <h2b/utils/Files.hpp>
#include <h2b/ihex/Parameters.hpp>
#include <h2b/ihex/Bin2Intel.hpp>
#include <h2b/ihex/Intel2Bin.hpp>
#include <map>

/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace h2b
{
  class IntelHex : public ihex::Parameters
  {
    public:
      explicit IntelHex(utils::Files* files);

      /**
       * @brief Convert intel to binary.
       * 
       * @param[in] summary Print summary.
       * @retval false on error.
       */
      auto intel2bin(bool summary = true) -> bool;

      /**
       * @brief Convert binary to intel.
       * 
       * @param[in] summary Print summary.
       * @retval false on error.
       */
      auto bin2intel(bool summary = true) -> bool;

    private:
      utils::Files* m_files;
      ihex::Bin2Intel m_bin2Intel;
      ihex::Intel2Bin m_intel2Bin;
  };
} // namespace h2b
