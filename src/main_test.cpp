/*
 * @file main_test.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "Hex2Bin.hpp"
#include "gtest/gtest.h"
#include <filesystem>

/* Private defines ----------------------------------------------------------*/
static constexpr auto* SAMPLE1 = "samples/sample1.txt";
static constexpr auto* SAMPLE2 = "samples/sample2.txt";
static constexpr auto* SAMPLE3 = "samples/sample3.txt";
static constexpr auto* SAMPLE_TEMP = "samples/sample.txt.temp";

using h2b::Hex2Bin;
using h2b::Hex2BinOpenResult;
using h2b::Hex2BinIsOpen;
namespace fs = std::filesystem;

/* Tests --------------------------------------------------------------------*/
static auto cleanup(Hex2Bin& hex2bin, std::string_view path) -> void
{
  hex2bin.close();
  fs::remove(path);
}

TEST(Hex2BinTest, TestOpenInput)
{
  Hex2Bin hex2bin{};
  const auto file = SAMPLE1;
  const auto ret = hex2bin.openInput(file);
  EXPECT_EQ(1, ret == Hex2BinOpenResult::Success);
}

TEST(Hex2BinTest, TestOpenOutput)
{
  Hex2Bin hex2bin{};
  const auto file = SAMPLE_TEMP;
  const auto ret = hex2bin.openOutput(file);
  const auto oretInput = hex2bin.isFilesOpen();
  cleanup(hex2bin, SAMPLE_TEMP);
  EXPECT_EQ(1, ret == Hex2BinOpenResult::Success);
  EXPECT_EQ(1, oretInput == Hex2BinIsOpen::Input);
}

TEST(Hex2BinTest, TestOpenFiles)
{
  Hex2Bin hex2bin{};
  const auto fileIn = SAMPLE1;
  const auto fileOut = SAMPLE_TEMP;
  const auto oretBoth = hex2bin.isFilesOpen();
  const auto retIn = hex2bin.openInput(fileIn);
  const auto oretOutput = hex2bin.isFilesOpen();
  const auto retOut = hex2bin.openOutput(fileOut);
  const auto oretSuccess = hex2bin.isFilesOpen();
  cleanup(hex2bin, fileOut);

  EXPECT_EQ(1, retIn == Hex2BinOpenResult::Success);
  EXPECT_EQ(1, retOut == Hex2BinOpenResult::Success);
  EXPECT_EQ(1, oretBoth == Hex2BinIsOpen::Both);
  EXPECT_EQ(1, oretOutput == Hex2BinIsOpen::Output);
  EXPECT_EQ(1, oretSuccess == Hex2BinIsOpen::Success);
}

TEST(Hex2BinTest, TestStart)
{
  Hex2Bin hex2bin{};
  const auto svalue = "32";
  const auto value = 32U;
  std::string what{};

  EXPECT_EQ(1, hex2bin.setStart(svalue, what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isValidStart() == true);
  EXPECT_EQ(1, hex2bin.getStart() == value);
}

TEST(Hex2BinTest, TestLimit)
{
  Hex2Bin hex2bin{};
  const auto svalue = "16";
  const auto value = 16U;
  std::string what{};

  EXPECT_EQ(1, hex2bin.setLimit(svalue, what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isValidLimit() == true);
  EXPECT_EQ(1, hex2bin.getLimit() == value);
}

static auto testExtractNoPrintWithStart(Hex2Bin& hex2bin, std::string_view sstart, std::string_view fileIn, std::string_view fileOut) -> void
{
  std::string what{};

  EXPECT_EQ(1, hex2bin.setStart(sstart, what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isValidStart() == true);

  const auto retIn = hex2bin.openInput(fileIn);
  const auto retOut = hex2bin.openOutput(fileOut);
  const auto oret = hex2bin.isFilesOpen();
  const auto eret = hex2bin.extractNoPrint();
  cleanup(hex2bin, fileOut);

  EXPECT_EQ(1, retIn == Hex2BinOpenResult::Success);
  EXPECT_EQ(1, retOut == Hex2BinOpenResult::Success);
  EXPECT_EQ(1, oret == Hex2BinIsOpen::Success);
  EXPECT_EQ(1, eret);
}

TEST(Hex2BinTest, TestExtractNoPrint1)
{
  Hex2Bin hex2bin{};
  std::string what{};

  EXPECT_EQ(1, hex2bin.setLimit("47", what) == true);
  EXPECT_EQ(1, what.empty() == true);

  testExtractNoPrintWithStart(hex2bin, "6", SAMPLE1, SAMPLE_TEMP);
}

TEST(Hex2BinTest, TestExtractNoPrint2)
{
  Hex2Bin hex2bin{};
  const auto slimit = "47";
  const auto fileIn = SAMPLE2;
  const auto fileOut = SAMPLE_TEMP;
  std::string what{};

  EXPECT_EQ(1, hex2bin.setLimit(slimit, what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isValidLimit() == true);

  const auto retIn = hex2bin.openInput(fileIn);
  const auto retOut = hex2bin.openOutput(fileOut);
  const auto oret = hex2bin.isFilesOpen();
  const auto eret = hex2bin.extractNoPrint();
  cleanup(hex2bin, fileOut);

  EXPECT_EQ(1, retIn == Hex2BinOpenResult::Success);
  EXPECT_EQ(1, retOut == Hex2BinOpenResult::Success);
  EXPECT_EQ(1, oret == Hex2BinIsOpen::Success);
  EXPECT_EQ(1, eret);
}

TEST(Hex2BinTest, TestExtractNoPrint3)
{
  Hex2Bin hex2bin{};
  testExtractNoPrintWithStart(hex2bin, "1", SAMPLE3, SAMPLE_TEMP);
}

/* Public function ----------------------------------------------------------*/
auto main(int argc, char** argv) -> int
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
