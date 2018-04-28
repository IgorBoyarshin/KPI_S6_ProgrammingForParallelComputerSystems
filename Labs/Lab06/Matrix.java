// ------------------------------------------------------------------------ //
// -------------------------------Matrix----------------------------------- //
// ------------------------------------------------------------------------ //

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.regex.Pattern;


public class Matrix {
    private final int SIZE;
    private int[][] elements;

    public Matrix(int size) {
        this.SIZE = size;
        this.elements = new int[SIZE][SIZE];
    }

    public Matrix(int size, int value) {
        this.SIZE = size;
        this.elements = new int[SIZE][SIZE];

        for (int row = 0; row < SIZE; row++) {
            for (int column = 0; column < SIZE; column++) {
                this.elements[row][column] = value;
            }
        }
    }

    public Matrix(int size, int[][] elements) {
        this.SIZE = size;
        this.elements = elements;
    }

    public int get(int row, int column) {
        return this.elements[row][column];
    }

    public void set(int row, int column, int value) {
        this.elements[row][column] = value;
    }
}
