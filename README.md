# hex2bin
[![Build Status](https://img.shields.io/travis/Keidan/hex2bin/master.svg?style=plastic)](https://travis-ci.org/Keidan/hex2bin)
[![GitHub license](https://img.shields.io/github/license/Keidan/hex2bin.svg?style=plastic)](https://github.com/Keidan/hex2bin/blob/master/LICENSE)

(GPL) Hex to binary converter.

This software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.

## Instructions


download the software :

	mkdir devel
	cd devel
	git clone git://github.com/Keidan/hex2bin.git
	cd hex2bin
	make
	./hex2bin -h 
        usage: hex2bin [options]
                --help, -h: Print this help.
                --input, -i: The input file to use (containing the hexadecimal characters).
                --output, -o: The output file to use.
                --limit, -l: Character limit per line (the value of the "start" option is not included).
                --start, -s: Adding a start offset per line.
                --printable, -p: Extracts and converts all printable characters.
                --extract_only, -e: Only extracts words from "start" to "limit".

## Example

`./hex2bin -i sample1.txt -o sample1.txt.new -s 6 -l 47`

`./hex2bin -i sample2.txt -o sample2.txt.new -l 47`


## License

[GPLv3](https://github.com/Keidan/hex2bin/blob/master/LICENSE)
