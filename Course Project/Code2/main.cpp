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
const unsigned int L = 3;
const unsigned int N = L * L * 2;
const unsigned int P = L * L;
const unsigned int H = N / P;
const unsigned int STACK_SIZE = 100000000;
const bool DO_PRINT = true;
const unsigned int ALL_H = P;
const bool DIRECT = true;
const bool REVERSED = !DIRECT;
const bool VECTOR = true;
const bool MATRIX = !VECTOR;


// Types
struct Vector {
    public:
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

        int& operator[](unsigned int index) {
            return elements[index];
        }

        const int& operator[](unsigned int index) const {
            return elements[index];
        }

        void* getVoidPtr() {
            return (void*)(elements);
        }
};


struct Matrix {
    public:
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

        int& operator[](unsigned int index) {
            return elements[index];
        }

        const int& operator[](unsigned int index) const {
            return elements[index];
        }

        void* getVoidPtr() {
            return (void*)(elements);
        }
};


// Functions
void fillVector(Vector& vector, unsigned int value);
void fillMatrix(Matrix& matrix, unsigned int value);
void outputVector(const Vector& vector);
void outputMatrix(const Matrix& matrix);
unsigned int getReceivedSizeInHs(bool isDirect, unsigned int row, unsigned int column);
unsigned int getSizeFromHs(bool isVector, unsigned int sizeHs);
unsigned int getRank(unsigned int row, unsigned int column);


// Data
// int e, d;
// Vector A, B, C, X, S, Z;
// Matrix MR, MT;
//-----------------------------------------------------------------------------
void ThreadFunction(unsigned int row, unsigned int column) {
    // Data
    int e;//, d;
    Vector A(1);
    Vector C(1);
    Matrix MR(1);
    Vector B(1);
    Vector S(ALL_H);
    Vector X(1);
    Vector Z(1);
    Matrix MT(ALL_H);

    const unsigned int receivedDirectSizeHs = getReceivedSizeInHs(DIRECT, row, column);
    const unsigned int receivedReversedSizeHs = getReceivedSizeInHs(!DIRECT, row, column);
    const unsigned int receivedDirectVectorSize = getSizeFromHs(VECTOR, receivedDirectSizeHs);
    const unsigned int receivedDirectMatrixSize = getSizeFromHs(MATRIX, receivedDirectSizeHs);
    const unsigned int receivedReversedVectorSize = getSizeFromHs(VECTOR, receivedReversedSizeHs);
    // const unsigned int receivedReversedMatrixSize = getSizeFromHs(MATRIX, receivedReversedSizeHs);
    const unsigned int bufferReceivedDirectSize =
        1 +                        // e
        receivedDirectVectorSize + // C
        receivedDirectMatrixSize;  // MR
    const unsigned int bufferReceivedReversedSize =
        N +                          // S
        N * N +                      // MT
        receivedReversedVectorSize + // B
        receivedReversedVectorSize + // X
        receivedReversedVectorSize;  // Z
    int* bufferReceivedDirect = new int[bufferReceivedDirectSize];
    int* bufferReceivedReversed = new int[bufferReceivedReversedSize];

//-----------------------------------------------------------------------------
    // 1. Input
    if (row == 0 && column == 0) {
        int e_in;
        Vector C_in(ALL_H);
        Matrix MR_in(ALL_H);

        e_in = 1;
        fillVector(C_in, 1);
        fillMatrix(MR_in, 1);

        // Because they don't have receives
        e = e_in;
        memcpy(C.getVoidPtr(), C_in.getVoidPtr(), H * sizeof(int));
        memcpy(MR.getVoidPtr(), MR_in.getVoidPtr(), H * N * sizeof(int));

        bufferReceivedDirect[0] = e_in;
        memcpy((void*) (bufferReceivedDirect + 1), C_in.getVoidPtr(), N * sizeof(int));
        memcpy((void*) (bufferReceivedDirect + 1 + C_in.size), MR_in.getVoidPtr(), N * N * sizeof(int));
    } else if (row == L - 1 && column == L - 1) {
        Vector S_in(ALL_H);
        Matrix MT_in(ALL_H);
        Vector B_in(ALL_H);
        Vector X_in(ALL_H);
        Vector Z_in(ALL_H);

        fillVector(S_in, 1);
        fillMatrix(MT_in, 1);
        fillVector(B_in, 1);
        fillVector(X_in, 1);
        fillVector(Z_in, 1);

        // Because they don't have receives
        memcpy(S.getVoidPtr(), S_in.getVoidPtr(), N * sizeof(int));
        memcpy(MT.getVoidPtr(), MT_in.getVoidPtr(), N * N * sizeof(int));
        memcpy(B.getVoidPtr(), (void*)(B_in.elements + B_in.size - H), H * sizeof(int));
        memcpy(X.getVoidPtr(), (void*)(X_in.elements + X_in.size - H), H * sizeof(int));
        memcpy(Z.getVoidPtr(), (void*)(Z_in.elements + Z_in.size - H), H * sizeof(int));

        memcpy((void*)
                (bufferReceivedReversed),
                S_in.getVoidPtr(), N * sizeof(int));
        memcpy((void*)
                (bufferReceivedReversed + S_in.size),
                MT_in.getVoidPtr(), N * N * sizeof(int));
        memcpy((void*)
                (bufferReceivedReversed + S_in.size + MT_in.size),
                B_in.getVoidPtr(), N * sizeof(int));
        memcpy((void*)
                (bufferReceivedReversed + S_in.size + MT_in.size + B_in.size),
                X_in.getVoidPtr(), N * sizeof(int));
        memcpy((void*)
                (bufferReceivedReversed + S_in.size + MT_in.size + B_in.size + X_in.size),
                Z_in.getVoidPtr(), N * sizeof(int));
    }

//-----------------------------------------------------------------------------
    // 2. Horizontal waves
    if (row == 0) {
        // Receive
        if (column >= 1 && column <= L - 1) {
            MPI_Recv(bufferReceivedDirect, bufferReceivedDirectSize, MPI_INT, getRank(row, column - 1),
                    MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            e = bufferReceivedDirect[0];
            memcpy(C.getVoidPtr(),
                    (void*)(bufferReceivedDirect + 1),
                    C.size * sizeof(int));
            memcpy(MR.getVoidPtr(),
                    (void*)(bufferReceivedDirect + 1 + receivedDirectVectorSize),
                    MR.size * sizeof(int));
        }

        // Send
        if (column >= 0 && column <= L - 2) {
            const unsigned int sentDirectHorSizeHs = getReceivedSizeInHs(DIRECT, row, column + 1);
            const unsigned int bufferSentDirectHorSize =
                1 +
                getSizeFromHs(VECTOR, sentDirectHorSizeHs) + // C
                getSizeFromHs(MATRIX, sentDirectHorSizeHs); // MR
            int* bufferSentDirectHor = new int[bufferSentDirectHorSize];

            const unsigned int sentDirectHorVectorSize =
                getSizeFromHs(VECTOR, sentDirectHorSizeHs);
            const unsigned int sentDirectHorMatrixSize =
                getSizeFromHs(MATRIX, sentDirectHorSizeHs);

            bufferSentDirectHor[0] = e;
            memcpy((void*)(bufferSentDirectHor + 1),
                    (void*)(bufferReceivedDirect + 1 + L * C.size),
                    sentDirectHorVectorSize * sizeof(int));
            memcpy((void*)(bufferSentDirectHor + 1 + sentDirectHorVectorSize),
                    (void*)(bufferReceivedDirect + 1 + receivedDirectVectorSize + L * MR.size),
                    sentDirectHorMatrixSize * sizeof(int));

            MPI_Send(bufferSentDirectHor, bufferSentDirectHorSize, MPI_INT, getRank(row, column + 1),
                    0, MPI_COMM_WORLD);

            delete[] bufferSentDirectHor;
        }
    } else if (row == L - 1) {
        // Receive
        if (0 <= column && column <= L - 2) {
            MPI_Recv(bufferReceivedReversed, bufferReceivedReversedSize, MPI_INT, getRank(row, column + 1),
                    MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int* currPtr = bufferReceivedReversed;
            memcpy(S.getVoidPtr(),
                    (void*)(currPtr),
                    S.size * sizeof(int));
            currPtr += S.size;
            memcpy(MT.getVoidPtr(),
                    (void*)(currPtr),
                    MT.size * sizeof(int));
            currPtr += MT.size;
            currPtr += receivedReversedVectorSize - H; // cut from the end
            memcpy(B.getVoidPtr(),
                    (void*)(currPtr),
                    B.size * sizeof(int));
            currPtr += receivedReversedVectorSize;
            memcpy(X.getVoidPtr(),
                    (void*)(currPtr),
                    X.size * sizeof(int));
            currPtr += receivedReversedVectorSize;
            memcpy(Z.getVoidPtr(),
                    (void*)(currPtr),
                    Z.size * sizeof(int));
        }

        // Send
        if (1 <= column && column <= L - 1) {
            const unsigned int sentReversedHorSizeHs = getReceivedSizeInHs(REVERSED, row, column - 1);
            const unsigned int bufferSentReversedHorSize =
                N +                                            // S
                N * N +                                        // MT
                getSizeFromHs(VECTOR, sentReversedHorSizeHs) + // B
                getSizeFromHs(VECTOR, sentReversedHorSizeHs) + // X
                getSizeFromHs(VECTOR, sentReversedHorSizeHs);  // Z

            int* bufferSentReversedHor = new int[bufferSentReversedHorSize];

            const unsigned int sentReversedHorVectorSize =
                getSizeFromHs(VECTOR, sentReversedHorSizeHs);

            int* currPtrDest = bufferSentReversedHor;
            int* currPtrSrc = bufferReceivedReversed;
            memcpy((void*)(currPtrDest),
                    (void*)(currPtrSrc),
                    N * sizeof(int));
            currPtrDest += N;
            currPtrSrc += N;
            memcpy((void*)(currPtrDest),
                    (void*)(currPtrSrc),
                    N * N * sizeof(int));
            currPtrDest += N * N;
            currPtrSrc += N * N;
            memcpy((void*)(currPtrDest),
                    (void*)(currPtrSrc),
                    sentReversedHorVectorSize * sizeof(int));
            currPtrDest += sentReversedHorVectorSize;
            currPtrSrc += receivedReversedVectorSize;
            memcpy((void*)(currPtrDest),
                    (void*)(currPtrSrc),
                    sentReversedHorVectorSize * sizeof(int));
            currPtrDest += sentReversedHorVectorSize;
            currPtrSrc += receivedReversedVectorSize;
            memcpy((void*)(currPtrDest),
                    (void*)(currPtrSrc),
                    sentReversedHorVectorSize * sizeof(int));

            MPI_Send(bufferSentReversedHor, bufferSentReversedHorSize, MPI_INT, getRank(row, column - 1),
                    0, MPI_COMM_WORLD);

            delete[] bufferSentReversedHor;
        }
    }

//-----------------------------------------------------------------------------
    // 3. Vertical waves
    if (row == 0) {
        // Send direct
        MPI_Request request;
        int* bufferSentDirect;
        {
            const unsigned int sentDirectSizeHs = getReceivedSizeInHs(DIRECT, row + 1, column);
            const unsigned int sentDirectVectorSize = getSizeFromHs(VECTOR, sentDirectSizeHs);
            const unsigned int sentDirectMatrixSize = getSizeFromHs(MATRIX, sentDirectSizeHs);
            const unsigned int bufferSentDirectSize =
                1 +                    // e
                sentDirectVectorSize + // C
                sentDirectMatrixSize;  // MR
            bufferSentDirect = new int[bufferSentDirectSize];

            bufferSentDirect[0] = e;
            memcpy((void*)(bufferSentDirect + 1),
                    (void*)(bufferReceivedDirect + 1 + C.size),
                    sentDirectVectorSize * sizeof(int));
            memcpy((void*)(bufferSentDirect + 1 + sentDirectVectorSize),
                    (void*)(bufferReceivedDirect + 1 + receivedDirectVectorSize + MR.size),
                    sentDirectMatrixSize * sizeof(int));

            MPI_Isend(bufferSentDirect, bufferSentDirectSize, MPI_INT, getRank(row + 1, column),
                    0, MPI_COMM_WORLD, &request);
        }

        // Receive reversed
        {
            MPI_Recv(bufferReceivedReversed, bufferReceivedReversedSize, MPI_INT, getRank(row + 1, column),
                    MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int* currPtr = bufferReceivedReversed;
            memcpy(S.getVoidPtr(),
                    (void*)(currPtr),
                    S.size * sizeof(int));
            currPtr += S.size;
            memcpy(MT.getVoidPtr(),
                    (void*)(currPtr),
                    MT.size * sizeof(int));
            currPtr += MT.size;
            currPtr += receivedReversedVectorSize - H; // cut from the end
            memcpy(B.getVoidPtr(),
                    (void*)(currPtr),
                    B.size * sizeof(int));
            currPtr += receivedReversedVectorSize;
            memcpy(X.getVoidPtr(),
                    (void*)(currPtr),
                    X.size * sizeof(int));
            currPtr += receivedReversedVectorSize;
            memcpy(Z.getVoidPtr(),
                    (void*)(currPtr),
                    Z.size * sizeof(int));
        }

        // Cleanup
        MPI_Wait(&request, MPI_STATUS_IGNORE);
        delete[] bufferSentDirect;
//-----------------------------------------------------------------------------
    } else if (row == L - 1) {
        // Send reversed
        MPI_Request request;
        int* bufferSentReversed;
        {
            const unsigned int sentReversedSizeHs = getReceivedSizeInHs(REVERSED, row - 1, column);
            const unsigned int sentReversedVectorSize = getSizeFromHs(VECTOR, sentReversedSizeHs);
            const unsigned int bufferSentReversedSize =
                N +                    // S
                N * N +                // MT
                sentReversedVectorSize + // B
                sentReversedVectorSize + // X
                sentReversedVectorSize;  // Z
            int* bufferSentReversed = new int[bufferSentReversedSize];

            const unsigned int sentToLeftOfVector =
                column == 0 ? 0 : H * getReceivedSizeInHs(REVERSED, row, column - 1);
            memcpy((void*)(bufferSentReversed),
                    (void*)(bufferReceivedReversed),
                    N * sizeof(int));
            memcpy((void*)(bufferSentReversed + N),
                    (void*)(bufferReceivedReversed + N),
                    N * N * sizeof(int));
            memcpy((void*)(bufferSentReversed + N + N * N),
                    (void*)(bufferReceivedReversed + N + N * N + sentToLeftOfVector),
                    sentReversedVectorSize * sizeof(int));
            memcpy((void*)(bufferSentReversed + N + N * N + 1 * sentReversedVectorSize),
                    (void*)(bufferReceivedReversed + N + N * N + 1 * receivedReversedVectorSize + sentToLeftOfVector),
                    sentReversedVectorSize * sizeof(int));
            memcpy((void*)(bufferSentReversed + N + N * N + 2 * sentReversedVectorSize),
                    (void*)(bufferReceivedReversed + N + N * N + 2 * receivedReversedVectorSize + sentToLeftOfVector),
                    sentReversedVectorSize * sizeof(int));

            MPI_Isend(bufferSentReversed, bufferSentReversedSize, MPI_INT, getRank(row - 1, column),
                    0, MPI_COMM_WORLD, &request);
        }

        // Receive direct
        {
            MPI_Recv(bufferReceivedDirect, bufferReceivedDirectSize, MPI_INT, getRank(row - 1, column),
                    MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            e = bufferReceivedDirect[0];
            memcpy(C.getVoidPtr(),
                    (void*)(bufferReceivedDirect + 1),
                    C.size * sizeof(int));
            memcpy(MR.getVoidPtr(),
                    (void*)(bufferReceivedDirect + 1 + receivedDirectVectorSize),
                    MR.size * sizeof(int));
        }

        // Cleanup
        MPI_Wait(&request, MPI_STATUS_IGNORE);
        delete[] bufferSentReversed;
//-----------------------------------------------------------------------------
    } else {
        const unsigned int center = L / 2;
        unsigned int switchCode;
        if (row < center) {
            // Receive direct
            // Send direct
            // Receive reversed
            // Send reversed
            switchCode = 2031;
        } else if (row > center) {
            // Receive reversed
            // Send reversed
            // Receive direct
            // Send direct
            switchCode = 3120;
        } else { // == center
            // Receive reversed
            // Receive direct
            // Send reversed
            // Send direct
            switchCode  = 3210;
        }

        for (int switcher = switchCode; switcher > 0; switcher /= 10) {
            switch (switcher % 10) {
                case 3: // Send direct
                    {
                        const unsigned int sentDirectSizeHs = getReceivedSizeInHs(DIRECT, row + 1, column);
                        const unsigned int sentDirectVectorSize = getSizeFromHs(VECTOR, sentDirectSizeHs);
                        const unsigned int sentDirectMatrixSize = getSizeFromHs(MATRIX, sentDirectSizeHs);
                        const unsigned int bufferSentDirectSize =
                            1 +                    // e
                            sentDirectVectorSize + // C
                            sentDirectMatrixSize;  // MR
                        int* bufferSentDirect = new int[bufferSentDirectSize];

                        bufferSentDirect[0] = e;
                        memcpy((void*)(bufferSentDirect + 1),
                                (void*)(bufferReceivedDirect + 1 + C.size),
                                sentDirectVectorSize * sizeof(int));
                        memcpy((void*)(bufferSentDirect + 1 + sentDirectVectorSize),
                                (void*)(bufferReceivedDirect + 1 + receivedDirectVectorSize + MR.size),
                                sentDirectMatrixSize * sizeof(int));

                        MPI_Send(bufferSentDirect, bufferSentDirectSize,
                                MPI_INT, getRank(row + 1, column),
                                0, MPI_COMM_WORLD);

                        delete[] bufferSentDirect;
                    }
                    break;
//-----------------------------------------------------------------------------
                case 2: // Send reversed
                    {
                        const unsigned int sentReversedSizeHs =
                            getReceivedSizeInHs(REVERSED, row - 1, column);
                        const unsigned int sentReversedVectorSize =
                            getSizeFromHs(VECTOR, sentReversedSizeHs);
                        const unsigned int bufferSentReversedSize =
                            N +                    // S
                            N * N +                // MT
                            sentReversedVectorSize + // B
                            sentReversedVectorSize + // X
                            sentReversedVectorSize;  // Z
                        int* bufferSentReversed = new int[bufferSentReversedSize];

                        memcpy((void*)(bufferSentReversed),
                                (void*)(bufferReceivedReversed),
                                N * sizeof(int));
                        memcpy((void*)(bufferSentReversed + N),
                                (void*)(bufferReceivedReversed + N),
                                N * N * sizeof(int));
                        memcpy((void*)(bufferSentReversed + N + N * N),
                                (void*)(bufferReceivedReversed + N + N * N),
                                sentReversedVectorSize * sizeof(int));
                        memcpy((void*)(bufferSentReversed + N + N * N + 1 * sentReversedVectorSize),
                                (void*)(bufferReceivedReversed + N + N * N + 1 * receivedReversedVectorSize),
                                sentReversedVectorSize * sizeof(int));
                        memcpy((void*)(bufferSentReversed + N + N * N + 2 * sentReversedVectorSize),
                                (void*)(bufferReceivedReversed + N + N * N + 2 * receivedReversedVectorSize),
                                sentReversedVectorSize * sizeof(int));

                        MPI_Send(bufferSentReversed, bufferSentReversedSize,
                                MPI_INT, getRank(row - 1, column),
                                0, MPI_COMM_WORLD);

                        delete[] bufferSentReversed;
                    }
                    break;
//-----------------------------------------------------------------------------
                case 1: // Receive direct
                    {
                        MPI_Recv(bufferReceivedDirect, bufferReceivedDirectSize,
                                MPI_INT, getRank(row - 1, column),
                                MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        e = bufferReceivedDirect[0];
                        memcpy(C.getVoidPtr(),
                                (void*)(bufferReceivedDirect + 1),
                                C.size * sizeof(int));
                        memcpy(MR.getVoidPtr(),
                                (void*)(bufferReceivedDirect + 1 + receivedDirectVectorSize),
                                MR.size * sizeof(int));
                    }
                    break;
//-----------------------------------------------------------------------------
                case 0: // Receive reversed
                    {
                        MPI_Recv(bufferReceivedReversed, bufferReceivedReversedSize,
                                MPI_INT, getRank(row + 1, column),
                                MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        int* currPtr = bufferReceivedReversed;
                        memcpy(S.getVoidPtr(),
                                (void*)(currPtr),
                                S.size * sizeof(int));
                        currPtr += S.size;
                        memcpy(MT.getVoidPtr(),
                                (void*)(currPtr),
                                MT.size * sizeof(int));
                        currPtr += MT.size;
                        currPtr += receivedReversedVectorSize - H; // cut from the end
                        memcpy(B.getVoidPtr(),
                                (void*)(currPtr),
                                B.size * sizeof(int));
                        currPtr += receivedReversedVectorSize;
                        memcpy(X.getVoidPtr(),
                                (void*)(currPtr),
                                X.size * sizeof(int));
                        currPtr += receivedReversedVectorSize;
                        memcpy(Z.getVoidPtr(),
                                (void*)(currPtr),
                                Z.size * sizeof(int));
                    }
                    break;
                default:
                    break;
            }
        }
    }
//-----------------------------------------------------------------------------

    // if (row == L - 1 && column == 0) {
        // std::cout << row << ":" << column << ": ";
        outputVector(S);
        // outputMatrix(MR);
    // }

    // Data in the received buffers is no longer needed
    delete[] bufferReceivedDirect;
    delete[] bufferReceivedReversed;


    // 4.
}



            // MPI_Request request;
            // MPI_Send(buffer, totalSize, MPI_INT, getRank(row, column + 1),
            //         0, MPI_COMM_WORLD, &request);
            // MPI_Request_free(&request);
            //
            //
    //     MPI_Isend(a.getPrt(), H, MPI_INT, getRank(1, 1), 0, MPI_COMM_WORLD, &request);
    // } else if (row <= 1 && column <= 1){
    //     MPI_Recv(a.getPrt(), H, MPI_INT, getRank(0, 0), MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);




//-----------------------------------------------------------------------------


int main() {
    int size, rank;
    MPI_Init(0, 0);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const unsigned int row = rank / L;
    const unsigned int column = rank % L;

    if (row == 0 && column == 0) {
        std::cout << ":> Started" << std::endl;
    }

    ThreadFunction(row, column);

    MPI_Finalize();
    // std::cout << ":> Finished " << rank << std::endl;

    return 0;







    // if (row == 2 && column == 3) {
    //     int counter = 0;
    //     for (int* buf = bufferReceivedReversed + N + N*N + 0*(4); counter < 32; counter++, buf++) {
    //         std::cout << *buf << std::endl;
    //     }
    // }


    // Preparations
    // fillVector(A, 0);


    // const time_t start = clock();
    //
    // const time_t finish = clock() - start;
    // std::cout << "Elapsed time = " << static_cast<double>(finish) / 1000.0
    //           << std::endl;
    //
    //
    //
    //
}


//-----------------------------------------------------------------------------
// Function definitions
void fillVector(Vector& vector, unsigned int value) {
    for (unsigned int i = 0; i < vector.size; i++) {
        // vector[i] = value;
        vector[i] = i;
    }
}

void fillMatrix(Matrix& matrix, unsigned int value) {
    for (unsigned int i = 0; i < matrix.size; i++) {
        // matrix[i] = value;
        matrix[i] = i;
    }

    // for (unsigned int i = 0; i < matrix.hs * N; i++) {
    //     for (unsigned int j = 0; j < N; j++) {
    //         matrix[i * N + j] = value;
    //     }
    // }
}

void outputVector(const Vector& vector) {
    for (unsigned int i = 0; i < vector.size; i++) {
        std::cout << vector[i] << " ";
    }
    std::cout << std::endl;
}

void outputMatrix(const Matrix& matrix) {
    for (unsigned int i = 0; i < matrix.hs * H; i++) {
        for (unsigned int j = 0; j < N; j++) {
            std::cout << matrix[i * N + j] << " ";
        }
        std::cout << std::endl;
    }
    // std::cout << std::endl;
}

unsigned int getReceivedSizeInHs(bool direct, unsigned int row, unsigned int column) {
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

unsigned int getSizeFromHs(bool isVector, unsigned int sizeHs) {
    return sizeHs * H * (isVector ? 1 : N);
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
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
    // const int nodes = L * L;
    // int neigh[nodes];
    // int indices[nodes];
    // int edges[2*4+3*4+2];
    //
    // neigh[getRank(0,0)] = 2;
    // neigh[getRank(0,1)] = 3;
    // neigh[getRank(0,2)] = 2;
    // neigh[getRank(1,0)] = 3;
    // neigh[getRank(1,1)] = 4;
    // neigh[getRank(1,2)] = 2;
    // neigh[getRank(2,0)] = 2;
    // neigh[getRank(2,1)] = 2;
    // neigh[getRank(2,2)] = 0;
    //
    // indices[0] = neigh[0];
    // for (unsigned int i = 1; i < nodes; i++) {
    //     indices[i] = indices[i-1] + neigh[i];
    // }
    //
    // for (unsigned int row = 0; row < L; row++) {
    //     for (unsigned int column = 0; column < L; column++) {
    //         unsigned int ed = 0;
    //         if (row > 0) {
    //             if (!(row == 2 && column == 2))
    //             edges[ed++] = getRank(row - 1, column);
    //         }
    //         if (row < L - 1) {
    //             if (!(row == 1 && column == 2))
    //             edges[ed++] = getRank(row + 1, column);
    //         }
    //         if (column > 0) {
    //             if (!(row == 2 && column == 2))
    //             edges[ed++] = getRank(row, column - 1);
    //         }
    //         if (column < L - 1) {
    //             if (!(row == 2 && column == 1))
    //             edges[ed++] = getRank(row, column + 1);
    //         }
    //     }
    // }
    //
    //
    // MPI_Comm commun;
    // MPI_Graph_create(MPI_COMM_WORLD, nodes, indices, edges, false, &commun);
    //
    // Vector a(ALL_H);
    // if (row == 0 && column == 0) {
    //     fillVector(a, 2);
    //     MPI_Request request;
    //     MPI_Isend(a.getPrt(), N, MPI_INT, getRank(2, 2), 0, commun, &request);
    // } else if (row == 2 && column == 2) {
    //     MPI_Recv(a.getPrt(), N, MPI_INT, getRank(0, 0), MPI_ANY_TAG, commun, MPI_STATUS_IGNORE);
    //     a[0] = 0;
    //     outputVector(a);
    // }
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    // Vector a(1);
    // if (row == 0 && column == 0) {
    //     fillVector(a, 2);
    //
    //     MPI_Request request;
    //     MPI_Isend(a.getPrt(), H, MPI_INT, getRank(0, 1), 0, MPI_COMM_WORLD, &request);
    //     MPI_Isend(a.getPrt(), H, MPI_INT, getRank(1, 0), 0, MPI_COMM_WORLD, &request);
    //     MPI_Isend(a.getPrt(), H, MPI_INT, getRank(1, 1), 0, MPI_COMM_WORLD, &request);
    // } else if (row <= 1 && column <= 1){
    //     MPI_Recv(a.getPrt(), H, MPI_INT, getRank(0, 0), MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //
    //     a[0] += getRank(row, column);
    //
    //     outputVector(a);
    // }
