// ----------------------------------------------------------------------------
// ------------------------ Monitor Sort --------------------------------------
// ----------------------------------------------------------------------------
public class MonitorSort {
    private boolean sorted = false;

    public synchronized void signalSorted() {
        sorted = true;
        notify();
    }

    public synchronized void waitSorted() {
        while (!sorted) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }
}
