#include <iostream>
#include <vector>
#include <cstdint>

// ---------------------------------------------------------
// 手动注入 FiPS 缺失的依赖函数，解决 bytehamster::util 兼容性问题
namespace bytehamster::util {
    // 64位哈希扰动函数 (Hash Remixing)
    inline uint64_t remix(uint64_t h) {
        h ^= h >> 33;
        h *= 0xff51afd7ed558ccdULL;
        h ^= h >> 33;
        h *= 0xc4ceb9fe1a85ec53ULL;
        h ^= h >> 33;
        return h;
    }

    // 快速映射函数 (FastRange)
    // 将 64 位哈希值映射到 [0, p) 范围内，无需使用取模 (%) 运算
    inline uint64_t fastrange64(uint64_t word, uint64_t p) {
        return (uint64_t)(((__uint128_t)word * (__uint128_t)p) >> 64);
    }
}
// ---------------------------------------------------------

#include <Fips.h> // 确认大写

// 防止编译器优化查询循环
#define DO_NOT_OPTIMIZE(value) asm volatile("" : : "r,m"(value) : "memory")

int main() {
    const size_t n = 1ULL << 25; // 2^25 = 33,554,432
    std::cout << "Testing with " << n << " keys..." << std::endl;

    // 1. 生成数据
    std::vector<uint64_t> keys(n);
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < n; ++i) {
        keys[i] = rng();
    }

    // 2. 测试 Build 时间
    auto start_build = std::chrono::high_resolution_clock::now();
    
    fips::FiPS<> phf(keys, 1.0); // 默认 gamma = 2.0
    
    auto end_build = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_build = end_build - start_build;
    std::cout << "Build time:  " << diff_build.count() << " s" << std::endl;
    std::cout << "Build speed: " << (n / diff_build.count() / 1e6) << " million keys/s" << std::endl;

    // 3. 测试 Lookup 时间
    // 为了公平测试，我们打乱查询顺序以避免 CPU 缓存预取过于简单
    std::shuffle(keys.begin(), keys.end(), rng);

    auto start_lookup = std::chrono::high_resolution_clock::now();
    
    uint64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        size_t pos = phf(keys[i]);
        DO_NOT_OPTIMIZE(pos);
        sum += pos; // 进一步防止优化
    }
    
    auto end_lookup = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_lookup = end_lookup - start_lookup;
    
    std::cout << "Lookup time:  " << diff_lookup.count() << " s" << std::endl;
    std::cout << "Lookup speed: " << (n / diff_lookup.count() / 1e6) << " million lookups/s" << std::endl;
    std::cout << "Average latency: " << (diff_lookup.count() * 1e9 / n) << " ns/key" << std::endl;
    std::cout << "Size: " << (double) phf.getBits()/n << " bits" << std::endl;

    return 0;
}

