src_dir		= ./src
obj_dir		= ./obj
bin		= hex2bin

CC         	= gcc
CFLAG      	= -Wall -Werror -O3
#CFLAG      	= -g -O0

all: $(bin)
clean:
	@rm -rf $(obj_dir) *~ $(bin)

$(bin): $(obj_dir)/$(bin).o
	$(CC) $(CFLAG) $^ -o $@

$(obj_dir)/%.o:$(src_dir)/%.c
	@mkdir -p $(obj_dir)
	@echo 'Building target: $@'
	$(CC) $(CFLAG) -c $< -o $@
