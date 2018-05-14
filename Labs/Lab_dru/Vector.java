// ------------------------------------------------------------------------ //
// -------------------------------Vector----------------------------------- //
// ------------------------------------------------------------------------ //

import java.util.Arrays;

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

    public void sort(int from, int to) {
        Arrays.sort(elements, from, to);
    }

    public Vector copy() {
        return new Vector(SIZE, elements);
    }

    public void merge(int from, int to) {
        final int size = (to - from) / 2;
        int v1 = from;
        int v2 = from + size;
        int v = 0;
        int[] arr = new int[2 * size];

        while (v < 2 * size) {
            if (v1 >= (from + size)) while (v2 < to) arr[v++] = elements[v2++];
            if (v2 >= to) while (v1 < (from + size)) arr[v++] = elements[v1++];

            while (v1 < (from + size) && elements[v1] <= elements[v2]) {
                arr[v++] = elements[v1++];
            }
            while (v2 < (to) && elements[v2] <= elements[v1]) {
                arr[v++] = elements[v2++];
            }
        }

        for (int i = from; i < to; i++) {
            elements[i] = arr[i - from];
        }
    }
}
