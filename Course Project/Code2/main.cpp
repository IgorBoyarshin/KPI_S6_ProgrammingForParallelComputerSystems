//-----------------------------------------------------------------------------
// Course Project: MPI
// Task: A = (B*C) * X + e * S * (MR*MT) - Z
// Author: Igor Boyarshin
// Date: 15.04.2018
//-----------------------------------------------------------------------------

#include <iostream>
#include <time.h>
#include <mpi.h>


// Constants
const unsigned int N = 4;
const unsigned int L = 2;
const unsigned int P = L * L;
const unsigned int H = N / P;
const unsigned int STACK_SIZE = 100000000;
const bool DO_PRINT = true;


// Types
struct Vector {
    private:
        int * elements;

    public:
        const unsigned int hs;
        const unsigned int size;

        Vector(unsigned int hs) : hs(hs), size(hs * H) {
            elements = new int[size];
            std::fill(elements, elements + size, 0);
        }

        ~Vector() {
            delete[] elements;
        }

        int& operator[](unsigned int row) {
            return elements[row];
        }

        const int& operator[](unsigned int row) const {
            return elements[row];
        }

        void* getPrt() const {
            return (void*)(&(*elements));
        }
};


struct Matrix {
    private:
        int * elements;

    public:
        const unsigned int hs;
        const unsigned int size;

        Matrix(unsigned int hs)
            : hs(hs), size(hs * H * N) {
            elements = new int[size];
            std::fill(elements, elements + size, 0);
        }

        ~Matrix() {
            delete[] elements;
        }

        int& operator[](unsigned int row) {
            return elements[row];
        }

        const int& operator[](unsigned int row) const {
            return elements[row];
        }

        void* getPrt() const {
            return (void*)(&(*elements));
        }
};


// Functions
void fillVector(Vector& vector, unsigned int value);
void fillMatrix(Matrix& matrix, unsigned int value);
void outputVector(const Vector& vector);
void outputMatrix(const Matrix& matrix);
unsigned int getSize(bool isDirect, unsigned int row, unsigned int column);
unsigned int getRank(unsigned int row, unsigned int column);


// Data
// int e, d;
// Vector A, B, C, X, S, Z;
// Matrix MR, MT;
//-----------------------------------------------------------------------------
void ThreadFunction(unsigned int row, unsigned int column) {
    // Data
    const unsigned int size_direct = getSize(true, row, column);
    const unsigned int size_reversed = getSize(false, row, column);
    int e;//, d;
    Vector A(size_direct);
    Vector C(size_direct);
    Matrix MR(size_direct);
    Vector B(size_reversed);
    Vector S(L * L);
    Vector X(size_reversed);
    Vector Z(size_reversed);
    Matrix MT(L * L);

    // 1. Input
    if (row == 0 && column == 0) {
        e = 1;
        fillVector(C, 1);
        fillMatrix(MR, 1);
    } else if (row == L - 1 && column == L - 1) {
        fillVector(B, 1);
        fillVector(S, 1);
        fillVector(X, 1);
        fillVector(Z, 1);
        fillMatrix(MT, 1);
    }

    // 2. horizontal wave
    if (row == 0 && (0 <= column && column <= L - 2)) {
        MPI_Send(&e, 1, MPI_INT, getRank(0, column + 1), MPI_ANY_TAG, MPI_COMM_WORLD);
        MPI_Send(C.getPrt(), C.size, MPI_INT, getRank(0, column + 1), MPI_ANY_TAG, MPI_COMM_WORLD);
    }


    // 3. vertical wave
}


//-----------------------------------------------------------------------------


int main() {
    std::cout << "Started" << std::endl;
    int size, rank;
    MPI_Init(0, 0);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const unsigned int row = rank / L;
    const unsigned int column = rank % L;
    ThreadFunction(row, column);

    MPI_Finalize();
    std::cout << "Finished" << std::endl;

    return 0;









    // Preparations
    // fillVector(A, 0);


    // const time_t start = clock();
    //
    // const time_t finish = clock() - start;
    // std::cout << "Elapsed time = " << static_cast<double>(finish) / 1000.0
    //           << std::endl;
}


//-----------------------------------------------------------------------------
// Function definitions
void fillVector(Vector& Vector, unsigned int value) {
    for (unsigned int i = 0; i < N; i++) {
        Vector[i] = value;
    }
}

void fillMatrix(Matrix& matrix, unsigned int value) {
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            matrix[i * N + j] = value;
        }
    }
}

void outputVector(const Vector& vector) {
    for (unsigned int i = 0; i < vector.size * H; i++) {
        std::cout << vector[i] << " ";
    }
    std::cout << std::endl;
}

void outputMatrix(const Matrix& matrix) {
    for (unsigned int i = 0; i < matrix.size * H; i++) {
        for (unsigned int j = 0; j < N; j++) {
            std::cout << matrix[i * N + j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

unsigned int getSize(bool direct, unsigned int row, unsigned int column) {
    if (direct) {
        if (row == 0) {
            return (L - column) * L;
        } else {
            return (L - row);
        }
    } else {
        if (row == L - 1) {
            return (column + 1) * L;
        } else {
            return (row + 1);
        }
    }
}

unsigned int getRank(unsigned int row, unsigned int column) {
    return row * L + column;
}



// Threads
// void ThreadFunction() {
//     const unsigned int tid = (static_cast<Pointer>(arguments))->tid;
//     const unsigned int low = (tid - 1) * H;
//     const unsigned int high = tid * H;
//
//     if (DO_PRINT) {
//         std::cout << "Thread " << tid << " started..." << std::endl;
//     }
//
//     // Input
//     switch (tid) {
//         case 1:
//             e = 1;
//             fillVector(C, 1);
//             fillMatrix(MR, 1);
//             break;
//         case P:
//             fillVector(B, 1);
//             fillVector(S, 1);
//             fillVector(X, 1);
//             fillMatrix(MT, 1);
//             fillVector(Z, 1);
//             break;
//         default:
//             break;
//     }
//
//     // Synchronization on input
//     switch (tid) {
//         case 1:
//             SetEvent(Event_InputFinishIn1);
//             WaitForSingleObject(Event_InputFinishInP, INFINITE);
//             break;
//         case P:
//             SetEvent(Event_InputFinishInP);
//             WaitForSingleObject(Event_InputFinishIn1, INFINITE);
//             break;
//         default:
//             WaitForSingleObject(Event_InputFinishIn1, INFINITE);
//             WaitForSingleObject(Event_InputFinishInP, INFINITE);
//     }
//
//     // Calculation1
//     int di = 0;
//     for (unsigned int i = low; i < high; i++) {
//         di += B[i] * C[i];
//     }
//
//     // Update common d
//     EnterCriticalSection(&CriticalSection_UpdateD);
//     d += di;
//     LeaveCriticalSection(&CriticalSection_UpdateD);
//
//     // Signal about the end of Calculations1
//     ReleaseSemaphore(Semaphores_Calculation1EndIn[tid - 1], P, NULL);
//
//     // Copy1
//     WaitForSingleObject(Mutex_CopyE, INFINITE);
//     const int ei = e;
//     ReleaseMutex(Mutex_CopyE);
//
//     WaitForSingleObject(Mutex_CopyS, INFINITE);
//     const Vector Si = S.copy();
//     ReleaseMutex(Mutex_CopyS);
//
//     WaitForSingleObject(Semaphore_CopyMt, INFINITE);
//     const Matrix MTi = MT.copy();
//     ReleaseSemaphore(Semaphore_CopyMt, 1, NULL);
//
//     // Wait for the end of Calculations1 in other threads
//     for (unsigned int i = 0; i < P; i++) {
//         WaitForSingleObject(Semaphores_Calculation1EndIn[i], INFINITE);
//     }
//
//     // Copy d
//     EnterCriticalSection(&CriticalSection_CopyD);
//     di = d;
//     LeaveCriticalSection(&CriticalSection_CopyD);
//
//     // Calculations2
//     for (unsigned int h = low; h < high; h++) {
//         const Vector& row = MR[h];
//         int elem = 0;
//         for (unsigned int i = 0; i < N; i++) { // cols 2
//             int temp = 0;
//             for (unsigned int j = 0; j < N; j++) { // elem
//                 temp += row[j] * MTi[j][i];
//             }
//             elem += temp * Si[i];
//         }
//         A[h] = di * X[h] + ei * elem - Z[h];
//     }
//
//     // Synchronization of Calculation2 end
//     switch (tid) {
//         case 1:
//             for (unsigned int i = 0; i < P - 1; i++) {
//                 WaitForSingleObject(Event_Calculation2EndIn[i], INFINITE);
//             }
//             break;
//         default:
//             // -2: one for zero-based, one for T1 not being accounter for here
//             SetEvent(Event_Calculation2EndIn[tid - 2]);
//     }
//
//     if (tid == 1) {
//         // Output A
//         if (N <= 8) {
//             outputVector(A);
//         }
//     }
//
//     if (DO_PRINT) {
//         std::cout << "Thread " << tid << " finished." << std::endl;
//     }
// };
