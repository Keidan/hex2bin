/**
 * @file main.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include <iostream>
#include <cstring>
#include <csignal>
#ifndef WIN32
#  include <getopt.h>
#else
#  include <Windows.h>
#  include "win32/getopt.h"
#endif
#include <cerrno>
#include <functional>
#include <memory>
#include "Hex2Bin.hpp"
#include "IntelHex.hpp"

/* Defines-------------------------------------------------------------------*/
#ifndef WIN32
#  define NO_RETURN __attribute__((noreturn))
#else
#  define NO_RETURN
#endif /* WIN32 */

/* Usings--------------------------------------------------------------------*/
using h2b::Files;
using h2b::IntelHex;

/* Private structures--------------------------------------------------------*/
enum class Direction : std::uint8_t
{
  Hex2Bin = 0,
  Intel2Bin,
  Bin2Intel
};
struct Context
{
    bool printable = false;
    bool extractOnly = false;
    bool defaultValue = true;
    Direction direction = Direction::Hex2Bin;
    std::uint32_t addrOffset = 0U;
};
enum class ArgInt
{
  Start = 0,
  Limit,
  Offset,
  Width,
  Linear,
  Padding,
  PaddingWidth
};

/* Private variables --------------------------------------------------------*/
static const std::vector<struct option> g_longOptions = {
  {         "help", 0, nullptr, 'h'},
  {      "version", 0, nullptr, 'v'},
  {        "input", 1, nullptr, 'i'},
  {       "output", 1, nullptr, 'o'},
  {        "limit", 1, nullptr, 'l'},
  {        "start", 1, nullptr, 's'},
  {    "printable", 0, nullptr, 'p'},
  { "extract_only", 0, nullptr, 'e'},
  {         "ihex", 1, nullptr, '0'},
  {       "offset", 1, nullptr, '1'},
  {        "width", 1, nullptr, '2'},
  {       "linear", 1, nullptr, '3'},
  {      "segment", 0, nullptr, '4'},
  {      "padding", 1, nullptr, '5'},
  {"padding_width", 1, nullptr, '6'},
  {        nullptr, 0, nullptr,   0},
};
static h2b::Files g_files{};
static const std::unique_ptr<h2b::Hex2Bin> g_hex2bin = std::make_unique<h2b::Hex2Bin>(&g_files);
static const std::unique_ptr<h2b::IntelHex> g_intelHex = std::make_unique<h2b::IntelHex>(&g_files);

/* Static forward -----------------------------------------------------------*/
static auto usage(int32_t xcode) -> void;
static auto version() -> void;
static auto signalHook(int s) -> void;
static auto shutdownHook() -> void;
static auto handleMain(const Context& context) -> int;
static auto handleHex2Bin(const Context& context) -> int;
static auto handleIntelHex(const Context& context) -> int;
static auto validateFiles() -> void;
static auto decodeArgInputOrOutput(std::string_view optionArg, bool isInput) -> void;
static auto decodeIntelHex(std::string_view optionArg) -> Direction;
static auto decodeArgInt(std::string_view optionArg, ArgInt type) -> void;
static auto handleArguments(int argc, char** argv, Context& context) -> void;

/* Public function ----------------------------------------------------------*/
auto main(int argc, char** argv) -> int
{
  Context context{};
#ifndef WIN32
  struct sigaction sa;

  std::memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = &signalHook;
  sigaction(SIGINT, &sa, nullptr);
  sigaction(SIGTERM, &sa, nullptr);
#else
  signal(SIGINT, signalHook);
  signal(SIGTERM, signalHook);
#endif
  atexit(shutdownHook);

  handleArguments(argc, argv, context);

  /* The files are closed in the exit functions */
  return handleMain(context);
}

/* Static functions ---------------------------------------------------------*/
/**
 * @brief Hook function used to capture signals.
 * 
 * @param[in] s Signal.
 */
static NO_RETURN void signalHook(const int s)
{
  exit((SIGINT == s || SIGTERM == s) ? EXIT_SUCCESS : EXIT_FAILURE);
}

/**
 * @brief Hook function called by atexit.
 */
static auto shutdownHook() -> void
{
  g_files.close();
}
/**
 * @brief usage function.
 * 
 * @param[in] xcode The exit code.
 */
static NO_RETURN void usage(const int32_t xcode)
{
  version();
  std::cout << "usage: " << APP_NAME << " [options]" << std::endl;
  std::cout << "Note: The limit, start, offset, length and width arguments can be expressed in decimal or hexadecimal if they start with 0x." << std::endl;
  std::cout << "Generic arguments:" << std::endl;
  std::cout << "\t--help, -h: Print this help." << std::endl;
  std::cout << "\t--input, -i: The input file to use (containing the hexadecimal characters)." << std::endl;
  std::cout << "\t--output, -o: The output file to use." << std::endl;
  std::cout << "Hex2Bin arguments:" << std::endl;
  std::cout << "\t--limit, -l: Limit of characters per line (the value of the \"start\" option is not included; default value: " << DEFAULT_LIMIT << ")."
            << std::endl;
  std::cout << "\t--start, -s: Adds a start offset per line (default value: " << DEFAULT_START << ")." << std::endl;
  std::cout << "\t--printable, -p: Extract and convert all printable characters." << std::endl;
  std::cout << "\t--extract_only, -e: Extract only the words from \"start\" to \"limit\"." << std::endl;
  std::cout << "Intel hex arguments (minimalistic implementation):" << std::endl;
  std::cout << "\t--ihex: Convert intel hex to binary or vice versa, direction depends on parameter value (\"h2b\" or \"b2h\")." << std::endl;
  std::cout << "\t--offset: Address offset to be used in ihex mode." << std::endl;
  std::cout << "\t--padding: Value to be used to fill in data in the event of address discrepancies (0 <= padding <= 255)." << std::endl;
  std::cout << "\t--width: Number of data items to be placed on a line (default value: " << DEFAULT_WIDTH << ") ; only if ihex = b2h)." << std::endl;
  std::cout << "\t--linear: Value to be used for \"start linear\" (only if ihex = b2h)." << std::endl;
  std::cout << "\t--segment: Use of the \"extended segment\" block instead of \"extended linear\" for the offset parameter (only if ihex = b2h)." << std::endl;
  std::cout << "\t--padding_width: If we find \"padding_width\" consecutive blocks ccorresponding to the value of the \"padding\" argument, the Intel HEX file "
               "will contain address jumps (only if ihex = b2h)."
            << std::endl;
  exit(xcode);
}

/**
 * @brief Print the version
 */
static void version()
{
  std::cout << APP_NAME << " version " << VERSION_MAJOR << "." << VERSION_MINOR << " (";
#if DEBUG
  std::cout << "debug";
#else
  std::cout << "release";
#endif
  std::cout << ")" << std::endl;
}

/**
 * @brief Handles the application's main function.
 * @note This function can call the "exit" method via the "usage" method call.
 * 
 * @param[in] context The application context, which is filled in with the application's arguments.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static auto handleMain(const Context& context) -> int
{
  validateFiles();
  int ret;
  if(Direction::Hex2Bin == context.direction)
    ret = handleHex2Bin(context);
  else
    ret = handleIntelHex(context);
  return ret;
}

/**
 * @brief Handles hex2bin mode
 * @note This function can call the "exit" method via the "usage" method call.
 * 
 * @param[in] context The application context, which is filled in with the application's arguments.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static auto handleHex2Bin(const Context& context) -> int
{
  auto ret = EXIT_SUCCESS;
  if(context.extractOnly)
  {
    g_hex2bin->extractOnly();
  }
  else
  {
    if(!context.printable)
    {
      if(!g_hex2bin->extractNoPrint())
      {
        ret = EXIT_FAILURE;
      }
    }
    else
    {
      if(!context.defaultValue && (g_hex2bin->isStart() || g_hex2bin->isLimit()))
      {
        std::cout << "The start and limit options are not managed in this mode." << std::endl;
      }
      if(!g_hex2bin->extractPrint())
      {
        ret = EXIT_FAILURE;
      }
    }
  }
  return ret;
}

/**
 * @brief Handles intel hex mode
 * @note This function can call the "exit" method via the "usage" method call.
 * 
 * @param[in] context The application context, which is filled in with the application's arguments.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static auto handleIntelHex(const Context& context) -> int
{
  using enum Direction;
  auto ret = EXIT_SUCCESS;
  if(Intel2Bin == context.direction)
  {
    if(!g_intelHex->intel2bin())
    {
      ret = EXIT_FAILURE;
    }
  }
  else if(Bin2Intel == context.direction)
  {
    if(!g_intelHex->bin2intel())
    {
      ret = EXIT_FAILURE;
    }
  }
  return ret;
}

/* Arguments functions ------------------------------------------------------*/
/**
 * @brief Validate the status of files to be used.
 */
static auto validateFiles() -> void
{
  using enum Files::OpenStatus;
  if(const auto isOpen = g_files.isFilesOpen(); AllOpened != isOpen)
  {
    if(AllClosed == isOpen)
    {
      std::cerr << "Invalid input and output values" << std::endl;
    }
    else if(InputClosed == isOpen)
    {
      std::cerr << "Invalid input value" << std::endl;
    }
    else if(OutputClosed == isOpen)
    {
      std::cerr << "Invalid output value" << std::endl;
    }
    usage(EXIT_FAILURE);
  }
}

/**
 * @brief Decodes the "input" or "output" argument.
 * @note This function can call the "exit" method via the "usage" method call.
 * 
 * @param[in] optionArg Value of the argument.
 * @param[in] isInput Argument "input" or "output" ?
 */
static auto decodeArgInputOrOutput(std::string_view optionArg, const bool isInput) -> void
{
  using enum Files::OpenResult;
  const auto openResult = isInput ? g_files.openInput(optionArg) : g_files.openOutput(optionArg);

  if(Error == openResult)
  {
#ifndef WIN32
    const auto error = strerror(errno);
#else
    enum
    {
      ERROR_SIZE = 200
    };
    char error[ERROR_SIZE];
    if(strerror_s(error, ERROR_SIZE, errno))
      throw std::exception("strerror_s failed!");
#endif
    std::cerr << "Unable to open the file '" << optionArg << "': (" << errno << ") " << error << std::endl;
    usage(EXIT_FAILURE);
  }
  else if(Already == openResult)
  {
    std::cerr << "Option '" << (isInput ? "input" : "output") << "' already called." << std::endl;
  }
}

/**
 * @brief Decodes ihex argument.
 * @note This function can call the "exit" method via the "usage" method call.
 * 
 * @param[in] optionArg Value of the argument.
 * @retval IntelHex::Direction
 */
static auto decodeIntelHex(std::string_view optionArg) -> Direction
{
  if("h2b" != optionArg && "b2h" != optionArg)
  {
    std::cerr << R"(Invalid ihex value: Only "h2b" or "b2h" values allowed.)" << std::endl;
    usage(EXIT_FAILURE);
  }
  using enum Direction;
  if("h2b" == optionArg)
    return Intel2Bin;
  return Bin2Intel;
}

/**
 * @brief Decodes int/hex_int argument.
 * 
 * @param[in] optionArg Value of the argument.
 * @param[in] type See ArgInt
 */
static auto decodeArgInt(std::string_view optionArg, ArgInt type) -> void
{
  using enum ArgInt;
  std::string what{};
  bool ret;
  const char* err;
  if(Start == type)
  {
    ret = g_hex2bin->start(optionArg, what);
    err = "start";
  }
  else if(Limit == type)
  {
    ret = g_hex2bin->limit(optionArg, what);
    err = "limit";
  }
  else if(Offset == type)
  {
    ret = g_intelHex->offset(optionArg, what);
    err = "offset";
  }
  else if(Width == type)
  {
    ret = g_intelHex->width(optionArg, what);
    err = "width";
  }
  else if(Linear == type)
  {
    ret = g_intelHex->linear(optionArg, what);
    err = "linear";
  }
  else if(Padding == type)
  {
    ret = g_intelHex->padding(optionArg, what);
    err = "padding";
  }
  else if(PaddingWidth == type)
  {
    ret = g_intelHex->paddingWidth(optionArg, what);
    err = "padding width";
  }
  else
  {
    ret = false;
    err = "unknown";
  }
  if(!ret)
  {
    std::cerr << "Invalid " << err << " value: " << what << std::endl;
    usage(EXIT_FAILURE);
  }
}

/**
 * @brief Handles arguments passed as parameters to the application.
 * @note This function can call the "exit" method via the "usage" method call.
 * 
 * @param[in] argc The number of arguments.
 * @param[in] argv The list of argumentsArguments count.
 * @param[out] context The context of the application that will be filled from the arguments of the application.
 */
static auto handleArguments(const int argc, char** argv, Context& context) -> void
{
  using enum ArgInt;
  auto opt = -1;
  /* parse the options */
  const auto* longOptions = g_longOptions.data();
  while(-1 != (opt = getopt_long(argc, argv, "vhi:o:s:l:pe0:1:2:3:45:6:", longOptions, nullptr)))
  {
    switch(opt)
    {
      case 'h': /* help */
        usage(EXIT_SUCCESS);
      case 'v': /* version */
        version();
        exit(EXIT_SUCCESS);
        break;
      case 'i': /* input */
        decodeArgInputOrOutput(optarg, true);
        break;
      case 'o': /* output */
        decodeArgInputOrOutput(optarg, false);
        break;
      case 's': /* start */
        decodeArgInt(optarg, Start);
        context.defaultValue = false;
        break;
      case 'l': /* limit */
        decodeArgInt(optarg, Limit);
        context.defaultValue = false;
        break;
      case 'p': /* printable */
        context.printable = true;
        break;
      case 'e': /* extract_only */
        context.extractOnly = true;
        break;
      case '0': /* ihex */
        context.direction = decodeIntelHex(optarg);
        break;
      case '1': /* offset */
        decodeArgInt(optarg, Offset);
        break;
      case '2': /* width */
        decodeArgInt(optarg, Width);
        break;
      case '3': /* linear */
        decodeArgInt(optarg, Linear);
        break;
      case '4': /* segment */
        g_intelHex->segment();
        break;
      case '5': /* padding */
        decodeArgInt(optarg, Padding);
        break;
      case '6': /* padding_width */
        decodeArgInt(optarg, PaddingWidth);
        break;
      default: /* '?' */
        std::cerr << "Unknown option '" << static_cast<char>(opt) << "'." << std::endl;
        usage(EXIT_FAILURE);
    }
  }
}
