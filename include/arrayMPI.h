#ifndef __ARRAY_MPI_HEADER__
#define __ARRAY_MPI_HEADER__

#include "myMPI.h"
#include "dataMPI.h"

#include <fstream>

template <class DataType, class LengthData>
class ArrayMPI : public DataMPI<DataType, LengthData> {
protected:
    virtual void readMPI(char *file_name);
    virtual void readUsually(char *file_name);
    virtual void readMy(char *file_name);

    virtual void writeMPI(char *file_name);
    virtual void writeUsually(char *file_name);
    virtual void writeMy(char *file_name);
public:
    ArrayMPI(MyMPI me);
    virtual ~ArrayMPI();
};

#include "arrayMPI.tcc"

#endif /* __ARRAY_MPI_HEADER__ */
