
#include <iostream>
#include <windows.h>
#include <intrin.h>  
#include <omp.h>
#include <vector>
#include <chrono>
#include<ctime>
#include <cstdint>

using namespace std;
using namespace std::chrono;




void task3() {
    const int size = 1000;
    int arr[size];
    for (int i = 0; i < size; ++i) {
        arr[i] = i + 1;
    }

    auto sum_array = [](int arr[], int size) {
        long long sum = 0;
        for (int i = 0; i < size; ++i) {
            sum += arr[i];
        }
        return sum;
        };

    const double cpu_frequency_hz = 1.0e7;

    unsigned __int64 start_tsc = __rdtsc();
    sum_array(arr, size);
    unsigned __int64 end_tsc = __rdtsc();
    unsigned __int64 ticks_elapsed = end_tsc - start_tsc;

    double time_in_ms = (ticks_elapsed / cpu_frequency_hz) * 1000;

    LARGE_INTEGER frequency, start_qpc, end_qpc;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_qpc);
    sum_array(arr, size);
    QueryPerformanceCounter(&end_qpc);

    double time_qpc = static_cast<double>(end_qpc.QuadPart - start_qpc.QuadPart) * 1000 / frequency.QuadPart;

    cout << "Time (QueryPerformanceCounter): " << time_qpc << " milliseconds" << endl;
    cout << "Time (__rdtsc): " << time_in_ms << " milliseconds" << endl;
    cout << "Cycles (__rdtsc): " << ticks_elapsed << endl;
}



long long sum_array(int arr[], int size) {
    long long sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += arr[i];
    }
    return sum;
}

void task4() {


    const int sizes[] = { 100000, 200000, 300000 };
    int* arr[3];

    for (int i = 0; i < 3; ++i) {
        arr[i] = new int[sizes[i]];
        for (int j = 0; j < sizes[i]; ++j) {
            arr[i][j] = j + 1;
        }
    }

    for (int i = 0; i < 3; ++i) {
        double start_time = omp_get_wtime();
        sum_array(arr[i], sizes[i]);
        double end_time = omp_get_wtime();
        cout << "Absolute time for size " << sizes[i] << ": "
            << (end_time - start_time) << " seconds" << endl;
    }

    for (int i = 0; i < 3; ++i) {
        DWORD start_tick = GetTickCount();
        int iterations = 0;
        long long sum = 0;

        while (GetTickCount() - start_tick < 2000) {
            sum_array(arr[i], sizes[i]);
            iterations++;
        }
        cout << "Relative time (iterations for size " << sizes[i] << "): "
            << iterations << " in 2 seconds" << endl;
    }

    for (int i = 0; i < 3; ++i) {
        delete[] arr[i];
    }
}

void multiplyMatrices(int** A, int** B, int** C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

class Matrix {
public:
    vector<vector<int>> data;
    int size;

    Matrix(int n) : size(n) {
        data.resize(n, vector<int>(n, 0));
    }

    Matrix multiply(const Matrix& other) {
        Matrix result(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < size; k++) {
                    result.data[i][j] += this->data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }
};


void task5_7() {
    const int sizes[] = { 512, 1024, 2048 };

    for (int n : sizes) {
        int** A = new int* [n];
        int** B = new int* [n];
        int** C = new int* [n];
        for (int i = 0; i < n; i++) {
            A[i] = new int[n];
            B[i] = new int[n];
            C[i] = new int[n];
        }

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = rand() % 100;
                B[i][j] = rand() % 100;
            }
        }

        auto start = high_resolution_clock::now();
        multiplyMatrices(A, B, C, n);
        auto end = high_resolution_clock::now();
        cout << "Time without objects for n = " << n << ": "
            << duration_cast<milliseconds>(end - start).count() << " ms" << endl;


        for (int i = 0; i < n; i++) {
            delete[] A[i];
            delete[] B[i];
            delete[] C[i];
        }
        delete[] A;
        delete[] B;
        delete[] C;

        Matrix A_obj(n), B_obj(n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A_obj.data[i][j] = rand() % 100;
                B_obj.data[i][j] = rand() % 100;
            }
        }

        start = high_resolution_clock::now();
        Matrix C_obj = A_obj.multiply(B_obj);
        end = high_resolution_clock::now();
        cout << "Time with objects for n = " << n << ": "
            << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
    }
}
template <typename T>
void multiplyMatrices(int n, T** A, T** B, T** C) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

template <typename T>
T** createMatrix(int n) {
    T** matrix = new T * [n];
    for (int i = 0; i < n; ++i) {
        matrix[i] = new T[n];
    }
    return matrix;
}

template <typename T>
void fillMatrix(int n, T** matrix) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = static_cast<T>(i + j);
        }
    }
}

template <typename T>
void deleteMatrix(int n, T** matrix) {
    for (int i = 0; i < n; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

template <typename T>
void measureTime(int n) {
    T** A = createMatrix<T>(n);
    T** B = createMatrix<T>(n);
    T** C = createMatrix<T>(n);

    fillMatrix(n, A);
    fillMatrix(n, B);

    auto start = high_resolution_clock::now();
    multiplyMatrices(n, A, B, C);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start).count();

    cout << "Time taken for type " << typeid(T).name() << ": " << duration << " ms" << endl;

    deleteMatrix(n, A);
    deleteMatrix(n, B);
    deleteMatrix(n, C);
}

void task9() {
    const int n = 1024;

    cout << "Starting matrix multiplication performance test with size " << n << "x" << n << endl;

    measureTime<int8_t>(n);
    measureTime<int16_t>(n);
    measureTime<int32_t>(n);
    measureTime<__int64>(n);
    measureTime<float>(n);
    measureTime<double>(n);
}


int main() {

    //task3();
    //task4();
    //task9();
    task5_7();
    return 0;
}