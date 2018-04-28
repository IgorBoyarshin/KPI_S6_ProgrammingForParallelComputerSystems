//-----------------------------------------------------------------------------
// Monitor Resources
//-----------------------------------------------------------------------------
public class MonitorResources {
    private int a = 0;
    private int e = Integer.MAX_VALUE;
    private Matrix MK;
    private Vector T;

    public synchronized void updateA(int a) {
        this.a += a;
    }

    public synchronized int getA() {
        return a;
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

    public synchronized void setT(Vector T) {
        this.T = T;
    }

    public synchronized Vector getT() {
        return T;
    }
}
