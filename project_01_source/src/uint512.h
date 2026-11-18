#ifndef UINT512_H
#define UINT512_H

/*
 * ============================================================================
 * HỆ THỐNG XỬ LÝ SỐ NGUYÊN LỚN (512-BIT UNSIGNED INTEGER)
 * ============================================================================
 * - Phục vụ bài tập: Số nguyên tố & Trao đổi khóa Diffie-Hellman (Lab01)
 * - Yêu cầu: Tự cài đặt kiểu dữ liệu số lớn, không sử dụng thư viện hỗ trợ số lớn
 *   bên ngoài (như GMP, NTL,...), tuân thủ chuẩn C++14 trở xuống.
 *
 * Biểu diễn:
 *   - Mảng 8 phần tử uint64_t: digits[8] tương ứng 8 * 64 = 512 bit.
 *   - Thứ tự lưu trữ Little-Endian theo word:
 *       + digits[0]: 64-bit có trọng số thấp nhất (LSW - Least Significant Word).
 *       + digits[7]: 64-bit có trọng số cao nhất (MSW - Most Significant Word).
 * ============================================================================
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <iostream>
#include <iomanip>

// ============================================================================
// HÀM HỖ TRỢ XỬ LÝ BIT VÀ CARRY/BORROW TRÊN TỪ 64-BIT (KHÔNG DÙNG __uint128_t)
// ============================================================================

// Cộng 2 số 64-bit có cờ nhớ carry: *result = a + b + carry_in, trả về carry_out (0 hoặc 1)
static inline uint64_t add64_carry(uint64_t a, uint64_t b,
                                    uint64_t carry_in, uint64_t *result)
{
    uint64_t s = a + b;
    uint64_t c1 = (s < a) ? 1ULL : 0ULL;
    *result = s + carry_in;
    uint64_t c2 = (*result < s) ? 1ULL : 0ULL;
    return c1 + c2;
}

// Trừ 2 số 64-bit có cờ mượn borrow: *result = a - b - borrow_in, trả về borrow_out (0 hoặc 1)
static inline uint64_t sub64_borrow(uint64_t a, uint64_t b,
                                     uint64_t borrow_in, uint64_t *result)
{
    uint64_t s = a - b;
    uint64_t b1 = (a < b) ? 1ULL : 0ULL;
    *result = s - borrow_in;
    uint64_t b2 = (s < borrow_in) ? 1ULL : 0ULL;
    return b1 + b2;
}

// ============================================================================
// ĐỊNH NGHĨA KIỂU DỮ LIỆU SỐ LỚN 512-BIT: uint512_t
// ============================================================================
struct uint512_t
{
    uint64_t digits[8]; // 8 * 64 = 512 bit

    // Constructor mặc định khởi tạo toàn bộ số 0
    uint512_t()
    {
        memset(digits, 0, sizeof(digits));
    }

    // Constructor khởi tạo từ số nguyên 64-bit không dấu
    uint512_t(uint64_t val)
    {
        memset(digits, 0, sizeof(digits));
        digits[0] = val;
    }

    // Lấy giá trị của bit thứ i (0 <= i < 512, bit 0 là LSB)
    int bit(int i) const
    {
        int word = i / 64;
        int off  = i % 64;
        return (int)((digits[word] >> off) & 1ULL);
    }

    // Kiểm tra số có bằng 0 hay không
    bool is_zero() const
    {
        for (int i = 0; i < 8; i++)
        {
            if (digits[i] != 0) return false;
        }
        return true;
    }

    // Kiểm tra số có bằng 1 hay không
    bool is_one() const
    {
        if (digits[0] != 1) return false;
        for (int i = 1; i < 8; i++)
        {
            if (digits[i] != 0) return false;
        }
        return true;
    }
};

// ============================================================================
// CÁC TOÁN TỬ SO SÁNH (==, !=, <, >, <=, >=)
// ============================================================================

inline bool operator==(const uint512_t &a, const uint512_t &b)
{
    return memcmp(a.digits, b.digits, sizeof(a.digits)) == 0;
}

inline bool operator!=(const uint512_t &a, const uint512_t &b)
{
    return !(a == b);
}

inline bool operator<(const uint512_t &a, const uint512_t &b)
{
    // So sánh từ word có trọng số cao nhất (MSW) xuống thấp nhất (LSW)
    for (int i = 7; i >= 0; i--)
    {
        if (a.digits[i] < b.digits[i]) return true;
        if (a.digits[i] > b.digits[i]) return false;
    }
    return false; // Hai số bằng nhau
}

inline bool operator>(const uint512_t &a, const uint512_t &b)  { return b < a; }
inline bool operator<=(const uint512_t &a, const uint512_t &b) { return !(b < a); }
inline bool operator>=(const uint512_t &a, const uint512_t &b) { return !(a < b); }

// ============================================================================
// CÁC TOÁN TỬ SỐ HỌC CƠ BẢN TRÊN SỐ LỚN
// ============================================================================

// Phép cộng hai số lớn (a + b)
inline uint512_t operator+(const uint512_t &a, const uint512_t &b)
{
    uint512_t result;
    uint64_t carry = 0;
    for (int i = 0; i < 8; i++)
    {
        carry = add64_carry(a.digits[i], b.digits[i], carry, &result.digits[i]);
    }
    return result;
}

// Phép trừ hai số lớn (a - b, với điều kiện a >= b)
inline uint512_t operator-(const uint512_t &a, const uint512_t &b)
{
    uint512_t result;
    uint64_t borrow = 0;
    for (int i = 0; i < 8; i++)
    {
        borrow = sub64_borrow(a.digits[i], b.digits[i], borrow, &result.digits[i]);
    }
    return result;
}

// Phép dịch trái 1 bit (nhân đôi số lớn: a * 2)
inline uint512_t shl1(const uint512_t &a)
{
    uint512_t result;
    uint64_t carry = 0;
    for (int i = 0; i < 8; i++)
    {
        result.digits[i] = (a.digits[i] << 1) | carry;
        carry = a.digits[i] >> 63;
    }
    return result;
}

// Phép dịch phải 1 bit (chia đôi số lớn: a / 2)
inline uint512_t shr1(const uint512_t &a)
{
    uint512_t result;
    uint64_t carry = 0;
    for (int i = 7; i >= 0; i--)
    {
        result.digits[i] = (a.digits[i] >> 1) | carry;
        carry = (a.digits[i] & 1ULL) << 63;
    }
    return result;
}

// ============================================================================
// XUẤT DỮ LIỆU SỐ LỚN (HỖ TRỢ std::cout << number THEO CHƯƠNG TRÌNH MẪU)
// ============================================================================

// In số lớn ở định dạng thập lục phân (Hexadecimal)
inline void print_hex(std::ostream &os, const uint512_t &n)
{
    bool leading_zero = true;
    for (int i = 7; i >= 0; i--)
    {
        if (leading_zero && n.digits[i] == 0) continue;
        if (leading_zero)
        {
            os << std::hex << n.digits[i];
            leading_zero = false;
        }
        else
        {
            os << std::hex << std::setw(16) << std::setfill('0') << n.digits[i];
        }
    }
    if (leading_zero) os << "0";
    os << std::dec; // Khôi phục định dạng thập phân
}

// Nạp chồng toán tử << để tương thích trực tiếp với chương trình mẫu của Lab01
inline std::ostream& operator<<(std::ostream &os, const uint512_t &n)
{
    os << "0x";
    print_hex(os, n);
    return os;
}

#endif // UINT512_H
