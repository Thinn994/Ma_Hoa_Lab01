/*
 * ============================================================================
 * dh.cpp - TRIỂN KHAI CÁC NHIỆM VỤ CỦA BÀI TẬP LAB01
 * ============================================================================
 * Các nhiệm vụ triển khai:
 *   A. Hàm lũy thừa mô-đun (modular_exponentiation): (base^exponent) % mod
 *   B. Hàm sinh số nguyên tố ngẫu nhiên an toàn (generate_safe_prime):
 *      p nguyên tố và (p - 1) / 2 nguyên tố, kiểm tra bằng Miller-Rabin.
 *   C. Hàm sinh khóa riêng ngẫu nhiên (generate_private_key) trong [2, p - 2]
 *
 * Các phép toán số học mô-đun tuân thủ chặt chẽ pseudocode hướng dẫn:
 *   - AddMod: (x + y) % p (không tràn số 512-bit)
 *   - MultMod: (x * y) % p (Double-and-Add)
 *   - ExpMod: (x^y) % p (Square-and-Multiply)
 * ============================================================================
 */

#include "dh.h"
#include <iostream>

using namespace std;

// ============================================================================
// HÀM HỖ TRỢ: XÁC ĐỊNH ĐỘ DÀI BIT CÓ NGHĨA (BIT LENGTH)
// ============================================================================
static int bit_length(const uint512_t &n)
{
    for (int i = 7; i >= 0; i--)
    {
        if (n.digits[i] != 0)
        {
            uint64_t w = n.digits[i];
            int b = 0;
            while (w) { w >>= 1; b++; }
            return i * 64 + b;
        }
    }
    return 0;
}

// ============================================================================
// PHÉP CỘNG MÔ-ĐUN: AddMod(x, y, p)
// Tính: (x + y) % p
//
// Pseudocode hướng dẫn:
//   AddMod(x, y, p):
//     x = x % p  // x < p
//     y = y % p  // y < p
//     if x >= p - y:
//         return x - (p - y)
//     return x + y
// ============================================================================
uint512_t add_mod(const uint512_t &x, const uint512_t &y, const uint512_t &p)
{
    // Đảm bảo x, y < p trước khi thực hiện cộng
    uint512_t a = (x >= p) ? (x - p) : x;
    uint512_t b = (y >= p) ? (y - p) : y;

    // Kiểm tra điều kiện a + b >= p bằng phép trừ (a >= p - b) để chống tràn bit
    uint512_t diff = p - b;
    if (a >= diff)
    {
        return a - diff; // Tương đương: (a + b) - p
    }
    return a + b;
}

// ============================================================================
// PHÉP NHÂN MÔ-ĐUN: MultMod(x, y, p)
// Tính: (x * y) % p
//
// Pseudocode hướng dẫn (Double-and-Add):
//   MultMod(x, y, p):
//     ret = 0
//     x = x % p
//     for i = 0 to n-1:
//         if y_i == 1:
//             ret = AddMod(ret, x, p)  // ret = (ret + x) % p
//         x = AddMod(x, x, p)          // x = (2x) % p
//     return ret
// ============================================================================
uint512_t mul_mod(const uint512_t &x, const uint512_t &y, const uint512_t &p)
{
    uint512_t ret; // Khởi tạo ret = 0
    uint512_t base = (x >= p) ? (x - p) : x; // x = x % p

    // Tối ưu hóa: chỉ duyệt đến bit có nghĩa cao nhất của y
    int len = bit_length(y);
    if (len == 0) return ret; // y = 0 thì kết quả bằng 0

    for (int i = 0; i < len; i++)
    {
        if (y.bit(i) == 1)
        {
            ret = add_mod(ret, base, p); // Cập nhật kết quả: ret = (ret + base) % p
        }
        base = add_mod(base, base, p);   // Nhân đôi: base = (2 * base) % p
    }
    return ret;
}

// ============================================================================
// NHIỆM VỤ A: TRIỂN KHAI HÀM LŨY THỪA MÔ-ĐUN (ExpMod trong Pseudocode)
// Hàm cần thực hiện: (base^exponent) % mod
//
// Pseudocode hướng dẫn (Square-and-Multiply):
//   ExpMod(x, y, p):
//     ret = 1
//     base = x % p
//     for i = 0 to n-1:
//         if y_i == 1:
//             ret = MultMod(ret, base, p)   // ret = (ret * base) % p
//         base = MultMod(base, base, p)     // base = (base * base) % p
//     return ret
// ============================================================================
uint512_t modular_exponentiation(const uint512_t &base,
                                  const uint512_t &exponent,
                                  const uint512_t &mod)
{
    uint512_t result(1); // result = 1
    uint512_t b = (base >= mod) ? (base - mod) : base; // base = base % mod

    // Lặp theo từng bit của số mũ (exponent)
    int len = bit_length(exponent);

    for (int i = 0; i < len; i++)
    {
        if (exponent.bit(i) == 1)
        {
            // Cập nhật kết quả với phép nhân mô-đun
            result = mul_mod(result, b, mod);
        }
        // Cập nhật base cho lần lặp tiếp theo bằng phép bình phương mô-đun
        b = mul_mod(b, b, mod);
    }
    return result;
}

// ============================================================================
// HÀM HỖ TRỢ: SINH SỐ NGẪU NHIÊN TRONG ĐOẠN [lo, hi]
// Sử dụng kỹ thuật Rejection Sampling đảm bảo phân phối đều, không gây lệch (bias)
// ============================================================================
static uint512_t random_in_range(prng_ctx *ctx,
                                  const uint512_t &lo,
                                  const uint512_t &hi)
{
    uint512_t range = hi - lo;
    int bits = bit_length(range);
    if (bits == 0) return lo;

    int words_needed = (bits + 63) / 64;
    int top_bit_off = bits % 64;

    while (true)
    {
        uint512_t rnd;
        for (int i = 0; i < words_needed; i++)
        {
            rnd.digits[i] = prng_next_u64(ctx);
        }

        // Mask các bit vượt quá độ dài bit của range
        if (top_bit_off > 0)
        {
            rnd.digits[words_needed - 1] &= ((uint64_t)1 << top_bit_off) - 1;
        }

        // Loại bỏ mẫu ngẫu nhiên lớn hơn range (Rejection Sampling)
        if (rnd > range) continue;

        return lo + rnd;
    }
}

// ============================================================================
// THUẬT TOÁN KIỂM TRA SỐ NGUYÊN TỐ XÁC SUẤT MILLER-RABIN
// ============================================================================
// Thuật toán:
//   1. Phân tích n - 1 = 2^r * d (với d là số lẻ).
//   2. Lặp k lần kiểm tra:
//      a. Chọn cơ số ngẫu nhiên a thuộc [2, n - 2].
//      b. Tính x = a^d % n (bằng hàm modular_exponentiation).
//      c. Nếu x == 1 hoặc x == n - 1 thì vượt qua vòng kiểm tra.
//      d. Lặp r - 1 lần: x = x^2 % n. Nếu x == n - 1 thì vượt qua.
//      e. Nếu không tìm được x == n - 1 thì n chắc chắn là hợp số (Composite).
// ============================================================================
bool miller_rabin(const uint512_t &n, int k, prng_ctx *ctx)
{
    if (n <= uint512_t(1)) return false;
    if (n == uint512_t(2) || n == uint512_t(3)) return true;
    if (n.bit(0) == 0) return false; // Số chẵn lớn hơn 2 là hợp số

    // Bước 1: Viết n - 1 = 2^r * d
    uint512_t n_minus_1 = n - uint512_t(1);
    uint512_t d = n_minus_1;
    int r = 0;
    while (d.bit(0) == 0)
    {
        d = shr1(d); // d = d / 2
        r++;
    }

    uint512_t lo(2);
    uint512_t hi = n - uint512_t(2);

    // Bước 2: Thực hiện k vòng thử nghiệm ngẫu nhiên
    for (int i = 0; i < k; i++)
    {
        uint512_t a = random_in_range(ctx, lo, hi);
        uint512_t x = modular_exponentiation(a, d, n);

        if (x == uint512_t(1) || x == n_minus_1)
            continue;

        bool probably_prime = false;
        for (int j = 0; j < r - 1; j++)
        {
            x = mul_mod(x, x, n); // x = x^2 % n
            if (x == n_minus_1)
            {
                probably_prime = true;
                break;
            }
        }

        if (!probably_prime)
        {
            return false; // n chắc chắn là hợp số
        }
    }
    return true; // n là số nguyên tố với xác suất lỗi tối đa (1/4)^k
}

// ============================================================================
// NHIỆM VỤ B: TRIỂN KHAI HÀM SINH SỐ NGUYÊN TỐ LỚN AN TOÀN
// Định nghĩa: Số nguyên tố an toàn p thỏa mãn:
//   - p là số nguyên tố
//   - q = (p - 1) / 2 cũng là số nguyên tố (q gọi là Sophie Germain prime)
// ============================================================================
uint512_t generate_safe_prime(prng_ctx *ctx, size_t bit_size)
{
    if (bit_size > 512) bit_size = 512;

    // Bảng các số nguyên tố nhỏ sàng lọc sơ bộ (Trial division sieve)
    static const uint32_t small_primes[] = {
        3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,
        79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,
        163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,
        241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,
        337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,
        431,433,439,443,449,457,461,463,467,479,487,491,499
    };
    static const int num_small = (int)(sizeof(small_primes) / sizeof(small_primes[0]));

    int attempt = 0;
    while (true)
    {
        attempt++;

        // 1. Sinh ứng viên số nguyên tố q ngẫu nhiên có độ dài (bit_size - 1) bit
        uint512_t q;
        for (int i = 0; i < 8; i++)
        {
            q.digits[i] = prng_next_u64(ctx);
        }

        size_t q_bits = bit_size - 1;
        {
            int top_word = (int)(q_bits / 64);
            int top_bit  = (int)(q_bits % 64);
            for (int i = top_word + (top_bit > 0 ? 1 : 0); i < 8; i++)
            {
                q.digits[i] = 0;
            }
            if (top_bit > 0)
            {
                q.digits[top_word] &= ((uint64_t)1 << top_bit) - 1;
            }

            // Đảm bảo bit MSB bật để đúng kích thước bit
            if (q_bits > 0)
            {
                int msb_word = (int)((q_bits - 1) / 64);
                int msb_off  = (int)((q_bits - 1) % 64);
                q.digits[msb_word] |= (uint64_t)1 << msb_off;
            }
        }
        // q phải là số lẻ
        q.digits[0] |= 1ULL;

        // 2. Tính số nguyên tố an toàn ứng viên: p = 2 * q + 1
        uint512_t p = shl1(q) + uint512_t(1);

        // Kiểm tra bit MSB của p
        {
            int msb_word = (int)((bit_size - 1) / 64);
            int msb_off  = (int)((bit_size - 1) % 64);
            if (!(p.digits[msb_word] & ((uint64_t)1 << msb_off)))
                continue;
        }

        // 3. Sàng lọc nhanh: kiểm tra chia hết cho các số nguyên tố nhỏ
        bool quick_reject = false;
        for (int si = 0; si < num_small; si++)
        {
            uint64_t sp = (uint64_t)small_primes[si];
            uint64_t pow32 = ((uint64_t)1 << 32) % sp;
            uint64_t pow64 = (pow32 * pow32) % sp;

            uint64_t rem_p = 0, rem_q = 0;
            for (int wi = 7; wi >= 0; wi--)
            {
                rem_p = (rem_p * pow64 % sp + p.digits[wi] % sp) % sp;
                rem_q = (rem_q * pow64 % sp + q.digits[wi] % sp) % sp;
            }
            if (rem_p == 0 || rem_q == 0)
            {
                quick_reject = true;
                break;
            }
        }
        if (quick_reject) continue;

        // 4. Kiểm tra nguyên tố bằng thuật toán Miller-Rabin (k = 20)
        // Cần cả q và p = 2q + 1 đều là số nguyên tố
        if (!miller_rabin(q, 20, ctx)) continue;
        if (!miller_rabin(p, 20, ctx)) continue;

        // Tìm thấy số nguyên tố an toàn thỏa mãn yêu cầu
        return p;
    }
}

// ============================================================
// NHIỆM VỤ C: TRIỂN KHAI HÀM SINH KHÓA RIÊNG NGẪU NHIÊN
// Sử dụng sinh số ngẫu nhiên để tạo khóa riêng
// Khóa riêng phải là một số ngẫu nhiên nằm trong khoảng [2, p - 2]
// ============================================================
uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p)
{
    // Đoạn [2, p - 2]
    uint512_t lo(2);
    uint512_t hi = p - uint512_t(2);

    return random_in_range(ctx, lo, hi);
}
