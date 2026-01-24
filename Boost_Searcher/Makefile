# Boost_Searcher Linux Makefile
# 用于在没有 CMake 的情况下编译项目

CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2 -I./cppjieba/include -I./cppjieba/deps/limonp/include -I./test/cpp-httplib-v0.7.15
LDFLAGS = -lboost_system -lboost_filesystem -ljsoncpp -lpthread

# 目标文件
TARGETS = parser debug http_server test_jieba

# 默认目标
all: $(TARGETS)

# 编译所有目标
parser: parser.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

debug: debug.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

http_server: http_server.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

test_jieba: test_jieba.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# 清理编译文件
clean:
	rm -f $(TARGETS)

# 安装目标
install: all
	@echo "安装可执行文件到 /usr/local/bin..."
	@mkdir -p /usr/local/bin
	@cp $(TARGETS) /usr/local/bin/
	@echo "安装完成"

# 卸载目标
uninstall:
	@echo "卸载可执行文件..."
	@rm -f /usr/local/bin/$(TARGETS)
	@echo "卸载完成"

# 帮助信息
help:
	@echo "Boost_Searcher Makefile 使用说明"
	@echo ""
	@echo "可用目标："
	@echo "  all        - 编译所有目标（默认）"
	@echo "  parser     - 编译 parser"
	@echo "  debug      - 编译 debug"
	@echo "  http_server - 编译 http_server"
	@echo "  test_jieba - 编译 test_jieba"
	@echo "  clean      - 清理编译文件"
	@echo "  install    - 安装到 /usr/local/bin"
	@echo "  uninstall  - 从 /usr/local/bin 卸载"
	@echo "  help       - 显示此帮助信息"
	@echo ""
	@echo "示例："
	@echo "  make           - 编译所有目标"
	@echo "  make parser    - 只编译 parser"
	@echo "  make clean      - 清理编译文件"

.PHONY: all clean install uninstall help
