    const int nodes = L * L;
    int neigh[nodes];
    int indices[nodes];
    int edges[2*4+3*4+2];

    neigh[getRank(0,0)] = 2;
    neigh[getRank(0,1)] = 3;
    neigh[getRank(0,2)] = 2;
    neigh[getRank(1,0)] = 3;
    neigh[getRank(1,1)] = 4;
    neigh[getRank(1,2)] = 2;
    neigh[getRank(2,0)] = 2;
    neigh[getRank(2,1)] = 2;
    neigh[getRank(2,2)] = 0;

    indices[0] = neigh[0];
    for (unsigned int i = 1; i < nodes; i++) {
        indices[i] = indices[i-1] + neigh[i];
    }

    for (unsigned int row = 0; row < L; row++) {
        for (unsigned int column = 0; column < L; column++) {
            unsigned int ed = 0;
            if (row > 0) {
                if (!(row == 2 && column == 2))
                edges[ed++] = getRank(row - 1, column);
            }
            if (row < L - 1) {
                if (!(row == 1 && column == 2))
                edges[ed++] = getRank(row + 1, column);
            }
            if (column > 0) {
                if (!(row == 2 && column == 2))
                edges[ed++] = getRank(row, column - 1);
            }
            if (column < L - 1) {
                if (!(row == 2 && column == 1))
                edges[ed++] = getRank(row, column + 1);
            }
        }
    }


    MPI_Comm commun;
    MPI_Graph_create(MPI_COMM_WORLD, nodes, indices, edges, false, &commun);

    Vector a(ALL_H);
    if (row == 0 && column == 0) {
        fillVector(a, 2);
        MPI_Request request;
        MPI_Isend(a.getPrt(), N, MPI_INT, getRank(2, 2), 0, commun, &request);
    } else if (row == 2 && column == 2) {
        MPI_Recv(a.getPrt(), N, MPI_INT, getRank(0, 0), MPI_ANY_TAG, commun, MPI_STATUS_IGNORE);
        a[0] = 0;
        outputVector(a);
    }
