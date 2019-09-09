src_dir		= ./src
obj_dir		= ./obj
bin		= hex2bin

#CC         	= gcc
CXX         	= g++
CXFLAG      	= -std=c++11 -Wall -Werror -O3

all: $(bin)
clean:
	@rm -rf $(obj_dir) *~ $(bin)

$(bin): $(obj_dir)/$(bin).o
	$(CXX) $(CXXFLAG) $^ -o $@

$(obj_dir)/%.o:$(src_dir)/%.cpp
	@mkdir -p $(obj_dir)
	@echo 'Building target: $@'
	$(CXX) $(CXXFLAG) -c $< -o $@

test: all
	@bash test
