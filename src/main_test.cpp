/*
 * @file main_test.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "Hex2Bin.hpp"
#include "gtest/gtest.h"

/* Private defines ----------------------------------------------------------*/
#define SAMPLE1     "sample1.txt"
#define SAMPLE2     "sample2.txt"
#define SAMPLE_TEMP "sample.txt.temp"

/* Tests --------------------------------------------------------------------*/
TEST(Hex2BinTest, TestOpenInput)
{
  Hex2Bin hex2bin;
  auto file = SAMPLE1;
  auto ret = hex2bin.openInput(file);
  EXPECT_EQ(1, ret == Hex2BinOpenResult::SUCCESS);
}

TEST(Hex2BinTest, TestOpenOutput)
{
  Hex2Bin hex2bin;
  auto file = SAMPLE_TEMP;
  auto ret = hex2bin.openOutput(file);
  EXPECT_EQ(1, ret == Hex2BinOpenResult::SUCCESS);
  std::remove(SAMPLE_TEMP);
}

TEST(Hex2BinTest, TestOpenFiles)
{
  Hex2Bin hex2bin;
  auto file_in = SAMPLE1;
  auto file_out = SAMPLE_TEMP;
  auto ret_in = hex2bin.openInput(file_in);
  auto ret_out = hex2bin.openOutput(file_out);
  auto oret = hex2bin.isFilesOpen();
  
  std::remove(file_out);  
  
  EXPECT_EQ(1, ret_in == Hex2BinOpenResult::SUCCESS);
  EXPECT_EQ(1, ret_out == Hex2BinOpenResult::SUCCESS);
  EXPECT_EQ(1, oret == Hex2BinIsOpen::SUCCESS);
}

TEST(Hex2BinTest, TestStart)
{
  Hex2Bin hex2bin;
  auto svalue = "32";
  std::uint32_t value = 32;
  std::string what = "";
  
  EXPECT_EQ(1, hex2bin.setStart(svalue, what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isValidStart() == true);
  EXPECT_EQ(1, hex2bin.getStart() == value);
}

TEST(Hex2BinTest, TestLimit)
{
  Hex2Bin hex2bin;
  auto svalue = "16";
  std::uint32_t value = 16;
  std::string what = "";
  
  EXPECT_EQ(1, hex2bin.setLimit(svalue, what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isValidLimit() == true);
  EXPECT_EQ(1, hex2bin.getLimit() == value);
}

/* Public function ----------------------------------------------------------*/
auto main(int argc, char** argv) -> int
{
  ::testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}



