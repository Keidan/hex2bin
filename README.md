# hex2bin
[![Linux CI](https://github.com/Keidan/hex2bin/actions/workflows/linux.yml/badge.svg)][linuxCI]
[![Windows CI](https://github.com/Keidan/hex2bin/actions/workflows/windows.yml/badge.svg)][windowsCI]
[![Release](https://img.shields.io/github/v/release/Keidan/hex2bin.svg?logo=github)][releases]
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)][license]
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=bugs)][sonarcloud]
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=code_smells)][sonarcloud]
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=duplicated_lines_density)][sonarcloud]
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=vulnerabilities)][sonarcloud]
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=sqale_rating)][sonarcloud]
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=reliability_rating)][sonarcloud]
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=security_rating)][sonarcloud]
[![Technical Debt](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=sqale_index)][sonarcloud]
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=ncloc)][sonarcloud]
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=coverage)][sonarcloud]
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=Keidan_hex2bin&metric=alert_status)][sonarcloud]


(GPL) Hex to binary converter.

This software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.

## Instructions

### MS Windows
On MS Windows, you'll need to install MS Visual Studio build tools.

To do this, you can use the following commands (open powershell as administrator):

Installation of Chocolatey:

	Set-ExecutionPolicy Bypass -Scope Process -Force; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

Installation of Python, Ninja and MS Visual Studio build tools :

	choco install python3 ninja visualstudio2022buildtools

### GNU Linux
On GNU Linux, you'll need to install CMake and Ninja.

	sudo apt-get install cmake ninja-build

### Download

	mkdir devel
	cd devel
	git clone https://github.com/Keidan/hex2bin.git
	cd hex2bin

### Build 

#### MS Windows

	cmake -S . -B build -DCMAKE_BUILD_TYPE=release -G"Visual Studio 17 2022"
	cmake --build build
	
#### GNU Linux

	cmake -S . -B build -DCMAKE_BUILD_TYPE=release -G"Ninja"
	cmake --build build

### VSCode
An MS VSCode workspace file can be found at the following location .vscode/hex2bin.code-workspace

### CMake options
	* Supported distrib.: -DDISTRIBUTION=[debug|release]
	* Supported distrib.: -DCMAKE_BUILD_TYPE=[debug|release]
	* Default start value: cmake -DDEFSTART=[int value] (see the '-s, --start' option of the binary)
	* Default limit value: cmake -DDEFLIMIT=[int value] (see the '-l, --limit' option of the binary)

_You can also use cmake-gui to manage the options._
		
## Example (with cmake -DDEFSTART=0 -DDEFLIMIT=0)
_For Windows, remember to add .exe after the binary name._

<ins>Test 1:</ins>

`hex2bin -i sample1.txt -o sample1.txt.new -s 6 -l 47`

`hex2bin -i sample1.txt.new -o sample1.txt.bin -p`

<ins>Test 2:</ins>

`hex2bin -i sample2.txt -o sample2.txt.new -l 47`

`hex2bin -i sample2.txt.new -o sample2.txt.bin -p`

<ins>Test 3:</ins>

`hex2bin -i sample3.txt -o sample3.txt.new -s 1`


## License

[GNU GPL v3 or later](https://github.com/Keidan/hex2bin/blob/master/license.txt)

[linuxCI]: https://github.com/Keidan/hex2bin/actions?query=workflow%3ALinux
[windowsCI]: https://github.com/Keidan/hex2bin/actions?query=workflow%3AWindows
[sonarcloud]: https://sonarcloud.io/summary/new_code?id=Keidan_hex2bin
[releases]: https://github.com/Keidan/hex2bin/releases
[license]: https://github.com/Keidan/hex2bin/blob/master/license.txt
