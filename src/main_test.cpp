/**
 * @file main_test.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "Hex2Bin.hpp"
#include "IntelHex.hpp"
#include "Helper.hpp"
#include "gtest/gtest.h"
#include <filesystem>

/* Private defines ----------------------------------------------------------*/
static constexpr auto* SAMPLE1 = "samples/sample1.txt";
static constexpr auto* SAMPLE2 = "samples/sample2.txt";
static constexpr auto* SAMPLE3 = "samples/sample3.txt";
static constexpr auto* SAMPLE_TEMP = "samples/sample.txt.temp";
static constexpr auto* SAMPLE1_HEX = "samples/sample1.hex";
static constexpr auto* SAMPLE1_BIN = "samples/sample1.bin";
static constexpr auto* SAMPLE2_HEX = "samples/sample2.hex";
static constexpr auto* SAMPLE2_BIN = "samples/sample2.bin";
static constexpr auto* SAMPLE_HEX_TEMP = "samples/sample.hex.temp";
static constexpr auto* SAMPLE_BIN_TEMP = "samples/sample.bin.temp";

using h2b::Hex2Bin;
using h2b::IntelHex;
using h2b::Helper;
using h2b::Files;
namespace fs = std::filesystem;

/* Tests --------------------------------------------------------------------*/
static auto cleanup(Files& files, std::string_view path) -> void
{
  files.close();
  if(fs::exists(path))
    fs::remove(path);
}

TEST(Hex2BinTest, HelperFragment)
{
  const auto dum = std::string("sssssss");
  const auto str = std::string("azerty qwerty");
  const auto toTest = dum + str + dum;
  const auto fragment = Helper::getFragment(toTest, 7, 13);
  EXPECT_EQ(1, fragment == str);
}

TEST(Hex2BinTest, HelperSplit)
{
  const auto sp = Helper::split("azerty qwerty", "\\s+");
  EXPECT_EQ(2, sp.size());
}

TEST(Hex2BinTest, HelperSetValueFromstringSuccess)
{
  std::uint32_t output = 0;
  std::string what;
  const auto ret = Helper::setValueFromstring(output, "1", what);
  EXPECT_EQ(1, ret);
}

TEST(Hex2BinTest, HelperSetValueFromstringInvalidArg)
{
  std::uint32_t output = 0;
  std::string what;
  const auto ret = Helper::setValueFromstring(output, "azerty", what);
  const auto view = std::string_view(what);
  EXPECT_EQ(0, ret);
  EXPECT_EQ(1, view.starts_with("invalid_argument"));
}

TEST(Hex2BinTest, HelperSetValueFromstringOutOfRange)
{
  std::uint32_t output = 0;
  std::string what;
  const auto ret = Helper::setValueFromstring(output, "4294967296", what);
  const auto view = std::string_view(what);
  EXPECT_EQ(0, ret);
  EXPECT_EQ(1, view.starts_with("out_of_range"));
}

TEST(Hex2BinTest, OpenInput)
{
  using enum Files::OpenResult;
  Files files;
  const auto file = SAMPLE1;
  const auto ret = files.openInput(file);
  EXPECT_EQ(1, ret == Success);
}

TEST(Hex2BinTest, OpenOutput)
{
  using enum Files::OpenResult;
  using enum Files::OpenStatus;
  Files files;
  const auto file = SAMPLE_TEMP;
  const auto ret = files.openOutput(file);
  const auto oretInput = files.isFilesOpen();
  cleanup(files, file);
  EXPECT_EQ(1, ret == Success);
  EXPECT_EQ(1, oretInput == InputClosed);
}

static auto testOpenFiles(std::string_view fileIn, std::string_view fileOut) -> void
{
  using enum Files::OpenResult;
  using enum Files::OpenStatus;
  Files files;
  const auto oretBoth = files.isFilesOpen();
  const auto retIn = files.openInput(fileIn);
  const auto oretOutput = files.isFilesOpen();
  const auto retOut = files.openOutput(fileOut);
  const auto oretSuccess = files.isFilesOpen();
  cleanup(files, fileOut);

  EXPECT_EQ(1, retIn == Success);
  EXPECT_EQ(1, retOut == Success);
  EXPECT_EQ(1, oretBoth == AllClosed);
  EXPECT_EQ(1, oretOutput == OutputClosed);
  EXPECT_EQ(1, oretSuccess == AllOpened);

}

TEST(Hex2BinTest, OpenFiles)
{
  testOpenFiles(SAMPLE1, SAMPLE_TEMP);
}

TEST(Hex2BinTest, Start)
{
  Hex2Bin hex2bin{nullptr};
  std::string what{};
  EXPECT_EQ(1, hex2bin.start("32", what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isStart() == true);
  EXPECT_EQ(1, hex2bin.start() == 32U);
}

TEST(Hex2BinTest, Limit)
{
  Hex2Bin hex2bin{nullptr};
  std::string what{};
  EXPECT_EQ(1, hex2bin.limit("16", what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isLimit() == true);
  EXPECT_EQ(1, hex2bin.limit() == 16U);
}

static auto testExtractNoPrintWithStart(Hex2Bin& hex2bin, std::string_view sstart, std::string_view fileIn, std::string_view fileOut) -> void
{
  using enum Files::OpenResult;
  using enum Files::OpenStatus;
  std::string what{};

  EXPECT_EQ(1, hex2bin.start(sstart, what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isStart() == true);

  const auto& files = hex2bin.files();
  const auto retIn = files->openInput(fileIn);
  const auto retOut = files->openOutput(fileOut);
  const auto oret = files->isFilesOpen();
  const auto eret = hex2bin.extractNoPrint();
  cleanup(*files, fileOut);

  EXPECT_EQ(1, retIn == Success);
  EXPECT_EQ(1, retOut == Success);
  EXPECT_EQ(1, oret == AllOpened);
  EXPECT_EQ(1, eret);
}

TEST(Hex2BinTest, ExtractNoPrintSample1)
{
  Files files{};
  Hex2Bin hex2bin{&files};
  std::string what{};
  EXPECT_EQ(1, hex2bin.limit("47", what) == true);
  EXPECT_EQ(1, what.empty() == true);
  testExtractNoPrintWithStart(hex2bin, "6", SAMPLE1, SAMPLE_TEMP);
}

TEST(Hex2BinTest, ExtractNoPrintSample2)
{
  using enum Files::OpenResult;
  using enum Files::OpenStatus;
  Files files{};
  Hex2Bin hex2bin{&files};
  const auto fileIn = SAMPLE2;
  const auto fileOut = SAMPLE_TEMP;
  std::string what{};

  EXPECT_EQ(1, hex2bin.limit("47", what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, hex2bin.isLimit() == true);

  const auto retIn = files.openInput(fileIn);
  const auto retOut = files.openOutput(fileOut);
  const auto oret = files.isFilesOpen();
  const auto eret = hex2bin.extractNoPrint();
  cleanup(files, fileOut);

  EXPECT_EQ(1, retIn == Success);
  EXPECT_EQ(1, retOut == Success);
  EXPECT_EQ(1, oret == AllOpened);
  EXPECT_EQ(1, eret);
}

TEST(Hex2BinTest, ExtractNoPrintSample3)
{
  Files files{};
  Hex2Bin hex2bin{&files};
  testExtractNoPrintWithStart(hex2bin, "1", SAMPLE3, SAMPLE_TEMP);
}

/* -------- */
/* IntelHex */
/* -------- */
TEST(IntelHexTest, OpenInput)
{
  using enum Files::OpenResult;
  Files files;
  const auto file = SAMPLE1_HEX;
  const auto ret = files.openInput(file);
  EXPECT_EQ(1, ret == Success);
}

TEST(IntelHexTest, OpenOutput)
{
  using enum Files::OpenResult;
  using enum Files::OpenStatus;
  Files files;
  const auto file = SAMPLE_BIN_TEMP;
  const auto ret = files.openOutput(file);
  const auto oretInput = files.isFilesOpen();
  cleanup(files, file);
  EXPECT_EQ(1, ret == Success);
  EXPECT_EQ(1, oretInput == InputClosed);
}

TEST(IntelHexTest, OpenFiles)
{
  testOpenFiles(SAMPLE1, SAMPLE_BIN_TEMP);
}

TEST(IntelHexTest, AddressOffset)
{
  IntelHex intelhex{nullptr};
  std::string what{};
  EXPECT_EQ(1, intelhex.offset("0x8000000", what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, intelhex.offset() == 0x8000000);
}

TEST(IntelHexTest, Width)
{
  IntelHex intelhex{nullptr};
  std::string what{};
  EXPECT_EQ(1, intelhex.width("0x10", what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, intelhex.width() == 0x10);
}

TEST(IntelHexTest, Linear)
{
  IntelHex intelhex{nullptr};
  std::string what{};
  EXPECT_EQ(1, intelhex.linear("0x80002C5", what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, intelhex.linear() == 0x80002C5);
}

TEST(IntelHexTest, Padding)
{
  IntelHex intelhex{nullptr};
  std::string what{};
  EXPECT_EQ(1, intelhex.padding("0x22", what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, intelhex.padding() == 0x22);
}

TEST(IntelHexTest, PaddingWidth)
{
  IntelHex intelhex{nullptr};
  std::string what{};
  EXPECT_EQ(1, intelhex.paddingWidth("10", what) == true);
  EXPECT_EQ(1, what.empty() == true);
  EXPECT_EQ(1, intelhex.paddingWidth() == 10);
}

TEST(IntelHexTest, ParseLine)
{
  auto input = ":1000000000200020C5020008B9020008BB02000859";
  IntelHex::Line line{};
  auto b = IntelHex::parseLine(input, line);
  EXPECT_EQ(1, b);
  EXPECT_EQ(1, line.length == 0x10);
  EXPECT_EQ(1, line.address == 0);
  EXPECT_EQ(1, line.type == IntelHex::RecordType::Data);
  EXPECT_EQ(1, line.data.size() == 16);
  EXPECT_EQ(1, line.checksum == 89);
}

TEST(IntelHexTest, ConvertLine)
{
  IntelHex::Line line{};
  line.address = 0x03AC;
  line.type = IntelHex::RecordType::Data;
  line.data.emplace_back(0x00);
  line.data.emplace_back(0x24);
  line.data.emplace_back(0xF4);
  line.data.emplace_back(0x00);
  line.length = static_cast<std::uint8_t>(line.data.size());
  line.checksum = IntelHex::evalCRC(line);
  auto convertData = IntelHex::convertLine(line);
  line = {};
  line.type = IntelHex::RecordType::EndOfFile;
  line.checksum = IntelHex::evalCRC(line);
  auto convertEoF = IntelHex::convertLine(line);
  EXPECT_EQ(1, convertData == ":0403AC000024F40035\n");
  EXPECT_EQ(1, convertEoF == ":00000001FF\n");
}

static auto testIntelToBin(IntelHex& intelhex, std::string_view fileIn, std::string_view fileOut) -> void
{
  using enum Files::OpenResult;
  using enum Files::OpenStatus;
  const auto& files = intelhex.files();
  const auto retIn = files->openInput(fileIn);
  const auto retOut = files->openOutput(fileOut);
  const auto oret = files->isFilesOpen();
  auto eret = intelhex.intel2bin(false);
  cleanup(*files, fileOut);
  EXPECT_EQ(1, retIn == Success);
  EXPECT_EQ(1, retOut == Success);
  EXPECT_EQ(1, oret == AllOpened);
  EXPECT_EQ(1, eret);
}

TEST(IntelHexTest, IntelToBin1)
{
  Files files{};
  IntelHex intelhex{&files};
  testIntelToBin(intelhex, SAMPLE1_HEX, SAMPLE_BIN_TEMP);
}

TEST(IntelHexTest, IntelToBin2)
{
  Files files{};
  IntelHex intelhex{&files};
  testIntelToBin(intelhex, SAMPLE2_HEX, SAMPLE_BIN_TEMP);
}

static auto testBinToIntel(IntelHex& intelhex, std::string_view fileIn, std::string_view fileOut) -> void
{
  using enum Files::OpenResult;
  using enum Files::OpenStatus;
  const auto& files = intelhex.files();
  const auto retIn = files->openInput(fileIn);
  const auto retOut = files->openOutput(fileOut);
  const auto oret = files->isFilesOpen();
  auto eret = intelhex.bin2intel(false);
  cleanup(*files, fileOut);
  EXPECT_EQ(1, retIn == Success);
  EXPECT_EQ(1, retOut == Success);
  EXPECT_EQ(1, oret == AllOpened);
  EXPECT_EQ(1, eret);
}

TEST(IntelHexTest, BinToIntel1)
{
  Files files{};
  IntelHex intelhex{&files};
  testBinToIntel(intelhex, SAMPLE1_BIN, SAMPLE_HEX_TEMP);
}
TEST(IntelHexTest, BinToIntel2)
{
  Files files{};
  IntelHex intelhex{&files};
  std::string what{};
  intelhex.offset("0x08000000", what);
  EXPECT_EQ(1, what.empty() == true);
  intelhex.linear("0x080002c5", what);
  EXPECT_EQ(1, what.empty() == true);
  intelhex.paddingWidth("0x20", what);
  EXPECT_EQ(1, what.empty() == true);
  testBinToIntel(intelhex, SAMPLE2_BIN, SAMPLE_HEX_TEMP);
}

/* Public function ----------------------------------------------------------*/
auto main(int argc, char** argv) -> int
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
