#include <immintrin.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <cstring>

// Реализация через AVX интринсик _mm256_load_ps
void vector_add_avx(const float* a, const float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; i += 8) {
        __m256 vec_a = _mm256_load_ps(&a[i]);
        __m256 vec_b = _mm256_load_ps(&b[i]);
        __m256 vec_result = _mm256_add_ps(vec_a, vec_b);
        _mm256_store_ps(&result[i], vec_result);
    }
}

// Cкалярная реализация сложения
void vector_add_scalar(const float* a, const float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; i++) result[i] = a[i] + b[i];
}

int main() {
    const size_t N = 8 * 1024 * 1024;  // 8 млн float (должно быть кратно 8)
    const int ITERATIONS = 10;
    
    std::cout << "Номер по списку: 21 => интринсик _mm256_load_ps." << std::endl;
    std::cout << "Размер векторов: " << N << " float (" << (N * sizeof(float) / (1024*1024)) << " МБ)." << std::endl;
    
    // Выделяем выровненную память
    float* a = (float*)aligned_alloc(32, N * sizeof(float));
    float* b = (float*)aligned_alloc(32, N * sizeof(float));
    float* result_avx = (float*)aligned_alloc(32, N * sizeof(float));
    float* result_scalar = (float*)aligned_alloc(32, N * sizeof(float));
    
    // Инициализируем данные
    for (size_t i = 0; i < N; i++) {
        a[i] = static_cast<float>(i) * 0.5f;
        b[i] = static_cast<float>(i) * 0.3f;
    }
    
    vector_add_avx(a, b, result_avx, N);
    vector_add_scalar(a, b, result_scalar, N);
    
    auto start_avx = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < ITERATIONS; iter++) vector_add_avx(a, b, result_avx, N);
    auto end_avx = std::chrono::high_resolution_clock::now();
    auto duration_avx = std::chrono::duration_cast<std::chrono::milliseconds>(end_avx - start_avx).count();
    
    auto start_scalar = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < ITERATIONS; iter++) vector_add_scalar(a, b, result_scalar, N);
    auto end_scalar = std::chrono::high_resolution_clock::now();
    auto duration_scalar = std::chrono::duration_cast<std::chrono::milliseconds>(end_scalar - start_scalar).count();
    
    std::cout << "Результаты (" << ITERATIONS << " итераций):" << std::endl;
    std::cout << "\t- _mm256_load_ps: " << duration_avx << " мс." << std::endl;
    std::cout << "\t- обычное скалярное: " << duration_scalar << " мс." << std::endl;
    std::cout << "\t- ускорение: " << (static_cast<double>(duration_scalar) / duration_avx) << "."<< std::endl;
    
    free(a);
    free(b);
    free(result_avx);
    free(result_scalar);
    
    return 0;
}