#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

void quicksort(long* arr, int left, int right) {
    int i = left, j = right;
    long pivot = arr[(left + right) / 2];
    long temp;

    while (i <= j) {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j) {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
            j--;
        }
    }

    if (left < j)
        quicksort(arr, left, j);
    if (i < right)
        quicksort(arr, i, right);
}

int main(int argc, char** argv) {
    int rank, size;
    long* data = nullptr;
    long* local_data = nullptr;
    int n = 0;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int* recvcounts = new int[size];
    int* displs = new int[size];

    if (rank == 0) {
        ifstream infile("test1.txt");
        if (!infile.is_open()) {
            cerr << "Error opening input file" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        infile >> n;

        if (n <= 0) {
            cerr << "Invalid input size" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        data = new long[n];
        for (int i = 0; i < n; i++) {
            infile >> data[i];
        }

        infile.close();
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int base_size = n / size;
    int remainder = n % size;
    for (int i = 0; i < size; i++) {
        recvcounts[i] = base_size + (i < remainder ? 1 : 0);
        displs[i] = (i == 0) ? 0 : displs[i - 1] + recvcounts[i - 1];
    }

    local_data = new long[recvcounts[rank]];

    MPI_Scatterv(data, recvcounts, displs, MPI_LONG,
        local_data, recvcounts[rank], MPI_LONG, 0, MPI_COMM_WORLD);

    start_time = MPI_Wtime();
    quicksort(local_data, 0, recvcounts[rank] - 1);
    end_time = MPI_Wtime();
    double local_sort_time = end_time - start_time;

    MPI_Gatherv(local_data, recvcounts[rank], MPI_LONG,
        data, recvcounts, displs, MPI_LONG, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        sort(data, data + n);  

        cout << "\nVector sortat:\n";
        for (int i = 0; i < n; i++) {
            cout << data[i] << " ";
        }
        cout << "\n";

        double max_time;
        MPI_Reduce(&local_sort_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        cout << "Timp maxim de sortare locala intre procese: " << max_time << " secunde" << endl;
    }
    else {
        MPI_Reduce(&local_sort_time, nullptr, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    }
    //cout << "Timp maxim de sortare locala intre procese: " << end_time - start_time << " secunde" << endl;

    delete[] local_data;
    delete[] recvcounts;
    delete[] displs;
    if (rank == 0)
        delete[] data;

    MPI_Finalize();
    return 0;
}








