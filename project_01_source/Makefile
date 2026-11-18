ifeq ($(ROOT),)
ROOT:=.
endif

CPP      := g++
# C++14 theo yêu cầu đề bài, không dùng thư viện ngoài (NTL, GMP...)
CPP_FLAGS := -std=c++14 -O2 -Wall -Wextra -Wpedantic

SHA3_SRC     := $(ROOT)/lib/fips202/fips202.c
SHA3_INCLUDE := -I $(ROOT)/lib/fips202
PRNG_SRC     := $(ROOT)/lib/fips202/prng.cpp

SRC     := $(ROOT)/src
INCLUDE := -I $(SRC)

MAIN_DH := $(SRC)/main.cpp
DH_SRC  := $(SRC)/dh.cpp

BIN   := bin
BUILD := bin/build

# ============================================================
# Tạo thư mục
# ============================================================
folders:
	@echo "### Creating output folders"
	mkdir -p $(BUILD) $(BIN)

# ============================================================
# Build target chính: dh
# ============================================================
dh: | folders
	@echo "### Compiling Diffie-Hellman project"
	$(CPP) $(CPP_FLAGS) \
		$(MAIN_DH) $(DH_SRC) $(PRNG_SRC) $(SHA3_SRC) \
		$(INCLUDE) $(SHA3_INCLUDE) \
		-o $(BIN)/dh
	@echo "### Build OK -> $(BIN)/dh"

# ============================================================
# Chạy với tham số mặc định (512-bit)
# ============================================================
run: dh
	@echo "### Running dh (512-bit)"
	./$(BIN)/dh 512

# ============================================================
# Chạy nhanh với 128-bit (để test)
# ============================================================
run-fast: dh
	@echo "### Running dh (128-bit) - faster for testing"
	./$(BIN)/dh 128

# ============================================================
# Dọn dẹp
# ============================================================
clean:
	rm -rf ./bin

.PHONY: folders dh run run-fast clean
