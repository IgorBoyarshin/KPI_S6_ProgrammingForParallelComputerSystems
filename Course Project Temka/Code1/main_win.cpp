//-----------------------------------------------------------------------------
// Course Project: WinAPI
// MA = MB * MC + min(Z) * MK
// By Artem Berezynec
//-----------------------------------------------------------------------------

#include "windows.h"
#include <chrono>
#include <iostream>


// Constants
const unsigned int N = 1800;
const unsigned int P = 4;
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
void outputMatrix(const Matrix& matrix);


// Data
unsigned int x = 32500;
Vector Z;
Matrix MA, MB, MC, MK;

// Semaphores
HANDLE Semaphores_Calculation1EndIn[P];

// Mutexes
HANDLE Mutex_CopyMc;

// Critical Sections
CRITICAL_SECTION CriticalSection_UpdateX;
CRITICAL_SECTION CriticalSection_CopyX;

// Events
// manual
HANDLE Event_InputFinishIn1;
HANDLE Event_InputFinishIn3;
HANDLE Event_InputFinishIn4;
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
            fillMatrix(MB, 1);
            break;
        case 3:
            fillMatrix(MC, 1);
            fillVector(Z, 1);
            break;
        case 4:
            fillMatrix(MK, 1);
            break;
        default:
            break;
    }

    // Synchronization on input
    switch (tid) {
        case 1:
            SetEvent(Event_InputFinishIn1);
            WaitForSingleObject(Event_InputFinishIn3, INFINITE);
            WaitForSingleObject(Event_InputFinishIn4, INFINITE);
            break;
        case 3:
            SetEvent(Event_InputFinishIn3);
            WaitForSingleObject(Event_InputFinishIn1, INFINITE);
            WaitForSingleObject(Event_InputFinishIn4, INFINITE);
            break;
        case 4:
            SetEvent(Event_InputFinishIn4);
            WaitForSingleObject(Event_InputFinishIn1, INFINITE);
            WaitForSingleObject(Event_InputFinishIn3, INFINITE);
            break;
        default:
            WaitForSingleObject(Event_InputFinishIn1, INFINITE);
            WaitForSingleObject(Event_InputFinishIn3, INFINITE);
            WaitForSingleObject(Event_InputFinishIn4, INFINITE);
            break;
    }

    // Calculation1
    unsigned int xi = 32500;
    for (unsigned int i = low; i < high; i++) {
        if (Z[i] < xi) {
            xi = Z[i];
        }
    }

    // Contribute to global min
    EnterCriticalSection(&CriticalSection_UpdateX);
    x = (xi < x) ? xi : x;
    LeaveCriticalSection(&CriticalSection_UpdateX);

    // Signal about the end of Calculations1
    ReleaseSemaphore(Semaphores_Calculation1EndIn[tid - 1], P, NULL);

    // Wait for the end of Calculations1 in other threads
    for (unsigned int i = 0; i < P; i++) {
        WaitForSingleObject(Semaphores_Calculation1EndIn[i], INFINITE);
    }

    // Copy
    WaitForSingleObject(Mutex_CopyMc, INFINITE);
    const Matrix MCi = MC.copy();
    ReleaseMutex(Mutex_CopyMc);

    EnterCriticalSection(&CriticalSection_CopyX);
    xi = x;
    LeaveCriticalSection(&CriticalSection_CopyX);

    // Calculations2
    for (unsigned int h = low; h < high; h++) {
        for (unsigned int i = 0; i < N; i++) {
            int elem = 0;
            for (unsigned int j = 0; j < N; j++) {
                elem += MB[h][j] * MCi[j][i];
            }
            MA[h][i] = elem + xi * MK[h][i];
        }
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
            break;
    }

    if (tid == 1) {
        // Output MA
        if (N <= 8) {
            outputMatrix(MA);
        }
    }

    if (DO_PRINT) {
        std::cout << "Thread " << tid << " finished." << std::endl;
    }
};
//-----------------------------------------------------------------------------
int main() {
    // Preparations
    fillMatrix(MA, 0);

    // Semaphores
    for (unsigned int i = 0; i < P; i++) {
        Semaphores_Calculation1EndIn[i] = CreateSemaphore(
                NULL,   // default security attributes
                0,      // initial count
                P,      // maximum count
                NULL    // unnamed semaphore
                );
    }

    // Mutexes
    Mutex_CopyMc = CreateMutex(
            NULL,   // default security attributes
            FALSE,  // initially not owned
            NULL    // unnamed
            );

    // Critical Sections
    InitializeCriticalSection(&CriticalSection_UpdateX);
    InitializeCriticalSection(&CriticalSection_CopyX);

    // Events
    // manual reset
    Event_InputFinishIn1 = CreateEvent(
            NULL,   // default security attibutes
            TRUE,   // manual reset
            FALSE,  // initial - not signaled
            NULL    // unnamed event
            );
    Event_InputFinishIn3 = CreateEvent(NULL, TRUE, FALSE, NULL);
    Event_InputFinishIn4 = CreateEvent(NULL, TRUE, FALSE, NULL);
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

void outputMatrix(const Matrix& matrix) {
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
