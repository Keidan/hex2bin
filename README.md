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
	./hex2bin 
		usage: hex2bin [options]
			--help, -h: Print this help
			--input, -i: The input file to use (containing the hex chars).
			--output, -o: The output file to use.
			--limit, -l: Limit chars per lines.
  			--printable, -p: Force and convert all printables char's.

## License

[GPLv3](https://github.com/Keidan/hex2bin/blob/master/LICENSE)
