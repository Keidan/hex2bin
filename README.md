# hex2bin
[![Build Status](https://github.com/Keidan/hex2bin/actions/workflows/build.yml/badge.svg)][build]
[![CodeFactor](https://www.codefactor.io/repository/github/keidan/hex2bin/badge)][codefactor]
[![Release](https://img.shields.io/github/v/release/Keidan/hex2bin.svg?logo=github)][releases]
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)][license]

(GPL) Hex to binary converter.

This software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.

## Instructions


download the software :

	mkdir devel
	cd devel
	git clone https://github.com/Keidan/hex2bin.git
	cd hex2bin
	cmake -DDISTRIBUTION=[debug|release] .
	make
	./hex2bin -h 
        usage: hex2bin [options]
                --help, -h: Print this help.
                --input, -i: The input file to use (containing the hexadecimal characters).
                --output, -o: The output file to use.
                --limit, -l: Limit of characters per line (the value of the "start" option is not included; default value: default value: [see cmake -DDEFLIMIT]).
                --start, -s: Adds a start offset per line (default value: [see cmake -DDEFSTART]).
                --printable, -p: Extract and convert all printable characters.
                --extract_only, -e: Extract only the words from "start" to "limit".

	Supported cmake options (optional):
		* Supported distrib.: -DDISTRIBUTION=[debug|release]
		* Default start value: cmake -DDEFSTART=[int value] (see the '-s, --start' option of the binary)
		* Default limit value: cmake -DDEFLIMIT=[int value] (see the '-l, --limit' option of the binary)
		
_You can also use cmake-gui to manage the options._
		
## Example (with cmake -DDEFSTART=0 -DDEFLIMIT=0)

<ins>Test 1:</ins>

`./hex2bin -i sample1.txt -o sample1.txt.new -s 6 -l 47`

`./hex2bin -i sample1.txt.new -o sample1.txt.bin -p`

<ins>Test 2:</ins>

`./hex2bin -i sample2.txt -o sample2.txt.new -l 47`

`./hex2bin -i sample2.txt.new -o sample2.txt.bin -p`

<ins>Test 3:</ins>

`./hex2bin -i sample3.txt -o sample3.txt.new -s 1`


## License

[GPLv3](https://github.com/Keidan/hex2bin/blob/master/license.txt)

[build]: https://github.com/Keidan/hex2bin/actions
[codefactor]: https://www.codefactor.io/repository/github/keidan/hex2bin
[releases]: https://github.com/Keidan/hex2bin/releases
[license]: https://github.com/Keidan/hex2bin/blob/master/license.txt
