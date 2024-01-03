/*
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

/* Defines-------------------------------------------------------------------*/
#ifndef WIN32
#  define NO_RETURN __attribute__((noreturn))
#else
#  define NO_RETURN
#endif /* WIN32 */

/* Usings--------------------------------------------------------------------*/
using h2b::Hex2BinIsOpen;
using h2b::Hex2BinOpenResult;

/* Private structures--------------------------------------------------------*/
struct Context
{
    bool printable = false;
    bool extractOnly = false;
    bool defaultValue = true;
};

/* Private variables --------------------------------------------------------*/
static const std::vector<struct option> g_longOptions = {
  {        "help", 0, nullptr, 'h'},
  {     "version", 0, nullptr, 'v'},
  {       "input", 1, nullptr, 'i'},
  {      "output", 1, nullptr, 'o'},
  {       "limit", 1, nullptr, 'l'},
  {       "start", 1, nullptr, 's'},
  {   "printable", 0, nullptr, 'p'},
  {"extract_only", 0, nullptr, 'e'},
  {       nullptr, 0, nullptr,   0},
};
static const std::unique_ptr<h2b::Hex2Bin> hex2bin = std::make_unique<h2b::Hex2Bin>();

/* Static forward -----------------------------------------------------------*/
static auto usage(int32_t xcode) -> void;
static auto version() -> void;
static auto signalHook(int s) -> void;
static auto shutdownHook() -> void;
static auto processMain(const Context& context) -> int;
static auto decodeArgStartOrLimit(std::string_view optionArg, bool isLimit) -> void;
static auto decodeArgInputOrOutput(std::string_view optionArg, bool isInput) -> void;
static auto processArguments(int argc, char** argv, Context& context) -> void;

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

  processArguments(argc, argv, context);

  /* The files are closed in the exit functions */
  return processMain(context);
}

/* Static functions ---------------------------------------------------------*/
/**
 * @brief Hook function used to capture signals.
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
  hex2bin->close();
}
/**
 * @brief usage function.
 * @param[in] xcode The exit code.
 */
static NO_RETURN void usage(const int32_t xcode)
{
  version();
  std::cout << "usage: " << APP_NAME << " [options]" << std::endl;
  std::cout << "\t--help, -h: Print this help" << std::endl;
  std::cout << "\t--input, -i: The input file to use (containing the hexadecimal characters)." << std::endl;
  std::cout << "\t--output, -o: The output file to use." << std::endl;
  std::cout << "\t--limit, -l: Limit of characters per line (the value of the \"start\" option is not included; default value: " << DEFAULT_LIMIT << ")."
            << std::endl;
  std::cout << "\t--start, -s: Adds a start offset per line (default value: " << DEFAULT_START << ")." << std::endl;
  std::cout << "\t--printable, -p: Extract and convert all printable characters." << std::endl;
  std::cout << "\t--extract_only, -e: Extract only the words from \"start\" to \"limit\"." << std::endl;
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
 * @brief Processes the application processing of the main function.
 * @note This function can call the "exit" method via the "usage" method call.
 * @param[in] context The context of the application which is filled from the arguments of the application.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static auto processMain(const Context& context) -> int
{
  using enum Hex2BinIsOpen;
  if(const auto isOpen = hex2bin->isFilesOpen(); Success != isOpen)
  {
    if(Both == isOpen)
    {
      std::cerr << "Invalid input and output values" << std::endl;
    }
    else if(Input == isOpen)
    {
      std::cerr << "Invalid input value" << std::endl;
    }
    else if(Output == isOpen)
    {
      std::cerr << "Invalid output value" << std::endl;
    }
    usage(EXIT_FAILURE);
  }
  auto ret = EXIT_SUCCESS;
  if(context.extractOnly)
  {
    hex2bin->extractOnly();
  }
  else
  {
    if(!context.printable)
    {
      if(!hex2bin->extractNoPrint())
      {
        ret = EXIT_FAILURE;
      }
    }
    else
    {
      if(!context.defaultValue && (hex2bin->isValidStart() || hex2bin->isValidLimit()))
      {
        std::cout << "The start and limit options are not managed in this mode." << std::endl;
      }
      if(!hex2bin->extractPrint())
      {
        ret = EXIT_FAILURE;
      }
    }
  }
  return ret;
}

/* Arguments functions ------------------------------------------------------*/
/**
 * @brief Decodes the "start" or "limit" argument.
 * @param[in] optionArg Value of the argument.
 * @param[in] isLimit Argument "limit" or "start" ?
 */
static auto decodeArgStartOrLimit(std::string_view optionArg, const bool isLimit) -> void
{
  std::string what{};
  const auto ret = isLimit ? hex2bin->setLimit(optionArg, what) : hex2bin->setStart(optionArg, what);
  if(!ret)
  {
    std::cerr << "Invalid " << (isLimit ? "limit" : "start") << " value: " << what << std::endl;
    usage(EXIT_FAILURE);
  }
}

/**
 * @brief Decodes the "input" or "output" argument.
 * @note This function can call the "exit" method via the "usage" method call.
 * @param[in] optionArg Value of the argument.
 * @param[in] isInput Argument "input" or "output" ?
 */
static auto decodeArgInputOrOutput(std::string_view optionArg, const bool isInput) -> void
{
  const auto openResult = isInput ? hex2bin->openInput(optionArg) : hex2bin->openOutput(optionArg);

  if(Hex2BinOpenResult::Error == openResult)
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
  else if(Hex2BinOpenResult::Already == openResult)
  {
    std::cerr << "Option '" << (isInput ? "input" : "output") << "' already called." << std::endl;
  }
}

/**
 * @brief Processes the arguments passed as parameters to the application.
 * @note This function can call the "exit" method via the "usage" method call.
 * @param[in] argc The number of arguments.
 * @param[in] argv The list of argumentsArguments count.
 * @param[out] context The context of the application that will be filled from the arguments of the application.
 */
static auto processArguments(const int argc, char** argv, Context& context) -> void
{
  auto opt = -1;
  /* parse the options */
  const auto* longOptions = g_longOptions.data();
  while(-1 != (opt = getopt_long(argc, argv, "vhi:o:s:l:pe", longOptions, nullptr)))
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
        decodeArgStartOrLimit(optarg, false);
        context.defaultValue = false;
        break;
      case 'l': /* limit */
        decodeArgStartOrLimit(optarg, true);
        context.defaultValue = false;
        break;
      case 'p': /* printable */
        context.printable = true;
        break;
      case 'e': /* extract_only */
        context.extractOnly = true;
        break;
      default: /* '?' */
        std::cerr << "Unknown option '" << static_cast<char>(opt) << "'." << std::endl;
        usage(EXIT_FAILURE);
    }
  }
}
