//-----------------------------------------------------------------------------
// Monitor Synchronization
//-----------------------------------------------------------------------------

public class MonitorSynchronization {
    private int F1 = 0;
    private int F2 = 0;
    private int F3 = 0;

    public synchronized void signalInputEnd() {
        F1++;

        if (F1 == 6) {
            notifyAll();
        }
    }

    public synchronized void signalCalc1End() {
        F2++;

        if (F2 == 6) {
            notifyAll();
        }
    }

    public synchronized void signalCalc2End() {
        F3++;

        if (F3 == 5) {
            notifyAll();
        }
    }

    public synchronized void waitInputEnd() {
        while (F1 < 6) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }

    public synchronized void waitCalc1End() {
        while (F2 < 6) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }

    public synchronized void waitCalc2End() {
        while (F3 < 5) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }
}
