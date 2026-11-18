#ifndef DH_H
#define DH_H

/*
 * ============================================================================
 * CÁC HÀM CƠ BẢN PHỤC VỤ TRAO ĐỔI KHÓA DIFFIE-HELLMAN (DHKE)
 * ============================================================================
 * Theo yêu cầu của Lab01:
 *   1. Hàm lũy thừa mô-đun (modular exponentiation): (base^exponent) % mod
 *   2. Hàm sinh số nguyên tố an toàn (generate_safe_prime):
 *      p là số nguyên tố và (p - 1) / 2 cũng là số nguyên tố
 *   3. Hàm sinh khóa riêng ngẫu nhiên (generate_private_key):
 *      Khóa riêng nằm trong khoảng [2, p - 2]
 *   4. Trao đổi khóa Diffie-Hellman giữa Alice và Bob
 * ============================================================================
 */

#include <stdint.h>
#include <stddef.h>
#include "uint512.h"

#ifdef _WIN32
#  include "../lib/fips202/prng_win.h"
#else
#  include "../lib/fips202/prng.h"
#endif

// ============================================================================
// CÁC PHÉP TOÁN SỐ HỌC MÔ-ĐUN (THEO PSEUDOCODE BÀI TẬP)
// ============================================================================

// Phép cộng mô-đun: tính (x + y) % p (AddMod trong pseudocode)
uint512_t add_mod(const uint512_t &x, const uint512_t &y, const uint512_t &p);

// Phép nhân mô-đun: tính (x * y) % p (MultMod trong pseudocode - Double-and-Add)
uint512_t mul_mod(const uint512_t &x, const uint512_t &y, const uint512_t &p);

// ============================================================================
// YÊU CẦU A: TRIỂN KHAI HÀM LŨY THỪA MÔ-ĐUN (MODULAR EXPONENTIATION)
// Hàm cần thực hiện: (base^exponent) % mod
// Sử dụng thuật toán nhân và bình phương (Square-and-Multiply - ExpMod pseudocode)
// ============================================================================
uint512_t modular_exponentiation(const uint512_t &base,
                                  const uint512_t &exponent,
                                  const uint512_t &mod);

// ============================================================================
// YÊU CẦU B: TRIỂN KHAI HÀM SINH SỐ NGUYÊN TỐ LỚN AN TOÀN
// ============================================================================

// Thuật toán kiểm tra số nguyên tố Miller-Rabin (Miller-Rabin Primality Test)
// Trả về true nếu n là số nguyên tố (với k vòng kiểm tra xác suất)
bool miller_rabin(const uint512_t &n, int k, prng_ctx *ctx);

// Hàm sinh số nguyên tố lớn an toàn p (Safe Prime):
// Trong đó p là số nguyên tố và (p - 1) / 2 cũng là số nguyên tố
uint512_t generate_safe_prime(prng_ctx *ctx, size_t bit_size = 512);

// ============================================================================
// YÊU CẦU C: TRIỂN KHAI HÀM SINH KHÓA RIÊNG NGẪU NHIÊN
// Khóa riêng phải là một số ngẫu nhiên nằm trong khoảng [2, p - 2]
// ============================================================================
uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p);

#endif // DH_H
