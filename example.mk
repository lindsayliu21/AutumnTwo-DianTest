CC = gcc
CFLAGS = -Wall -O2

# 定义所有源文件
SRCS = variable.c dependency_graph.c make_parser.c make_build.c minimake.c

# 定义目标文件
OBJS = $(SRCS:.c=.o)

# 最终可执行文件
TARGET = minimake

# 默认目标
all: $(TARGET)

# 链接目标
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


# 编译规则
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# 清理目标(删除所有中间文件)
clean:
	rm -f $(OBJS) $(TARGET)

# 重新构建
rebuild: clean all
.PHONY: all clean rebuild
