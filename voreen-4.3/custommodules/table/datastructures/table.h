#include <vector>

#include "column.h"

template <typename T>
class Table {
public:
    Table();
    
    void addColumn(T* col);
    
    void addColumn(T* col, size_t index);
    
    size_t numColumns() const;
    
    T* getColumn(size_t index) const;
    
    
private:
    std::vector<T*> columns_;
};

typedef Table<float> TableFloat;
