//-----------------------------------------------------------------------------
// Monitor Resources
//-----------------------------------------------------------------------------
public class MonitorResources {
    private Matrix MO;
    private int a;
    private int x = Integer.MIN_VALUE;

    public synchronized void updateX(int x) {
        if (x > this.x) {
            this.x = x;
        }
    }

    public synchronized int getX() {
        return x;
    }

    public synchronized void setMO(Matrix MO) {
        this.MO = MO;
    }

    public synchronized Matrix getMO() {
        return MO.copy();
    }

    public synchronized void setA(int a) {
        this.a = a;
    }

    public synchronized int getA() {
        return a;
    }
}
