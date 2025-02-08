#include <iostream>
#include <vector>
#include <algorithm>  // For std::sort
#include <chrono>     // For timing
#include <random>     // For generating random numbers
#include <cmath>

// Median-of-three pivot selection
template <typename T>
T median_of_three(T& a, T& b, T& c) {
    if (a < b) {
        if (b < c) return b;  
        return (a < c) ? c : a;
    } else {
        if (a < c) return a;
        return (b < c) ? c : b;
    }
}

// Hoare partitioning
template <typename T>
int partition(std::vector<T>& arr, int left, int right, T pivot) {
    int i = left - 1, j = right + 1;
    while (true) {
        do { i++; } while (arr[i] < pivot);
        do { j--; } while (arr[j] > pivot);
        if (i >= j) return j;
        std::swap(arr[i], arr[j]);
    }
}

// InsertionSort for small partitions
template <typename T>
void insertion_sort(std::vector<T>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        T key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// HeapSort fallback
template <typename T>
void heapsort(std::vector<T>& arr, int left, int right) {
    std::make_heap(arr.begin() + left, arr.begin() + right + 1);
    std::sort_heap(arr.begin() + left, arr.begin() + right + 1);
}

// Iterative introsort (removes recursion overhead)
template <typename T>
void introsort(std::vector<T>& arr, int left, int right, int depth_limit) {
    while (right - left > 16) {  
        if (depth_limit == 0) {  
            heapsort(arr, left, right);
            return;
        }

        T pivot = median_of_three(arr[left], arr[(left + right) / 2], arr[right]);

        int partition_index = partition(arr, left, right, pivot);
        depth_limit--;

        if (partition_index - left < right - partition_index) {
            introsort(arr, left, partition_index, depth_limit);
            left = partition_index + 1;
        } else {
            introsort(arr, partition_index + 1, right, depth_limit);
            right = partition_index;
        }
    }

    insertion_sort(arr, left, right);
}

// Public sort function
template <typename T>
void my_sort(std::vector<T>& arr) {
    int depth_limit = 2 * log2(arr.size());
    introsort(arr, 0, arr.size() - 1, depth_limit);
}

// Function to generate a random array
std::vector<int> generate_random_vector(size_t size) {
    std::vector<int> vec(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 1'000'000);

    for (size_t i = 0; i < size; ++i) {
        vec[i] = dis(gen);
    }

    return vec;
}

// Benchmark function
void benchmark(size_t size, int iterations) {
    std::cout << "Benchmarking with array size: " << size << " (" << iterations << " runs)\n";

    double my_sort_total_time = 0.0;
    double std_sort_total_time = 0.0;

    for (int i = 0; i < iterations; ++i) {
        // Generate a random array
        std::vector<int> arr1 = generate_random_vector(size);
        std::vector<int> arr2 = arr1; // Copy for std::sort

        // Measure time for my_sort
        auto start = std::chrono::high_resolution_clock::now();
        my_sort(arr1);
        auto end = std::chrono::high_resolution_clock::now();
        my_sort_total_time += std::chrono::duration<double, std::milli>(end - start).count();

        // Measure time for std::sort
        start = std::chrono::high_resolution_clock::now();
        std::sort(arr2.begin(), arr2.end());
        end = std::chrono::high_resolution_clock::now();
        std_sort_total_time += std::chrono::duration<double, std::milli>(end - start).count();
    }

    std::cout << "My Sort   Avg Time: " << my_sort_total_time / iterations << " ms\n";
    std::cout << "std::sort Avg Time: " << std_sort_total_time / iterations << " ms\n";
    std::cout << "Performance Ratio (My Sort / std::sort): " << (my_sort_total_time / std_sort_total_time) * 100 << "%\n";
    std::cout << "------------------------------------\n";
}

// Main function
int main() {
    benchmark(10'000, 10);
    benchmark(100'000, 10);
    benchmark(1'000'000, 5);

    return 0;
}
