//-----------------------------------------------------------------------------
// Lab3: C#. Semaphores, Events, Mutexes, Locks, Monitors
// Task: MA = max(Z) * (MO * MT) + min(Z) * MS
// Author: Igor Boyarshin
// Date: 18.03.2018
//-----------------------------------------------------------------------------

using System;
using System.Threading;


public class Lab3 {

    // Constants
    private static readonly int N = 600;
    private static readonly int P = 6;
    private static readonly int H = N / P;
    private static readonly int outputThreshold = 12;
    private static readonly bool doOutput = true;

    // Data
    private int a;
    private volatile int b;
    private Vector Z;
    private Matrix MO, MT, MS, MA;

    // Semaphores
    private static Semaphore[] Semaphore_Calculation1EndIn;

    // Mutexes
    private static Mutex Mutex_SetA;
    // private static Mutex Mutex_SetB;

    // Locks
    private static readonly object Lock_CopyMt = new object();

    // Events
    private static EventWaitHandle Event_InputFinishIn1;
    private static EventWaitHandle Event_InputFinishIn3;
    private static EventWaitHandle Event_InputFinishIn4;
    private static EventWaitHandle[] Event_Calculation2EndIn;

    // Monitors
    private static object Monitor_SetAb = new object();

//-----------------------------------------------------------------------------

    public Lab3(int N) {
        // Preparations
        MA = new Matrix(N, 0);
        b = 0;
        a = 400000000;

        // Semaphores
        Semaphore_Calculation1EndIn = new Semaphore[P];
        for (int i = 0; i < P; i++) {
            Semaphore_Calculation1EndIn[i] = new Semaphore(0, P);
        }

        // Mutexes
        Mutex_SetA = new Mutex(false);

        // Events
        // manual
        Event_InputFinishIn1 = new EventWaitHandle(false, EventResetMode.ManualReset);
        Event_InputFinishIn3 = new EventWaitHandle(false, EventResetMode.ManualReset);
        Event_InputFinishIn4 = new EventWaitHandle(false, EventResetMode.ManualReset);
        // automatic
        Event_Calculation2EndIn = new EventWaitHandle[P];
        for (int i = 0; i < P; i++) {
            Event_Calculation2EndIn[i] =
                new EventWaitHandle(false, EventResetMode.AutoReset);
        }

        // Threads
        Thread[] threads = new Thread[P];


//-----------------------------------------------------------------------------
        for (int thread = 0; thread < P; thread++) {
            int index = thread;
            threads[thread] = new Thread(() => {
                int tid = index + 1;
                int low = (tid - 1) * H;
                int high = tid * H;

                if (doOutput) {
                    Console.WriteLine(":> Thread " + tid + " started!");
                }

                // 1. Input
                switch (tid) {
                case 1:
                    Z = FillVectorOnes();
                    break;
                case 3:
                    MO = FillMatrixOnes();
                    MS = FillMatrixOnes();
                    break;
                case 4:
                    MT = FillMatrixOnes();
                    break;
                default:
                    break;
                }

                // 2. Signal T1..6 about input finish
                switch (tid) {
                case 1:
                    Event_InputFinishIn1.Set();
                    break;
                case 3:
                    Event_InputFinishIn3.Set();
                    break;
                case 4:
                    Event_InputFinishIn4.Set();
                    break;
                default:
                    break;
                }

                // 3. Wait for input finish in T1, T3, T4
                Event_InputFinishIn1.WaitOne();
                Event_InputFinishIn3.WaitOne();
                Event_InputFinishIn4.WaitOne();

                // 4. Copy MTi = MT
                Matrix MTi;
                lock (Lock_CopyMt) {
                    MTi = MT.copy();
                }

                // 5.
                int ai = FindMin(ref Z, low, high);

                // 6.
                Mutex_SetA.WaitOne();
                a = Min(a, ai);
                Mutex_SetA.ReleaseMutex();

                // 7.
                int bi = FindMax(ref Z, low, high);

                // 8.
                b = Max(b, bi);

                // 9. Signal T1..6 about Calculation1 finish
                Semaphore_Calculation1EndIn[tid - 1].Release(P);

                // 10. Wait for Calculation1 finish in T1..6
                for (int i = 0; i < P; i++) {
                    Semaphore_Calculation1EndIn[i].WaitOne();
                }

                // 11. Copy a, b
                Monitor.Enter(Monitor_SetAb);
                try {
                    ai = a;
                    bi = b;
                } finally {
                    Monitor.Exit(Monitor_SetAb);
                }

                // 12. Calculation2
                for (int i = low; i < high; i++) {
                    for (int j = 0; j < N; j++) {
                        int product = 0;
                        for (int k = 0; k < N; k++) {
                            product += MO[i, k] * MTi[k, j];
                        }
                        MA[i, j] = bi * product + ai * MS[i, j];
                    }
                }

                // 13. Wait for Calculation2 finish in T1..6
                Event_Calculation2EndIn[tid - 1].Set();
                switch (tid) {
                case 1:
                    for (int i = 0; i < P; i++) {
                        Event_Calculation2EndIn[i].WaitOne();
                    }
                    break;
                default:
                    break;
                }

                // 14. Output
                switch (tid) {
                case 1:
                    if (N <= outputThreshold) {
                        OutputMatrix(ref MA);
                    }
                    break;
                default:
                    break;
                }

                if (doOutput) {
                    Console.WriteLine(":> Finished Thread " + tid);
                }
            });
        }
//-----------------------------------------------------------------------------

        for (int i = 0; i < 6; i++) {
            threads[i].Start();
        }
        for (int i = 0; i < 6; i++) {
            threads[i].Join();
        }
    }

//-----------------------------------------------------------------------------

    public void OutputMatrix(ref Matrix matrix) {
        for (int row = 0; row < N; row++) {
            for (int column = 0; column < N; column++) {
                Console.Write(matrix[row, column] + " ");
            }
            Console.WriteLine();
        }
    }

    public Vector FillVectorOnes() {
        return new Vector(N, 1);
    }

    public Matrix FillMatrixOnes() {
        return new Matrix(N, 1);
    }

    private int Min(int a, int b) {
        return a < b ? a : b;
    }

    private int Max(int a, int b) {
        return a > b ? a : b;
    }

    private int FindMin(ref Vector v, int low, int high) {
        return FindMinOrMax(ref v, low, high, true);
    }

    private int FindMax(ref Vector v, int low, int high) {
        return FindMinOrMax(ref v, low, high, false);
    }

    private int FindMinOrMax(ref Vector v, int low, int high, bool findMinElement) {
        int current = v[low];
        for (int i = low; i < high; i++) {
            int element = v[i];
            if (findMinElement ?
                    (element < current) : (element > current)) {
                current = element;
            }
        }

        return current;
    }

//-----------------------------------------------------------------------------

    public static void Main(String[] args) {
        // Console.WriteLine(":> Main program started!");
        new Lab3(N);
    }
}
