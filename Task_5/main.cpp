#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>

const size_t ARRAY_SIZE = 32 * 1024 * 1024;
const int DEFAULT_THREADS = 4;

void fill_array(std::vector<float>& arr) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    for (size_t i = 0; i < arr.size(); i++) arr[i] = dis(gen);
}

// Последовательная версия
long long count_positive_serial(const std::vector<float>& arr) {
    long long count = 0;
    for (size_t i = 0; i < arr.size(); i++)
        if (arr[i] > 0.0f) count++;
    return count;
}

// Параллельная версия с использованием task
long long count_positive_parallel_task(const std::vector<float>& arr, int num_threads) {
    long long count = 0;
    
    #pragma omp parallel num_threads(num_threads)
    {
        #pragma omp single
        {
            // Рекурсивно создаем задачи для подсчета элементов
            const size_t TASK_SIZE = 1024 * 1024;
            
            for (size_t start = 0; start < arr.size(); start += TASK_SIZE) {
                size_t end = std::min(start + TASK_SIZE, arr.size());
                
                // Создаем отдельную задачу для каждого блока
                #pragma omp task shared(arr, count)
                {
                    long long local_count = 0;
                    for (size_t i = start; i < end; i++)
                        if (arr[i] > 0.0f) local_count++;
                    #pragma omp atomic
                    count += local_count;
                }
            }
            #pragma omp taskwait
        }
    }
    return count;
}

int main(int argc, char* argv[]) {
    std::cout << "Номер по списку: 21 => Найти количество положительных элементов в массиве; task; float." << std::endl;
    
    // Флаги командной строки
    bool use_parallel = true;
    int num_threads = DEFAULT_THREADS;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--serial") use_parallel = false;
        else if (arg == "--threads" && i + 1 < argc) {
            num_threads = std::atoi(argv[i + 1]);
            i++;
        }
    }
    
    std::cout << "Размер массива: " << ARRAY_SIZE << " float (" << (ARRAY_SIZE * sizeof(float) / (1024 * 1024)) << " МБ)." << std::endl;
    std::vector<float> arr(ARRAY_SIZE);
    fill_array(arr);
    
    auto start_serial = std::chrono::high_resolution_clock::now();
    long long result_serial = count_positive_serial(arr);
    auto end_serial = std::chrono::high_resolution_clock::now();
    auto duration_serial = std::chrono::duration_cast<std::chrono::milliseconds>(end_serial - start_serial).count();

    std::cout << "Последовательная версия:" << std::endl;
    std::cout << "\t - " << result_serial << " элементов." << std::endl;
    std::cout << "\t - Время: " << duration_serial << " мс." << std::endl << std::endl;
    
    if (use_parallel) {
        auto start_parallel = std::chrono::high_resolution_clock::now();
        long long result_parallel = count_positive_parallel_task(arr, num_threads);
        auto end_parallel = std::chrono::high_resolution_clock::now();
        auto duration_parallel = std::chrono::duration_cast<std::chrono::milliseconds>(end_parallel - start_parallel).count();
        
        std::cout << "Параллельная версия (" << num_threads << " потока(ов)): " << std::endl;
        std::cout << "\t - " << result_parallel << " элементов." << std::endl;
        std::cout << "\t - Время: " << duration_parallel << " мс." << std::endl;
        
        std::cout << "Ускорение: " << static_cast<double>(duration_serial) / duration_parallel << "." << std::endl;
    }
    
    return 0;
}