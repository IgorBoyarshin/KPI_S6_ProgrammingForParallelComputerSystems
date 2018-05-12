//-----------------------------------------------------------------------------
// Course Project: WinAPI. Semaphores, Events, Mutexes, Critical Sections
// Task: A = (B*C) * X + e * S * (MR*MT) - Z
// Author: Igor Boyarshin
// Date: 23.03.2018
//-----------------------------------------------------------------------------

#include "windows.h"
#include <chrono>
#include <iostream>


// Constants
const unsigned int N = 600;
const unsigned int P = 6;
const unsigned int H = N / P;
const unsigned int STACK_SIZE = 100000000;
const bool DO_PRINT = true;


// Types
struct Vector {
    private:
        unsigned int elements[N];

    public:
        Vector() {
            std::fill(elements, elements + N, 0);
        }

        unsigned int& operator[](unsigned int row) {
            return elements[row];
        }

        const unsigned int& operator[](unsigned int row) const {
            return elements[row];
        }

        Vector copy() const {
            Vector v;
            for (unsigned int i = 0; i < N; i++) {
                v[i] = elements[i];
            }

            return v;
        }
};


struct Matrix {
    private:
        struct Vector elements[N];

    public:
        struct Vector& operator[](unsigned int row) {
            return elements[row];
        }

        const struct Vector& operator[](unsigned int row) const {
            return elements[row];
        }

        Matrix copy() const {
            Matrix m;
            for (unsigned int i = 0; i < N; i++) {
                const struct Vector& v = elements[i];
                for (unsigned int j = 0; j < N; j++) {
                    m[i][j] = v[j];
                }
            }

            return m;
        }
};


// Functions
void fillVector(struct Vector& vector, unsigned int value);
void fillMatrix(Matrix& matrix, unsigned int value);
void outputVector(const struct Vector& vector);


// Data
int e, d;
Vector A, B, C, X, S, Z;
Matrix MR, MT;

// Semaphores
HANDLE Semaphores_Calculation1EndIn[P];
HANDLE Semaphore_CopyMt;

// Mutexes
HANDLE Mutex_CopyE;
HANDLE Mutex_CopyS;

// Critical Sections
CRITICAL_SECTION CriticalSection_UpdateD;
CRITICAL_SECTION CriticalSection_CopyD;

// Events
// manual
HANDLE Event_InputFinishIn1;
HANDLE Event_InputFinishInP;
// automatic
HANDLE Event_Calculation2EndIn[P - 1];


typedef struct ThreadParameters {
    unsigned int tid;
} *Pointer;


//-----------------------------------------------------------------------------
// Threads
void ThreadFunction(LPVOID arguments) {
    const unsigned int tid = (static_cast<Pointer>(arguments))->tid;
    const unsigned int low = (tid - 1) * H;
    const unsigned int high = tid * H;

    if (DO_PRINT) {
        std::cout << "Thread " << tid << " started..." << std::endl;
    }

    // Input
    switch (tid) {
        case 1:
            e = 1;
            fillVector(C, 1);
            fillMatrix(MR, 1);
            break;
        case P:
            fillVector(B, 1);
            fillVector(S, 1);
            fillVector(X, 1);
            fillMatrix(MT, 1);
            fillVector(Z, 1);
            break;
        default:
            break;
    }

    // Synchronization on input
    switch (tid) {
        case 1:
            SetEvent(Event_InputFinishIn1);
            WaitForSingleObject(Event_InputFinishInP, INFINITE);
            break;
        case P:
            SetEvent(Event_InputFinishInP);
            WaitForSingleObject(Event_InputFinishIn1, INFINITE);
            break;
        default:
            WaitForSingleObject(Event_InputFinishIn1, INFINITE);
            WaitForSingleObject(Event_InputFinishInP, INFINITE);
    }

    // Calculation1
    int di = 0;
    for (unsigned int i = low; i < high; i++) {
        di += B[i] * C[i];
    }

    // Update common d
    EnterCriticalSection(&CriticalSection_UpdateD);
    d += di;
    LeaveCriticalSection(&CriticalSection_UpdateD);

    // Signal about the end of Calculations1
    ReleaseSemaphore(Semaphores_Calculation1EndIn[tid - 1], P, NULL);

    // Copy1
    WaitForSingleObject(Mutex_CopyE, INFINITE);
    const int ei = e;
    ReleaseMutex(Mutex_CopyE);

    WaitForSingleObject(Mutex_CopyS, INFINITE);
    const Vector Si = S.copy();
    ReleaseMutex(Mutex_CopyS);

    WaitForSingleObject(Semaphore_CopyMt, INFINITE);
    const Matrix MTi = MT.copy();
    ReleaseSemaphore(Semaphore_CopyMt, 1, NULL);

    // Wait for the end of Calculations1 in other threads
    for (unsigned int i = 0; i < P; i++) {
        WaitForSingleObject(Semaphores_Calculation1EndIn[i], INFINITE);
    }

    // Copy d
    EnterCriticalSection(&CriticalSection_CopyD);
    di = d;
    LeaveCriticalSection(&CriticalSection_CopyD);

    // Calculations2
    for (unsigned int h = low; h < high; h++) {
        const Vector& row = MR[h];
        int elem = 0;
        for (unsigned int i = 0; i < N; i++) { // cols 2
            int temp = 0;
            for (unsigned int j = 0; j < N; j++) { // elem
                temp += row[j] * MTi[j][i];
            }
            elem += temp * Si[i];
        }
        A[h] = di * X[h] + ei * elem - Z[h];
    }

    // Synchronization of Calculation2 end
    switch (tid) {
        case 1:
            for (unsigned int i = 0; i < P - 1; i++) {
                WaitForSingleObject(Event_Calculation2EndIn[i], INFINITE);
            }
            break;
        default:
            // -2: one for zero-based, one for T1 not being accounter for here
            SetEvent(Event_Calculation2EndIn[tid - 2]);
    }

    if (tid == 1) {
        // Output A
        if (N <= 8) {
            outputVector(A);
        }
    }

    if (DO_PRINT) {
        std::cout << "Thread " << tid << " finished." << std::endl;
    }
};


//-----------------------------------------------------------------------------


int main() {
    // Preparations
    fillVector(A, 0);

    // Semaphores
    for (unsigned int i = 0; i < P; i++) {
        Semaphores_Calculation1EndIn[i] = CreateSemaphore(
                NULL,   // default security attributes
                0,      // initial count
                P,      // maximum count
                NULL    // unnamed semaphore
                );
    }
    Semaphore_CopyMt = CreateSemaphore(NULL, 1, 1, NULL);

    // Mutexes
    Mutex_CopyE = CreateMutex(
            NULL,   // default security attributes
            FALSE,  // initially not owned
            NULL    // unnamed
            );
    Mutex_CopyS = CreateMutex(NULL, FALSE, NULL);

    // Critical Sections
    InitializeCriticalSection(&CriticalSection_UpdateD);
    InitializeCriticalSection(&CriticalSection_CopyD);

    // Events
    // manual reset
    Event_InputFinishIn1 = CreateEvent(
            NULL,   // default security attibutes
            TRUE,   // manual reset
            FALSE,  // initial - not signaled
            NULL    // unnamed event
            );
    Event_InputFinishInP = CreateEvent(NULL, TRUE, FALSE, NULL);
    // automatic reset
    for (unsigned int i = 0; i < P - 1; i++) {
        Event_Calculation2EndIn[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }


    // Threads
    struct ThreadParameters arguments[P];

    DWORD Tids[P];
    HANDLE Threads[P];
    const auto start = std::chrono::steady_clock::now();
    for (unsigned int i = 0; i < P; i++) {
        arguments[i].tid = (i + 1);

        Threads[i] = CreateThread(
                NULL,   // Default secutiry attributes
                STACK_SIZE, // stack size
                (LPTHREAD_START_ROUTINE)ThreadFunction, // thread function name
                &arguments[i],   // argument to thread function
                0,      // creating flags(0 => run immediately after creation)
                &Tids[i]   // thread identifier
                );
    }

    // Cleanup
    for (unsigned int i = 0; i < P; i++) {
        WaitForSingleObject(Threads[i], INFINITE);
        CloseHandle(Threads[i]);
    }

    // Chrono
    const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - start
                        ).count();
    std::cout << "Elapsed time = " << delta / 1000.0 << "s"
              << std::endl;

    std::cin.get();
}


//-----------------------------------------------------------------------------
// Function definitions
void fillVector(struct Vector& Vector, unsigned int value) {
    for (unsigned int i = 0; i < N; i++)
        Vector[i] = value;
}

void fillMatrix(Matrix& matrix, unsigned int value) {
    for (unsigned int i = 0; i < N; i++)
        for (unsigned int j = 0; j < N; j++)
            matrix[i][j] = value;
}

void outputVector(const struct Vector& vector) {
    for (unsigned int i = 0; i < N; i++) {
        std::cout << vector[i] << " ";
    }
    std::cout << std::endl;
}
