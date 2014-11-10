#ifndef __DECOMPOSITION_HEADER__
#define __DECOMPOSITION_HEADER__

#include "matrixMPI.h"
#include "types.h"

class Decomposition : public MatrixMPI<TypeDecomposition, ulong> {
public:
    Decomposition(MyMPI me);
    ~Decomposition();
    friend class Decompose;
    friend class Compare;
};

#endif /* __DECOMPOSITION_HEADER__ */