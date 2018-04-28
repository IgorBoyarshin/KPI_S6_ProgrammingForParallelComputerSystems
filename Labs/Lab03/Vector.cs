// ------------------------------------------------------------------------ //
// -------------------------------Vector----------------------------------- //
// ------------------------------------------------------------------------ //

public class Vector {
    private readonly int SIZE;
    private int[] elements;

    public Vector(int size) {
        this.SIZE = size;
        this.elements = new int[SIZE];
    }

    public Vector(int size, int v) {
        this.SIZE = size;
        this.elements = new int[SIZE];

        for (int i = 0; i < SIZE; i++) {
            this.elements[i] = v;
        }
    }

    public Vector(int size, int[] elements) {
        this.SIZE = size;
        this.elements = elements;
    }

    public int this[int index] {
        get {
            return elements[index];
        }
        set {
            elements[index] = value;
        }
    }
}
