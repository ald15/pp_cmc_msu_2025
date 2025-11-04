/**
 * Номер по списку: 21
 * Задача: Найти количество положительных элементов в массиве
 * Метод параллелизации: Априорное разделение, packaged_task
 * Тип данных: float
 */

#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <random>
#include <chrono>

// Функция для подсчета положительных элементов в одной части массива
size_t count_positive(const std::vector<float>& arr, size_t start, size_t end) {
    size_t count = 0;
    for (size_t i = start; i < end; ++i)
        if (arr[i] > 0.0f) ++count;
    return count;
}

int main() {
    std::cout << "Номер по списку: 21." << std::endl;
    std::cout << "Задача: Подсчет положительных элементов в массиве." << std::endl;
    constexpr size_t n = 100000000;
    // Генерация случайного массива с элементами типа float
    std::vector<float> arr(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
    for (size_t i = 0; i < n; ++i) {
        arr[i] = dist(gen);
    }

#ifdef PARALLEL
    std::cout << "[Параллельная версиия]" << std::endl;
#ifdef NTHREADS
    // Явно заданное количество потоков
    size_t num_threads = NTHREADS;
#else
    // Все доступные потоки
    size_t num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 2;
#endif
    std::cout << "Количество потоков: " << num_threads << std::endl;
    auto start_time = std::chrono::steady_clock::now();
    std::vector<std::thread> threads(num_threads);
    std::vector<std::future<size_t>> futures(num_threads);

    // Вычисляем размер блока для каждого потока 
    size_t block_size = n / num_threads;

    // Запускаем потоки с packaged_task
    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * block_size;
        size_t end = (i == num_threads - 1) ? n : (i + 1) * block_size;
        std::packaged_task<size_t()> task(
            [&arr, start, end]() {
                return count_positive(arr, start, end);
            }
        );

        futures[i] = task.get_future();
        threads[i] = std::thread(std::move(task));
    }

    for (size_t i = 0; i < num_threads; ++i) threads[i].join();
    size_t total_count = 0;
    for (size_t i = 0; i < num_threads; ++i) total_count += futures[i].get();
    auto end_time = std::chrono::steady_clock::now();
    auto diff = end_time - start_time;
    int time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
    std::cout << "Найдено:" << total_count << " положительных элементов." << std::endl;
    std::cout << "Время: " << time_ms << " мс." << std::endl;

#else
    std::cout << "[Последовательной версия]" << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    size_t total_count = count_positive(arr, 0, n);

    auto end_time = std::chrono::steady_clock::now();
    auto diff = end_time - start_time;
    int time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    std::cout << "Найдено: " << total_count << " положительных элементов." << std::endl;
    std::cout << "Время: " << time_ms << " мс." << std::endl;
#endif
    return 0;
}
