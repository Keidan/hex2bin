/*
 * @file hex2bin.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include <exception>
#include <regex>
#include <vector>
#include <limits>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <getopt.h>

/* Usings -------------------------------------------------------------------*/
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using std::int32_t;
using std::uint32_t;

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
static ofstream output;
static ifstream input;

/* Static forward -----------------------------------------------------------*/
static auto usage(int32_t xcode) -> void;
static auto signal_hook(int s) -> void;
static auto shutdown_hook() -> void;
static auto split(const string& in, const string &reg) -> vector<string>;
static auto get_fragment(const string &line, uint32_t start, uint32_t limit) -> string;
static auto extract_only(uint32_t start, uint32_t limit) -> void;
static auto extract_no_print(uint32_t start, uint32_t limit) -> void;
static auto extract_print() -> bool;


/* Public function ----------------------------------------------------------*/
auto main(int argc, char** argv) -> int {
  auto opt = -1;
  int32_t limit = 0U, start = 0U;
  struct sigaction sa;
  auto printable = false, extractOnly = false;


  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = &signal_hook;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  atexit(shutdown_hook);

  /* parse the options */
  while ((opt = getopt_long(argc, argv, "hi:o:s:l:pe", long_options, NULL)) != -1) {
    switch (opt) {
      case 'h': /* help */
        usage(EXIT_SUCCESS);
        break;
      case 'i': /* input */
        if(!input.is_open()) {
          input.open(optarg, std::ios::in|std::ios::binary);
          if(!input.is_open() || input.fail()) {
            std::cerr << "Unable to open the file '" << optarg << "': (" << errno << ") " << strerror(errno) << std::endl;
            usage(EXIT_FAILURE);
          }
        } else
            std::cerr << "Option 'output' already called." << std::endl;
        break;
      case 'o': /* output */
        if(!output.is_open()) {
          output.open(optarg, std::ios::out|std::ios::binary);
          if(!output.is_open() || output.fail()) {
            std::cerr << "Unable to open the file '" << optarg << "': (" << errno << ") " << strerror(errno) << std::endl;
            usage(EXIT_FAILURE);
          }
        } else 
            std::cerr << "Option 'output' already called." << std::endl;
        break;
      case 's': /* start */
          try {
            start = std::stoi(optarg);
            if(start < 0) start = 0;
          } catch (const std::exception& e) {
            std::cerr << "Invalid start value: " << e.what() << std::endl;
            usage(EXIT_FAILURE);
          }
        break;
      case 'l': /* limit */
          try {
            limit = std::stoi(optarg);
            if(limit < 0) limit = 0;
          } catch (const std::exception& e) {
            std::cerr << "Invalid limit value: " << e.what() << std::endl;
            usage(EXIT_FAILURE);
          }
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

  if(!input.is_open() || !output.is_open()) {
    if(!input.is_open() && !output.is_open()) std::cerr << "Invalid input and output values" << std::endl;
    else if(!input.is_open()) std::cerr << "Invalid input value" << std::endl;
    else std::cerr << "Invalid output value" << std::endl;
    usage(EXIT_FAILURE);
  }
  auto ret = EXIT_SUCCESS;
  if(extractOnly) {
    extract_only(start, limit);
  } else {
    if(!printable) {
      extract_no_print(start, limit);
    } else {
      if(start || limit)
        std::cout << "The start and limit options are not managed in this mode." << std::endl;
      if(!extract_print()) {
        ret = EXIT_FAILURE;
        std::cerr << "Unable to allocate a memory for the input buffer" << std::endl;
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
  if(input.is_open())
    input.close();
  if(output.is_open())
    output.close();
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
  std::cout << "\t--limit, -l: Character limit per line (the value of the \"start\" option is not included)." << std::endl;
  std::cout << "\t--start, -s: Adding a start offset per line." << std::endl;
  std::cout << "\t--printable, -p: Extracts and converts all printable characters." << std::endl;
  std::cout << "\t--extract_only, -e: Only extracts words from \"start\" to \"limit\"." << std::endl;
  exit(xcode);
}


/**
 * @brief Splits a string according to the specified regex
 * @param[in] in The input string.
 * @param[in] reg The regex.
 * @retval The result in a vector.
 */
static auto split(const string& in, const string &reg) -> vector<string> {
  // passing -1 as the submatch index parameter performs splitting
  std::regex re(reg);
  std::sregex_token_iterator first{in.begin(), in.end(), re, -1}, last;
  return {first, last};
}

/**
 * @brief Returns a fragment of the input line.
 * @param[in] line The input line.
 * @param[in] start Start offset.
 * @param[in] limit Max char.
 * @retval The fragment of the input line.
 */
static auto get_fragment(const string &line, uint32_t start, uint32_t limit) -> string {
  auto len = 0UL, lim = 0UL;
  len = line.size();
  if(len < start)
    len = 0UL;
  lim = (limit > len) ? len : limit;
  return line.substr(start, lim);
}

/**
 * @brief Only extracts words from "start" to "limit".
 * @param[in] output The output file.
 * @param[in] input The input file.
 * @param[in] start The start offset.
 * @param[in] limit The limit per lines.
 */
static auto extract_only(uint32_t start, uint32_t limit) -> void {
  string line;

  while (std::getline(input, line)) {
    auto fragment = get_fragment(line, start, limit);
    if(!fragment.empty() && fragment.at(fragment.size() - 1) != '\n') fragment += "\n";
    output << fragment;
  }
  output.flush();
}

/**
 * @brief Extracts and without converting all printable characters.
 * @param[in] output The output file.
 * @param[in] input The input file.
 * @param[in] start The start offset.
 * @param[in] limit The limit per lines.
 */
static auto extract_no_print(uint32_t start, uint32_t limit) -> void {
  string line;
  while (std::getline(input, line)) {
    auto fragment = get_fragment(line, start, limit);
    auto tokens = split(fragment, "\\s+");
    for(vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
      output << (char)std::stol((*it), NULL, 16);
    }
  }
  output.flush();
}

/**
 * @brief Extracts and converts all printable characters.
 * @param[in] output The output file.
 * @param[in] input The input file.
 * @retval Returns false on error, true else.
 */
static auto extract_print() -> bool {
  char cc[2];
  uint8_t *buf, c;
  auto offset = 0UL;
  /* get the file size */

  input.ignore(std::numeric_limits<std::streamsize>::max());
  auto length = input.gcount();
  input.clear();   //  Since ignore will have set eof.
  input.seekg(0, std::ios_base::beg);

  /* temp buffer */
  buf = new uint8_t[length];
  if(buf == NULL)
    return false;
  auto i = 0UL;
  while(offset < length) {
    input.read((char*)&c, 1);
    if(!input) break;
    offset++;
    if(std::isalnum(c)) buf[i++] = c;
  }
  length = i;
  /* write the datas */
  for(i = 0; i < length; i+=2) {
    memcpy(cc, buf + i, 2);
    output << (char)std::stol(cc, NULL, 16);
  }
  output.flush();
  delete [] buf;
  return true;
}
