// Lab6: Java. Monitors
// MA = min(Z) * (MO*MK) - d*MS
// Artem Berezynec
// 10.05.2018

import java.util.List;
import java.util.ArrayList;

public class Main {

    private static final int N = 4;
    private static final int P = 4;
    private static final int H = N / P;

    private MonitorResources monitorResources = new MonitorResources();
    private MonitorSynchronization monitorSynchronization = new MonitorSynchronization();

    private Matrix MA = new Matrix(N, 0);
    private Matrix MO;
    private Matrix MS;
    private Vector Z;

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
                    monitorSynchronization.waitInputEnd();
                    break;
                case 2:
                    Z = fillVectorOnes();
                    final Matrix MK = fillMatrixOnes();
                    monitorResources.setMK(MK);

                    monitorSynchronization.signalInputEnd();
                    monitorSynchronization.waitInputEnd();
                    break;
                case 3:
                    final int d = 1;
                    monitorResources.setD(d);
                    MO = fillMatrixOnes();

                    monitorSynchronization.signalInputEnd();
                    monitorSynchronization.waitInputEnd();
                    break;
                case 4:
                    MS = fillMatrixOnes();

                    monitorSynchronization.signalInputEnd();
                    monitorSynchronization.waitInputEnd();
                    break;
                default:
                    break;
            }

            // Calc1 e
            int ei = Z.get(start);
            for (int i = start; i < finish; i++) {
                if (Z.get(i) < ei) {
                    ei = Z.get(i);
                }
            }
            monitorResources.updateE(ei);

            // Sync on calc1
            monitorSynchronization.signalCalc1End();
            monitorSynchronization.waitCalc1End();

            // Copies
            ei = monitorResources.getE();
            final Matrix MKi = monitorResources.getMK();
            final int d = monitorResources.getD();

            // Calc2
            for (int h = start; h < finish; h++) {
                for (int i = 0; i < N; i++) {
                    int prod = 0;
                    for (int j = 0; j < N; j++) {
                        prod += MO.get(h, j) * MKi.get(j, i);
                    }
                    MA.set(h, i, ei * prod - d * MS.get(h, i));
                }
            }

            // Sync on calc2
            switch (tid) {
                case 4:
                    monitorSynchronization.waitCalc2End();
                    break;
                default:
                    monitorSynchronization.signalCalc2End();
                    break;
            }

            // Output
            if (tid == 4) {
                if (N <= 8) {
                    outputMatrix(MA);
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

    public void outputMatrix(Matrix matrix) {
        for (int row = 0; row < N; row++) {
            for (int column = 0; column < N; column++) {
                System.out.print(matrix.get(row, column) + " ");
            }
            System.out.println();
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
