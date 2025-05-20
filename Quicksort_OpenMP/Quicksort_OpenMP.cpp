#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <omp.h>

#define THRESHOLD 100000

void swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

int partition(std::vector<int>& arr, int low, int high) {
    if (low < 0 || high >= arr.size() || low > high) {
        std::cerr << "Eroare partition: index invalid (" << low << ", " << high << ")\n";
        return low;
    }

    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (arr[j] <= pivot) {
            ++i;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quicksort_parallel(std::vector<int>& arr, int low, int high) {
    if (low < 0 || high >= arr.size() || low >= high)
        return;

    int pi = partition(arr, low, high);

    if ((high - low) < THRESHOLD) {
        quicksort_parallel(arr, low, pi - 1);
        quicksort_parallel(arr, pi + 1, high);
    }
    else {
#pragma omp task shared(arr)
        quicksort_parallel(arr, low, pi - 1);

#pragma omp task shared(arr)
        quicksort_parallel(arr, pi + 1, high);
    }
}

void quicksort(std::vector<int>& arr) {
#pragma omp parallel
    {
#pragma omp single nowait
        quicksort_parallel(arr, 0, arr.size() - 1);
    }
}

void sort_file(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "Eroare: nu pot deschide fisierul " << filename << "\n";
        return;
    }

    int n;
    fin >> n;
    if (n <= 0) {
        std::cerr << "Eroare: numar invalid de elemente: " << n << "\n";
        return;
    }

    std::vector<int> arr(n);
    for (int i = 0; i < n; ++i) {
        if (!(fin >> arr[i])) {
            std::cerr << "Eroare la citirea elementului " << i << "\n";
            return;
        }
    }
    fin.close();

    std::cout << "Sortare " << filename << " (" << n << " elemente)...\n";
    double start = omp_get_wtime();

    quicksort(arr);

    double end = omp_get_wtime();
    std::cout << "Timp: " << (end - start) << " secunde\n\n";
}

int main() {
    omp_set_num_threads(4);  

    int testNumber = 12;  
    std::string filename = "test" + std::to_string(testNumber) + ".txt";

    sort_file(filename);
    return 0;
}

