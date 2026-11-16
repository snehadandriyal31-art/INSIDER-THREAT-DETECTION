# Makefile — Insider Threat Detection (C DSA layer + C++ OOP layer)
#   make            build ./insider_detect
#   make test       build + run all unit tests
#   make asan       rebuild everything with AddressSanitizer/LeakSanitizer (Linux/WSL)
#   make clean
CC      = gcc
CXX     = g++
SAN    ?=
CFLAGS   = -std=c11   -Wall -Wextra -Iinclude -O2 $(SAN)
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -Isrc/cpp -O2 $(SAN)
LDFLAGS  = $(SAN)

C_SRC   = $(wildcard src/c/*.c)
CPP_SRC = $(wildcard src/cpp/*.cpp)
C_OBJ   = $(C_SRC:.c=.o)
CPP_OBJ = $(CPP_SRC:.cpp=.o)
TARGET  = insider_detect

TEST_NAMES = test_array test_list test_stack test_queue test_hash test_sortsearch test_score
TEST_BINS  = $(addprefix tests/bin/,$(TEST_NAMES))

all: $(TARGET)

$(TARGET): $(C_OBJ) $(CPP_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

src/c/%.o: src/c/%.c
	$(CC) $(CFLAGS) -c $< -o $@

src/cpp/%.o: src/cpp/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tests/bin:
	mkdir -p tests/bin

$(TEST_BINS): tests/bin/%: tests/%.c $(C_OBJ) | tests/bin
	$(CC) $(CFLAGS) -Itests -o $@ $< $(C_OBJ) -lm

test: $(TEST_BINS)
	./tests/bin/test_array
	./tests/bin/test_list
	./tests/bin/test_stack
	./tests/bin/test_queue
	./tests/bin/test_hash
	./tests/bin/test_sortsearch
	./tests/bin/test_score
	@echo "ALL TEST SUITES PASSED"

asan:
	$(MAKE) clean
	$(MAKE) SAN="-fsanitize=address,undefined -fno-omit-frame-pointer -g" all test

clean:
	rm -f $(C_OBJ) $(CPP_OBJ) $(TARGET) $(TARGET).exe
	rm -rf tests/bin

.PHONY: all test asan clean
