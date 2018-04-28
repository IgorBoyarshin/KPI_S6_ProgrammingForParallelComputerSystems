//-----------------------------------------------------------------------------
// Lab6: Java. Monitors
// Task: A = (B*C) * Z + min(E) * T * (MO*MK)
// Author: Igor Boyarshin
// Date: 24.04.2018
//-----------------------------------------------------------------------------

import java.util.concurrent.RecursiveAction;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.ForkJoinTask;
import java.util.List;
import java.util.ArrayList;

public class Main {

    private static final int N = 600;
    private static final int P = 6;
    private static final int H = N / P;
    private static final int OUTPUT_THRESHOLD = 8;

    private static ForkJoinPool thePool = new ForkJoinPool(P);

    private MonitorResources monitorResources = new MonitorResources();
    private MonitorSynchronization monitorSynchronization = new MonitorSynchronization();

    private Vector A = new Vector(N, 0);
    private Vector B;
    private Vector C;
    private Vector Z;
    private Vector E;
    private Matrix MO;

    public class Task extends RecursiveAction {
        private static final long serialVersionUID = 42L;

        private final int tid;
        private final int start;
        private final int finish;

        public Task(int tid, int start, int finish) {
            this.tid = tid;
            this.start = start;
            this.finish = finish;
        }

        @Override
        protected void compute() {
            if (finish - start == H) {
                process();
            } else {
                List<Task> subtasks = new ArrayList<>();

                subtasks.add(new Task(tid, start, start + H));
                subtasks.add(new Task(tid + 1, start + H, finish));

                ForkJoinTask.invokeAll(subtasks);
            }
        }

        private void process() {
            System.out.println(":> Starting Thread " + tid);

            // Input
            switch (tid) {
                case 1:
                    final Vector T = fillVectorOnes();
                    monitorResources.setT(T);
                    B = fillVectorOnes();
                    break;
                case 2:
                    C = fillVectorOnes();
                    break;
                case 3:
                    Z = fillVectorOnes();
                    final Matrix MK = fillMatrixOnes();
                    monitorResources.setMK(MK);
                    break;
                case 4:
                    E = fillVectorOnes();
                    MO = fillMatrixOnes();
                    break;
                default:
                    break;
            }

            // Sync on input
            monitorSynchronization.signalInputEnd();
            monitorSynchronization.waitInputEnd();

            // Calc1 a
            int ai = 0;
            for (int i = start; i < finish; i++) {
                ai += B.get(i) * C.get(i);
            }

            monitorResources.updateA(ai);

            // Calc1 e
            int ei = E.get(start);
            for (int i = start; i < finish; i++) {
                if (E.get(i) < ei) {
                    ei = E.get(i);
                }
            }

            monitorResources.updateE(ei);

            // Sync on calc1
            monitorSynchronization.signalCalc1End();
            monitorSynchronization.waitCalc1End();

            // Copies
            ai = monitorResources.getA();
            ei = monitorResources.getE();
            final Matrix MKi = monitorResources.getMK();
            final Vector Ti = monitorResources.getT();

            // Calc2
            for (int h = start; h < finish; h++) {
                int temp = 0;
                for (int i = 0; i < N; i++) {
                    int prod = 0;
                    for (int j = 0; j < N; j++) {
                        prod += MO.get(h, j) * MKi.get(j, i);
                    }

                    temp += Ti.get(i) * prod;
                }

                A.set(h, ai * Z.get(h) + ei * temp);
            }

            // Sync on calc2
            switch (tid) {
                case 2:
                    monitorSynchronization.waitCalc2End();
                    break;
                default:
                    monitorSynchronization.signalCalc2End();
                    break;
            }

            // Output
            if (tid == 2) {
                if (N <= OUTPUT_THRESHOLD) {
                    outputVector(A);
                }
            }

            System.out.println(":> Finished Thread " + tid);
        }
    }

    public Main() {
        thePool.invoke(new Task(1, 0, N));
    }

//-----------------------------------------------------------------------------

    public static void main(String[] args) {
        System.out.println(":> Main program started!");
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
