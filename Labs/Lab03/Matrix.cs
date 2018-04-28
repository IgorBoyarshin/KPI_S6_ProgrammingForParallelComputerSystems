// ------------------------------------------------------------------------ //
// -------------------------------Matrix----------------------------------- //
// ------------------------------------------------------------------------ //

public class Matrix {
    private readonly int SIZE;
    private int[,] elements;

    public Matrix(int size) {
        this.SIZE = size;
        this.elements = new int[SIZE, SIZE];
    }

    public Matrix(int size, int v) {
        this.SIZE = size;
        this.elements = new int[SIZE, SIZE];

        for (int row = 0; row < SIZE; row++) {
            for (int column = 0; column < SIZE; column++) {
                this.elements[row, column] = v;
            }
        }
    }

    public Matrix(int size, int[,] elements) {
        this.SIZE = size;
        this.elements = elements;
    }

    public int this[int x, int y] {
        get {
            return elements[x, y];
        }
        set {
            elements[x, y] = value;
        }
    }

    public Matrix copy() {
        int[,] elems = new int[SIZE, SIZE];
        for (int row = 0; row < SIZE; row++) {
            for (int column = 0; column < SIZE; column++) {
                elems[row, column] = elements[row, column];
            }
        }

        return new Matrix(SIZE, elems);
    }
}
