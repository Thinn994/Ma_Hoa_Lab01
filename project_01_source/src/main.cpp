/*
 * ============================================================================
 * CHƯƠNG TRÌNH CHÍNH: TRAO ĐỔI KHÓA DIFFIE-HELLMAN (DHKE)
 * Môn học: Nhập môn Mã hóa Mật mã - Bộ môn Công nghệ Tri thức
 * ============================================================================
 * Cấu trúc chương trình hoàn thành theo mẫu đề bài Lab01:
 *   1. Sinh số nguyên tố lớn an toàn p và phần tử sinh g
 *   2. Sinh khóa riêng của Alice và Bob trong khoảng [2, p - 2]
 *   3. Tính giá trị công khai của Alice và Bob bằng hàm modular_exponentiation
 *   4. Trao đổi giá trị công khai và tính bí mật chung
 *   5. Hiển thị kết quả và xác minh bí mật chung trùng khớp
 * ============================================================================
 */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "dh.h"

int main(int argc, char *argv[])
{
    // ========================================================================
    // D: HOÀN THÀNH LOGIC TRAO ĐỔI KHÓA DIFFIE-HELLMAN
    // ========================================================================

    // 1. Sinh số nguyên tố lớn p và phần tử sinh g
    // Kích thước bit ví dụ, có thể điều chỉnh qua tham số dòng lệnh (mặc định 512 bit)
    int bit_size = 512;
    if (argc >= 2)
    {
        int custom_bits = std::atoi(argv[1]);
        if (custom_bits >= 32 && custom_bits <= 512)
        {
            bit_size = custom_bits;
        }
    }

    // Khởi tạo bộ sinh số giả ngẫu nhiên PRNG (SHAKE-128 / Keccak sponge)
    prng_ctx ctx;
    uint8_t seed[32];
    std::memcpy(seed, "Lab01_SafePrime_DHKE_Seed2024!!", sizeof(seed));
    prng_seed(&ctx, seed, sizeof(seed));

    std::cout << "--- TRAO DOI KHOA DIFFIE-HELLMAN (KICH THUOC: " << bit_size << " BIT) ---\n\n";
    std::cout << "[1] Dang sinh so nguyen to an toan p (" << bit_size << " bit)...\n";

    // Sinh một số nguyên tố an toàn p thỏa mãn p và (p - 1) / 2 đều là số nguyên tố
    uint512_t p = generate_safe_prime(&ctx, bit_size);
    std::cout << "So nguyen to an toan p: " << p << "\n";

    // Phần tử sinh g (g = 2 là phần tử sinh phổ biến cho nhóm số nguyên tố an toàn)
    uint512_t g = 2;
    std::cout << "Phan tu sinh g: " << g << "\n\n";

    // 2. Sinh khóa riêng của Alice và Bob
    std::cout << "[2] Sinh khoa rieng cua Alice va Bob...\n";
    uint512_t a = generate_private_key(&ctx, p); // Khóa riêng của Alice nằm trong [2, p - 2]
    uint512_t b = generate_private_key(&ctx, p); // Khóa riêng của Bob nằm trong [2, p - 2]
    std::cout << "Khoa rieng Alice (a): " << a << "\n";
    std::cout << "Khoa rieng Bob   (b): " << b << "\n\n";

    // 3. Tính giá trị công khai của Alice và Bob
    std::cout << "[3] Tinh gia tri cong khai cua Alice va Bob...\n";
    uint512_t A = modular_exponentiation(g, a, p); // Alice tính A = g^a % p
    uint512_t B = modular_exponentiation(g, b, p); // Bob tính B = g^b % p
    std::cout << "Gia tri cong khai Alice (A = g^a % p): " << A << "\n";
    std::cout << "Gia tri cong khai Bob   (B = g^b % p): " << B << "\n\n";

    // 4. Tính bí mật chung
    std::cout << "[4] Tinh bi mat chung...\n";
    uint512_t alice_shared_secret = modular_exponentiation(B, a, p); // Alice tính s = B^a % p
    uint512_t bob_shared_secret   = modular_exponentiation(A, b, p); // Bob tính s = A^b % p

    // 5. Hiển thị kết quả và xác minh rằng bí mật chung trùng khớp
    std::cout << "\n[5] Hien thi ket qua va xac minh rang bi mat chung trung khop:\n";
    std::cout << "Bi mat chung Alice nhan duoc: " << alice_shared_secret << "\n";
    std::cout << "Bi mat chung Bob nhan duoc: " << bob_shared_secret << "\n";
    std::cout << "Qua trinh tinh toan dung khong? " << (alice_shared_secret == bob_shared_secret) << "\n";

    return (alice_shared_secret == bob_shared_secret) ? 0 : 1;
}
