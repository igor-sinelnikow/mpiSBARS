#ifndef __COMPARE_HEADER__
#define __COMPARE_HEADER__

#include "gpu_computing.h"
#include "decomposition.h"
#include "matrix_gomology.h"

#include "analyze.h"

#include <vector>
#include <math.h>

typedef vector<Repeat> TypeNew;

class Compare {
    MyMPI me;
    double eps;
    GpuComputing gpu;

    MatrixGomology compareSelf(Decomposition &decomposition);
    MatrixGomology compareTwo(Decomposition &decomposition1, Decomposition &decomposition2);

    void compareDecomposition(TypeDecomposition *decompose1, ulong length_decompose1,
                              TypeDecomposition *decompose2, ulong length_decompose2,
                              ulong width, TypeGomology *data, ulong begin, ulong sum_all);
    void compareDecompositionHost(TypeDecomposition *decompose1, ulong length_decompose1,
                                  TypeDecomposition *decompose2, ulong length_decompose2,
                                  ulong width, TypeGomology *data, ulong begin, ulong sum_all);
    bool compareVector(TypeDecomposition *vec1, TypeDecomposition *vec2, ulong length);

public:
    Compare(MyMPI me, GpuComputing gpu, double eps);
    ~Compare();

    MatrixGomology doCompare(Decomposition &decompose);
    MatrixGomology doCompare(Decomposition &decompose1, Decomposition &decompose2);
    MatrixGomology comparisonMatrix(MatrixGomology matrix1, MatrixGomology matrix2);

    double getEps();
    void setEps(double eps_new);
};

#endif /* __COMPARE_HEADER__ */
