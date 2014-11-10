#include "compare.h"

Compare::Compare(MyMPI new_me)
    : eps(0), me(new_me)
{
}

Compare::~Compare()
{
}


MatrixGomology Compare::doCompare(Decomposition &decompose1GC, Decomposition &decompose1GA, double new_eps)
{
    eps = new_eps;
    MatrixGomology matGC(me), matGA(me), matrixGomology(me);
    matGC = compareSelf(decompose1GC);
    matGA = compareSelf(decompose1GA);
    matrixGomology = comparisonMatrix(matGC, matGA);
    return matrixGomology;
}

MatrixGomology Compare::doCompare(Decomposition &decompose1GC, Decomposition &decompose1GA,
                          Decomposition &decompose2GC, Decomposition &decompose2GA, double new_eps)
{
    eps = new_eps;
    MatrixGomology matGC(me), matGA(me), matrixGomology(me);
    matGC = compareTwo(decompose1GC, decompose2GC);
    matGA = compareTwo(decompose1GA, decompose2GA);
    matrixGomology = comparisonMatrix(matGC, matGA);
    return matrixGomology;
}

MatrixGomology Compare::compareSelf(Decomposition &decomposition)
{
    ulong length_all[me.getSize()];
    //ulong *length = new ulong [me.getSize()]; ????
    me.Allgather(&decomposition.height, 1, MPI_UNSIGNED_LONG,
                 &length_all,           1, MPI_UNSIGNED_LONG);

    MPI_Request *req_send = new MPI_Request [me.getSize()];
    for (int i = 0; i < me.getSize(); i++)
        me.iSend(decomposition.data, decomposition.length, MPI_TFLOAT, i, 0, &req_send[i]);

    ulong sum_all = 0;
    ulong *decompose_other_begin = new ulong [me.getSize()];
    ulong *sum_length_array  =  new ulong [me.getSize()];
    for (int i = 0; i < me.getSize(); i++) {
        decompose_other_begin[i] = sum_all * decomposition.width;
        sum_length_array[i] = sum_all;
        sum_all += length_all[i];
    }

    TypeDecomposition *decompose_other = new TypeDecomposition [sum_all * decomposition.width];
    MPI_Request *req_recv = new MPI_Request [me.getSize()];
    for (int i = 0; i < me.getSize(); i++) {
        me.iRecv(&decompose_other[decompose_other_begin[i]],
                 decomposition.width * length_all[i], MPI_TFLOAT, i, 0, &req_recv[i]);
    }

    MatrixGomology matrixGomology(me);
    matrixGomology.length = decomposition.height * sum_all;
    matrixGomology.height = decomposition.height;
    matrixGomology.width = sum_all;
    matrixGomology.data = new bool [matrixGomology.length];
    for (uint i = 0; i < matrixGomology.length; i++)
        matrixGomology.data[i] = false;

    int num_send = 0;
    bool *send_flag = new bool [me.getSize()];
    for (int i = 0; i < me.getSize(); i++)
        send_flag[i] = false;
    while (num_send < me.getSize()) {
        for (int i = 0; i < me.getSize(); i++) {
            if (!send_flag[i] && me.Test(&req_recv[i])) {
                compareDecomposition(decomposition.data, decomposition.height,
                                    &decompose_other[decompose_other_begin[i]],
                                    length_all[i],
                                    decomposition.width, matrixGomology.data,
                                    sum_length_array[i], sum_all);
                send_flag[i] = true;
                num_send++;
            }
        }
    }
    return matrixGomology;
}

MatrixGomology Compare::compareTwo(Decomposition &decomposition1, Decomposition &decomposition2)
{
    ulong length_all[me.getSize()];
    //ulong *length = new ulong [me.getSize()]; ????
    me.Allgather(&decomposition2.height, 1, MPI_UNSIGNED_LONG,
                 &length_all,            1, MPI_UNSIGNED_LONG);

    MPI_Request *req_send = new MPI_Request [me.getSize()];
    for (int i = 0; i < me.getSize(); i++)
        me.iSend(decomposition2.data, decomposition2.length, MPI_TFLOAT, i, 0, &req_send[i]);

    ulong sum_all = 0;
    ulong *decompose_other_begin = new ulong [me.getSize()];
    ulong *sum_length_array  =  new ulong [me.getSize()];
    for (int i = 0; i < me.getSize(); i++) {
        decompose_other_begin[i] = sum_all * decomposition2.width;
        sum_length_array[i] = sum_all;
        sum_all += length_all[i];
    }

    TypeDecomposition *decompose_other = new TypeDecomposition [sum_all * decomposition2.width];
    MPI_Request *req_recv = new MPI_Request [me.getSize()];
    for (int i = 0; i < me.getSize(); i++) {
        me.iRecv(&decompose_other[decompose_other_begin[i]],
                 decomposition2.width * length_all[i], MPI_TFLOAT, i, 0, &req_recv[i]);
    }

    MatrixGomology matrixGomology(me);
    matrixGomology.length = decomposition1.height * sum_all;
    matrixGomology.height = decomposition1.height;
    matrixGomology.width = sum_all;
    matrixGomology.data = new bool [matrixGomology.length];
    for (uint i = 0; i < matrixGomology.length; i++)
        matrixGomology.data[i] = false;

    int num_send = 0;
    bool *send_flag = new bool [me.getSize()];
    for (int i = 0; i < me.getSize(); i++)
        send_flag[i] = false;
    while (num_send < me.getSize()) {
        for (int i = 0; i < me.getSize(); i++) {
            if (!send_flag[i] && me.Test(&req_recv[i])) {
                compareDecomposition(decomposition1.data, decomposition1.height,
                                    &decompose_other[decompose_other_begin[i]],
                                    length_all[i],
                                    decomposition1.width, matrixGomology.data,
                                    sum_length_array[i], sum_all);
                send_flag[i] = true;
                num_send++;
            }
        }
    }
    return matrixGomology;
}

MatrixGomology Compare::comparisonMatrix(MatrixGomology mat1, MatrixGomology mat2)
{
    for (int i = 0; i < mat1.length; i++)
        mat1.data[i] = (mat1.data[i] && mat2.data[i]);
    mat2.free();
    MatrixGomology matrixGomology(me);
    matrixGomology = mat1;
    return matrixGomology;
}

void Compare::compareDecomposition(TypeDecomposition *decompose1, ulong length_decompose1,
                                   TypeDecomposition *decompose2, ulong length_decompose2,
                                   ulong width, TypeGomology *data, ulong begin, ulong sum_all)
{
    bool answer;
    for (int i = 0; i < length_decompose1; i++)
        for (int j = 0; j < length_decompose2; j++) {
            answer = compareVector(&decompose1[i * width], &decompose2[j * width], width);
            data[i * sum_all + begin + j] = answer;
        }
}

bool Compare::compareVector(TypeDecomposition *vec1, TypeDecomposition *vec2, ulong length)
{
    TypeDecomposition sum = 0;
    double eps_2 = eps * eps;
    for (int i = 0; i < length; i++) {
        sum += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
        if (sum > eps_2)
            break;
    }
    if (sum > eps_2)
        return false;
    else
        return true;
}
