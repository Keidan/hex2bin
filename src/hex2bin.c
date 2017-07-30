/*
 * @file hex2bin.c
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>

static const struct option long_options[] = { 
  { "help"         , 0, NULL, 'h' },
  { "input"        , 1, NULL, 'i' },
  { "output"       , 1, NULL, 'o' },
  { "limit"        , 1, NULL, 'l' },
  { "printable"    , 0, NULL, 'p' },
  { "extract_only" , 0, NULL, 'e' },
  { NULL           , 0, NULL,  0  },
};


static void usage(int xcode);
static void hex2bin_extract_only(FILE *output, FILE *input, int limit);
static void hex2bin_extract_no_print(FILE *output, FILE *input, int limit);
static int hex2bin_extract_print(FILE *output, FILE *input, int limit);


int main(int argc, char** argv) {
  int opt, limit = 0;
  FILE *input = NULL, *output = NULL;
  _Bool printable = 0, extract_only = 0;
  /* parse the options */
  while ((opt = getopt_long(argc, argv, "hi:o:l:pe", long_options, NULL)) != -1) {
    switch (opt) {
      case 'h': /* help */
        usage(EXIT_SUCCESS);
        break;
      case 'i': /* input */
	input = fopen(optarg, "rb");
	if(input == NULL) {
	  fprintf(stderr, "Unable to open the file '%s': (%d) %s", optarg, errno, strerror(errno));
	  usage(EXIT_FAILURE);
	}
	break;
      case 'o': /* output */
	output = fopen(optarg, "w+");
	if(output == NULL) {
	  fprintf(stderr, "Unable to open the file '%s': (%d) %s", optarg, errno, strerror(errno));
	  usage(EXIT_FAILURE);
	}
	break;
      case 'l': /* limit */
	if(optarg !=  NULL) limit = atoi(optarg);
	if(limit < 0) limit = 0;
	break;
      case 'p': /* printable */
	printable = 1;
	break;
      case 'e': /* extract_only */
	extract_only = 1;
	break;
      default: /* '?' */
        fprintf(stderr, "Unknown option '%c'.\n", (char)opt);
        usage(EXIT_FAILURE);
    }
  }

  if(input == NULL || output == NULL) {
    if(input == NULL && output == NULL) fprintf(stderr, "Invalid input and output values\n");
    else if(input != NULL) fprintf(stderr, "Invalid input value\n");
    else fprintf(stderr, "Invalid output value\n");
    usage(EXIT_FAILURE);
  }
  int ret = EXIT_SUCCESS;
  if(extract_only) {
    hex2bin_extract_only(output, input, limit);
  } else {
    if(!printable) {
      hex2bin_extract_no_print(output, input, limit);
    } else {
      if(hex2bin_extract_print(output, input, limit) == -1) {
        ret = EXIT_FAILURE;
        fprintf(stderr, "Unable to allocate a memory for the input buffer\n");
      }
    }
  }

  fflush(output);
  /* close the fd's */
  fclose(input);
  fclose(output);
  
  return ret;
}


/**
 * @brief usage function.
 * @param xcode The exit code.
 */
static void usage(int xcode) {
  printf("usage: hex2bin [options]\n");
  printf("\t--help, -h: Print this help\n");
  printf("\t--input, -i: The input file to use (containing the hex chars).\n");
  printf("\t--output, -o: The output file to use.\n");
  printf("\t--limit, -l: Limit chars per lines.\n");
  printf("\t--printable, -p: Extracts and convert all printables char's.\n");
  printf("\t--extract_only, -e: Extracts only the words to the limit.\n");
  exit(xcode);
}

/**
 * @brief Extracts only the words to the limit.
 * @param output The output file.
 * @param input The input file.
 * @param limit The limit per lines.
 */
static void hex2bin_extract_only(FILE *output, FILE *input, int limit) {
  size_t length;
  char *buffer = NULL;
  int len = 0, lim, alength = limit + 2;
  char array[alength];
  while (getline(&buffer, &length, input) != -1) {
    len = strlen(buffer);
    lim = (limit > len) ? len : limit;
    strncpy(array, buffer, lim);
    if(array[lim - 1] != '\n') strcat(array, "\n");
    fprintf(output, "%s", array);
    fflush(output);
    bzero(array, alength);
  }
  if(buffer) free(buffer);
}

/**
 * @brief Extracts and without converting printables char's
 * @param output The output file.
 * @param input The input file.
 * @param limit The limit per lines.
 */
static void hex2bin_extract_no_print(FILE *output, FILE *input, int limit) {
  size_t length;
  int n;
  char *buffer = NULL, *token, *sep, *copy;
  while (getline(&buffer, &length, input) != -1) {
    token = strdup((char*)buffer);
    copy = token;
    if(limit) n = 1;
    while((sep = strsep(&token, " ")) != NULL) {
      fprintf(output, "%c", (char)strtol(sep, NULL, 16));
      if(limit) {
        if(n++ > limit) break;
      }
    }
    free(copy);
  }
  if(buffer) free(buffer);
}
/**
 * @brief Extracts and convert all printables char's
 * @param output The output file.
 * @param input The input file.
 * @param limit The limit per lines.
 * @return Returns -1 on error, 0 else.
 */

static int hex2bin_extract_print(FILE *output, FILE *input, int limit) {
  char cc[2];
  uint8_t *buf, c;
  size_t length, offset = 0, reads, i;
  /* get the file size */
  fseek(input, 0, SEEK_END);
  length = ftell(input);
  fseek(input, 0, SEEK_SET);
  /* temp buffer */
  buf = malloc(length);
  if(buf == NULL)
    return -1;
  i = 0;
  while(offset < length) {
    reads = fread(&c, 1, 1, input);
    if(!reads) break;
    offset += reads;
    if(isalnum(c)) buf[i++] = c;
  }
  length = i;
  /* write the datas */
  for(i = 0; i < length; i+=2) {
    memcpy(cc, buf + i, 2);
    fprintf(output, "%c", (char)strtol(cc, NULL, 16));
  }
  free(buf);
  return 0;
}
