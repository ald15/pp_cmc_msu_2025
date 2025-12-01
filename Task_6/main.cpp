#include <mpi.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <random>


const int NUM_ELEMENTS = 100;   // Количество элементов для обработки
const int TAG = 0;              // Тег для всех MPI сообщений
const int SEED = 42;            // Сид для воспроизводимости случайных чисел

// Случайное float число [1.0, 10.0]
float generate_random_number() {
    static bool initialized = false;
    static std::mt19937 gen;
    static std::uniform_real_distribution<float> distribution(1.0f, 10.0f);
    
    if (!initialized) {
        gen.seed(SEED);
        initialized = true;
    }
    return distribution(gen);
}

int main(int argc, char *argv[]) {
    int rank, size;
    
    // Инициализация MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        printf("Номер по списку: 21 => Паттерн: 6 - линейная цепочка; float; задача: обработка потока чисел. \n");
        printf("Число процессов:   %d.\n", size);
        if (size < 4) printf("Для демонстрации Pipeline желательно мин. 4 процесса...\n");
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    float data = 0.0f;
    float result = 0.0f;
    MPI_Status status;
    
    if (rank == 0) {
        printf("[Процесс %d] генератор\n", rank);
        printf("  Генерация %d случайных чисел (float)\n", NUM_ELEMENTS);
        
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            data = generate_random_number();
            if (size > 1) MPI_Send(&data, 1, MPI_FLOAT, rank + 1, TAG, MPI_COMM_WORLD); // Отправка данных 1 процессу (если он есть)
        }
        
        printf("  [Процесс %d] Генерация завершена!\n\n", rank);
        
    } else if (rank == size - 1) {
        printf("[Процесс %d] аккумулирующий (финальный)\n", rank);
        
        float sum = 0.0f;
        
        // Если это не 1 процесс, то получаем данные от предыдущего
        if (rank > 1)
            for (int i = 0; i < NUM_ELEMENTS; i++) {
                MPI_Recv(&data, 1, MPI_FLOAT, rank - 1, TAG, MPI_COMM_WORLD, &status);        
                sum += data;
            }
        else
            for (int i = 0; i < NUM_ELEMENTS; i++) {
                MPI_Recv(&data, 1, MPI_FLOAT, rank - 1, TAG, MPI_COMM_WORLD, &status);
                result = data * 2.0f;  // f(x) = x * 2
                sum += result;
            } // Если это процесс 1 

        printf("\nРезультаты:\n");
        printf("\tОбщая сумма:       %.6f\n", sum);
        printf("\tЧисло элементов:   %d\n", NUM_ELEMENTS);
        printf("\tСреднее значение:  %.6f\n", sum / NUM_ELEMENTS);
        
    } else {
        printf("[Процесс %d] обработчик\n", rank);
        
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            MPI_Recv(&data, 1, MPI_FLOAT, rank - 1, TAG, MPI_COMM_WORLD, &status); // Получаем данные от предыдущего процесса
            
            // Функции в зависимости от номера процесса
            if (rank == 1) result = data * 2.0f; // Процесс 1: f(x) = x * 2
            else 
                if (rank == 2) result = sinf(data); // Процесс 2: g(x) = sin(x)
                else result = data * data; // Процесс 3+: h(x) = x^2
            MPI_Send(&result, 1, MPI_FLOAT, rank + 1, TAG, MPI_COMM_WORLD); // Отправляем результат следующему процессу
        }
        
        printf("  [Процесс %d] Обработка завершена!\n\n", rank);
    }
    
    MPI_Finalize();
    
    return 0;
}