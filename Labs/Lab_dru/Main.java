//-----------------------------------------------------------------------------
// Java. Monitors
// A = a * max(Z) * T + sort(E) * (MO * MK)
// By Andrei Vasilyna
// Date: 13.05.2018
//-----------------------------------------------------------------------------

import java.util.List;
import java.util.ArrayList;

public class Main {
    private static final int N = 3000;
    private static final int P = 4;
    private static final int H = N / P;
    private static final int OUTPUT_THRESHOLD = 8;

    private static final MonitorResources monitorResources
        = new MonitorResources();
    private static final MonitorSynchronization monitorSynchronization
        = new MonitorSynchronization();
    private static final MonitorSort monitorSort2
        = new MonitorSort();
    private static final MonitorSort monitorSort3
        = new MonitorSort();
    private static final MonitorSort monitorSort4
        = new MonitorSort();

    private Vector A = new Vector(N, 0);
    private Vector Z;
    private Vector T;
    private Vector E;
    private Matrix MK;

    public class MyThread extends Thread {
        private final int tid;
        private final int start;
        private final int finish;

        public MyThread(int tid, int start, int finish) {
            this.tid = tid;
            this.start = start;
            this.finish = finish;
        }

        @Override
        public void run() {
            System.out.println("Starting Thread " + tid);

            // Input
            switch (tid) {
                case 1:
                    Z = fillVectorOnes();
                    MK = fillMatrixOnes();
                    break;
                case 3:
                    T = fillVectorOnes();
                    E = fillVectorOnes();
                    break;
                case 4:
                    final Matrix MO = fillMatrixOnes();
                    final int a = 1;
                    monitorResources.setMO(MO);
                    monitorResources.setA(a);
                    break;
                default:
                    break;
            }

            // Sync on input
            if (tid != 2) {
                monitorSynchronization.signalInputEnd();
            }
            monitorSynchronization.waitInputEnd();

            // Find max
            int xi = Integer.MIN_VALUE;
            for (int i = start; i < finish; i++) {
                if (Z.get(i) > xi) {
                    xi = Z.get(i);
                }
            }
            monitorResources.updateX(xi);

            // Sort Z_H
            E.sort(start, finish);
            if (tid == 2) {
                monitorSort2.signalSorted();
            } else if (tid == 4) {
                monitorSort4.signalSorted();
            }

            if (tid == 1) {
                monitorSort2.waitSorted();
            } else if (tid == 3) {
                monitorSort4.waitSorted();
            }

            // Sort Z_2h
            if (tid == 1 || tid == 3) {
                E.merge(start, start + 2 * H);
            }

            if (tid == 3) {
                monitorSort3.signalSorted();
            }
            if (tid == 1) {
                monitorSort3.waitSorted();
            }

            if (tid == 1) {
                E.merge(0, N);
                monitorSynchronization.signalSortEnd();
            } else {
                monitorSynchronization.waitSortEnd();
            }

            // Copies
            xi = monitorResources.getX();
            final int ai = monitorResources.getA();
            final Matrix MOi = monitorResources.getMO();
            final Vector Ei = E.copy();

            // Main calc
            for (int h = start; h < finish; h++) {
                int temp = 0;
                for (int i = 0; i < N; i++) {
                    int prod = 0;
                    for (int j = 0; j < N; j++) {
                        prod += MK.get(h, j) * MOi.get(j, i);
                    }

                    temp += Ei.get(i) * prod;
                }

                A.set(h, ai * xi * T.get(h) + temp);
            }

            // Sync on calc2
            switch (tid) {
                case 1:
                    monitorSynchronization.waitCalculationsEnd();
                    break;
                default:
                    monitorSynchronization.signalCalculationsEnd();
                    break;
            }

            // Output
            if (tid == 1) {
                if (N <= OUTPUT_THRESHOLD) {
                    outputVector(A);
                }
            }

            System.out.println("Finished Thread " + tid);
        }
    }

    public Main() {
        MyThread[] threads = new MyThread[P];
        for (int i = 0; i < P; i++) {
            threads[i] = new MyThread(i + 1, i * H, (i+1) * H);
        }
        for (int i = 0; i < P; i++) {
            threads[i].start();
        }
        for (int i = 0; i < P; i++) {
            try {
                threads[i].join();
            } catch (Exception e) {}
        }
    }

//-----------------------------------------------------------------------------

    public static void main(String[] args) {
        new Main();
    }

    public void outputVector(Vector vector) {
        for (int i = 0; i < N; i++) {
            System.out.print(vector.get(i) + " ");
        }
        System.out.println();
    }

    public Vector fillVectorOnes() {
        return new Vector(N, 1);
    }

    public Matrix fillMatrixOnes() {
        return new Matrix(N, 1);
    }
}
