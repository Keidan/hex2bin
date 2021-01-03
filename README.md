# hex2bin
[![Build Status](https://travis-ci.com/Keidan/hex2bin.svg?branch=master)](https://travis-ci.com/Keidan/hex2bin)

(GPL) Hex to binary converter.

This software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.

## Instructions


download the software :

	mkdir devel
	cd devel
	git clone git://github.com/Keidan/hex2bin.git
	cd hex2bin
	cmake -DDISTRIBUTION=[debug|release] .
	make
	./hex2bin -h 
        usage: hex2bin [options]
                --help, -h: Print this help.
                --input, -i: The input file to use (containing the hexadecimal characters).
                --output, -o: The output file to use.
                --limit, -l: Character limit per line (the value of the "start" option is not included ; default value: [see cmake -DDEFSTART]).
                --start, -s: Adding a start offset per line (default value: [see cmake -DDEFLIMIT].
                --printable, -p: Extracts and converts all printable characters.
                --extract_only, -e: Only extracts words from "start" to "limit".

	Supported cmake options (optional):
		* Supported distrib.: -DDISTRIBUTION=[debug|release]
		* Default start value: cmake -DDEFSTART=[int value] (see the '-s, --start' option of the binary)
		* Default limit value: cmake -DDEFLIMIT=[int value] (see the '-l, --limit' option of the binary)
		
## Example (with cmake -DDEFSTART=0 -DDEFLIMIT=0)

<u>Test 1:</u>

`./hex2bin -i sample1.txt -o sample1.txt.new -s 6 -l 47`

`./hex2bin -i sample1.txt.new -o sample1.txt.bin -p`

<u>Test 2:</u>

`./hex2bin -i sample2.txt -o sample2.txt.new -l 47`

`./hex2bin -i sample2.txt.new -o sample2.txt.bin -p`

<u>Test 3:</u>

`./hex2bin -i sample3.txt -o sample3.txt.new -s 1`


## License

[GPLv3](https://github.com/Keidan/hex2bin/blob/master/LICENSE)
