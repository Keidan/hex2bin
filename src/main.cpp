/*
 * @file main.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include <iostream>
#include <cstring>
#include <csignal>
#include <getopt.h>
#include <cerrno>
#include "Hex2Bin.hpp"

/* Usings -------------------------------------------------------------------*/
using std::int32_t;
using std::string;

/* Private variables --------------------------------------------------------*/
static const struct option long_options[] = { 
  { "help"         , 0, NULL, 'h' },
  { "input"        , 1, NULL, 'i' },
  { "output"       , 1, NULL, 'o' },
  { "limit"        , 1, NULL, 'l' },
  { "start"        , 1, NULL, 's' },
  { "printable"    , 0, NULL, 'p' },
  { "extract_only" , 0, NULL, 'e' },
  { NULL           , 0, NULL,  0  },
};
static Hex2Bin *hex2bin = nullptr;

/* Static forward -----------------------------------------------------------*/
static auto usage(int32_t xcode) -> void;
static auto signal_hook(int s) -> void;
static auto shutdown_hook() -> void;


/* Public function ----------------------------------------------------------*/
auto main(int argc, char** argv) -> int
{
  auto opt = -1;
  struct sigaction sa;
  auto printable = false, extractOnly = false;
  Hex2BinOpenResult openResult;
  string what;
  auto defaultValue = true;
  
  
  std::memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = &signal_hook;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  atexit(shutdown_hook);
  
  hex2bin = new Hex2Bin();
  
  /* parse the options */
  while ((opt = getopt_long(argc, argv, "hi:o:s:l:pe", long_options, NULL)) != -1)
  {
    switch (opt)
    {
      case 'h': /* help */
        usage(EXIT_SUCCESS);
        break;
      case 'i': /* input */
	openResult = hex2bin->openInput(optarg);
	if(openResult == Hex2BinOpenResult::ERROR)
	{
	  std::cerr << "Unable to open the file '" << optarg << "': (" << errno << ") " << strerror(errno) << std::endl;
	  usage(EXIT_FAILURE);
	}
	else if(openResult == Hex2BinOpenResult::ALREADY)
	{
	  std::cerr << "Option 'input' already called." << std::endl;
	}
        break;
      case 'o': /* output */
	openResult = hex2bin->openOutput(optarg);
	if(openResult == Hex2BinOpenResult::ERROR)
	{
	  std::cerr << "Unable to open the file '" << optarg << "': (" << errno << ") " << strerror(errno) << std::endl;
	  usage(EXIT_FAILURE);
	}
	else if(openResult == Hex2BinOpenResult::ALREADY)
	{
	  std::cerr << "Option 'output' already called." << std::endl;
	}
        break;
      case 's': /* start */
	if(!hex2bin->setStart(optarg, what))
	{
	  std::cerr << "Invalid start value: " << what << std::endl;
	  usage(EXIT_FAILURE);
	}
	defaultValue = false;
        break;
      case 'l': /* limit */
	if(!hex2bin->setLimit(optarg, what))
	{
	  std::cerr << "Invalid limit value: " << what << std::endl;
	  usage(EXIT_FAILURE);
	}
	defaultValue = false;
        break;
      case 'p': /* printable */
        printable = true;
        break;
      case 'e': /* extract_only */
        extractOnly = true;
        break;
      default: /* '?' */
        std::cerr << "Unknown option '" << (char)opt << "'." << std::endl;
        usage(EXIT_FAILURE);
    }
  }
  auto isOpen = hex2bin->isFilesOpen();
  if(isOpen != Hex2BinIsOpen::SUCCESS)
  {
    if(isOpen == Hex2BinIsOpen::BOTH)
    {
      std::cerr << "Invalid input and output values" << std::endl;
    }
    else if(isOpen == Hex2BinIsOpen::INPUT)
    {
      std::cerr << "Invalid input value" << std::endl;
    }
    else if(isOpen == Hex2BinIsOpen::OUTPUT)
    {
      std::cerr << "Invalid output value" << std::endl;
    }
    usage(EXIT_FAILURE);
  }
  auto ret = EXIT_SUCCESS;
  if(extractOnly)
  {
    hex2bin->extractOnly();
  }
  else
  {
    if(!printable)
    {
      if(!hex2bin->extractNoPrint())
      {
        ret = EXIT_FAILURE;
      }
    }
    else
    {
      if(!defaultValue && (hex2bin->isValidStart() || hex2bin->isValidLimit()))
      {	
        std::cout << "The start and limit options are not managed in this mode." << std::endl;
      }
      if(!hex2bin->extractPrint())
      {
        ret = EXIT_FAILURE;
      }
    }
  }
  
  /* The files are closed in the exit functions */
  return ret;
}

/* Static functions ---------------------------------------------------------*/
/**
 * @brief Hook function used to capture signals.
 * @param[in] s Signal.
 */
auto signal_hook(int s) -> void { 
  exit(s);
}

/**
 * @brief Hook function called by atexit.
 */
auto shutdown_hook() -> void {
  if(hex2bin != nullptr)
  {
    delete hex2bin, hex2bin = nullptr;
  }
}

/**
 * @brief usage function.
 * @param[in] xcode The exit code.
 */
static auto usage(int32_t xcode) -> void {
  std::cout << "usage: hex2bin [options]" << std::endl;
  std::cout << "\t--help, -h: Print this help" << std::endl;
  std::cout << "\t--input, -i: The input file to use (containing the hexadecimal characters)." << std::endl;
  std::cout << "\t--output, -o: The output file to use." << std::endl;
  std::cout << "\t--limit, -l: Character limit per line (the value of the \"start\" option is not included ; default value: " << DEFAULT_LIMIT << ")." << std::endl;
  std::cout << "\t--start, -s: Adding a start offset per line (default value: " << DEFAULT_START << ")." << std::endl;
  std::cout << "\t--printable, -p: Extracts and converts all printable characters." << std::endl;
  std::cout << "\t--extract_only, -e: Only extracts words from \"start\" to \"limit\"." << std::endl;
  exit(xcode);
}

