//-----------------------------------------------------------------------------
// Monitor Synchronization
//-----------------------------------------------------------------------------

public class MonitorSynchronization {
    private int inputEnd = 0;
    private int calculationsEnd = 0;
    private boolean sortEnd = false;

    public synchronized void signalInputEnd() {
        inputEnd++;

        if (inputEnd == 3) {
            notifyAll();
        }
    }

    public synchronized void signalCalculationsEnd() {
        calculationsEnd++;

        if (calculationsEnd == 3) {
            notifyAll();
        }
    }

    public synchronized void signalSortEnd() {
        sortEnd = true;
        notifyAll();
    }

    public synchronized void waitInputEnd() {
        while (inputEnd < 3) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }

    public synchronized void waitCalculationsEnd() {
        while (calculationsEnd < 3) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }

    public synchronized void waitSortEnd() {
        while (!sortEnd) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }
}
