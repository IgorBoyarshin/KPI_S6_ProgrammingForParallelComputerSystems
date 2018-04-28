// ------------------------------------------------------------------------ //
// -------------------------------Vector----------------------------------- //
// ------------------------------------------------------------------------ //

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.regex.Pattern;


public class Vector {
    private final int SIZE;
    private int[] elements;

    public Vector(int size) {
        this.SIZE = size;
        this.elements = new int[SIZE];
    }

    public Vector(int size, int value) {
        this.SIZE = size;
        this.elements = new int[SIZE];

        for (int i = 0; i < SIZE; i++) {
            this.elements[i] = value;
        }
    }

    public Vector(int size, int[] elements) {
        this.SIZE = size;
        this.elements = elements;
    }

    public int get(int index) {
        return this.elements[index];
    }

    public void set(int index, int value) {
        this.elements[index] = value;
    }
}
