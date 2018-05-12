// Monitor Resources

public class MonitorResources {
    private int d;
    private int e = Integer.MAX_VALUE;
    private Matrix MK;

    public synchronized void setD(int d) {
        this.d = d;
    }

    public synchronized int getD() {
        return d;
    }

    public synchronized void updateE(int e) {
        if (e < this.e) {
            this.e = e;
        }
    }

    public synchronized int getE() {
        return e;
    }

    public synchronized void setMK(Matrix MK) {
        this.MK = MK;
    }

    public synchronized Matrix getMK() {
        return MK;
    }
}
