/*
 * @file main_test.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "Hex2Bin.hpp"
#include "gtest/gtest.h"

/* Private defines ----------------------------------------------------------*/
static constexpr auto* SAMPLE1 = "samples/sample1.txt";
static constexpr auto* SAMPLE2 = "samples/sample2.txt";
static constexpr auto* SAMPLE3 = "samples/sample3.txt";
static constexpr auto* SAMPLE_TEMP = "samples/sample.txt.temp";

/* Tests --------------------------------------------------------------------*/
TEST(Hex2BinTest, TestOpenInput)
{
  h2b::Hex2Bin hex2bin;
  const auto file = SAMPLE1;
  const auto ret = hex2bin.openInput(file);
  EXPECT_EQ(1, ret == h2b::Hex2BinOpenResult::Success);
}

TEST(Hex2BinTest, TestOpenOutput)
{
  h2b::Hex2Bin hex2bin{};
  const auto file = SAMPLE_TEMP;
  const auto ret = hex2bin.openOutput(file);
  EXPECT_EQ(1, ret == h2b::Hex2BinOpenResult::Success);
  std::remove(SAMPLE_TEMP);
}

TEST(Hex2BinTest, TestOpenFiles)
{
  h2b::Hex2Bin hex2bin{};
  const auto fileIn = SAMPLE1;
  const auto fileOut = SAMPLE_TEMP;
  const auto retIn = hex2bin.openInput(fileIn);
  const auto retOut = hex2bin.openOutput(fileOut);
  const auto oret = hex2bin.isFilesOpen();

  std::remove(fileOut);

  EXPECT_EQ(1, retIn == h2b::Hex2BinOpenResult::Success);
  EXPECT_EQ(1, retOut == h2b::Hex2BinOpenResult::Success);
  EXPECT_EQ(1, oret == h2b::Hex2BinIsOpen::Success);
}

TEST(Hex2BinTest, TestStart)
{
  h2b::Hex2Bin hex2bin{};
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
  h2b::Hex2Bin hex2bin{};
  const auto svalue = "16";
  const auto value = 16U;
  std::string what{};

  EXPECT_EQ(1, hex2bin.setLimit(svalue, what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isValidLimit() == true);
  EXPECT_EQ(1, hex2bin.getLimit() == value);
}

static auto testExtractNoPrintWithStart(h2b::Hex2Bin& hex2bin, const std::string& sstart, const std::string& fileIn, const std::string& fileOut) -> void
{
  std::string what{};

  EXPECT_EQ(1, hex2bin.setStart(sstart, what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isValidStart() == true);

  const auto retIn = hex2bin.openInput(fileIn);
  const auto retOut = hex2bin.openOutput(fileOut);
  const auto oret = hex2bin.isFilesOpen();

  EXPECT_EQ(1, retIn == h2b::Hex2BinOpenResult::Success);
  EXPECT_EQ(1, retOut == h2b::Hex2BinOpenResult::Success);
  EXPECT_EQ(1, oret == h2b::Hex2BinIsOpen::Success);
  EXPECT_EQ(1, hex2bin.extractNoPrint());

  std::remove(fileOut.c_str());
}

TEST(Hex2BinTest, TestExtractNoPrint1)
{
  h2b::Hex2Bin hex2bin{};
  std::string what{};

  EXPECT_EQ(1, hex2bin.setLimit("47", what) == true);
  EXPECT_EQ(1, what.empty() == true);

  testExtractNoPrintWithStart(hex2bin, "6", SAMPLE1, SAMPLE_TEMP);
}

TEST(Hex2BinTest, TestExtractNoPrint2)
{
  h2b::Hex2Bin hex2bin{};
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

  EXPECT_EQ(1, retIn == h2b::Hex2BinOpenResult::Success);
  EXPECT_EQ(1, retOut == h2b::Hex2BinOpenResult::Success);
  EXPECT_EQ(1, oret == h2b::Hex2BinIsOpen::Success);
  EXPECT_EQ(1, hex2bin.extractNoPrint());

  std::remove(fileOut);
}

TEST(Hex2BinTest, TestExtractNoPrint3)
{
  h2b::Hex2Bin hex2bin{};
  testExtractNoPrintWithStart(hex2bin, "1", SAMPLE3, SAMPLE_TEMP);
}

/* Public function ----------------------------------------------------------*/
auto main(int argc, char** argv) -> int
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


