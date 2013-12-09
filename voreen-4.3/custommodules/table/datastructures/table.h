#include <vector>
#include <algorithm>

#include "column.h"

class Table {
public:
    Table() {}
    
    void addColumn(ColumnBase* col) {
        // We don't want to insert duplicates
        if (std::find(columns_.begin(), columns_.end(), col) != columns_.end()) return;
        
        columns_.push_back(col);
    }
    
    void addColumn(ColumnBase* col, size_t index) {
        // Check if the insert position is correct
        if (index >= numColumns()) return;
        // We don't want to insert duplicates
        if (std::find(columns_.begin(), columns_.end(), col) != columns_.end()) return;
        
        columns_.insert(columns_.begin() + index, col);
    }
    
    size_t numColumns() const {
        return columns_.size();
    }
    
    ColumnBase* getColumn(size_t index) const {
        if (index >= numColumns()) return 0;
        
        return columns_[index];
    }

protected:
    std::vector<ColumnBase*> columns_;
};

/*template <typename T>
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

typedef Table<float> TableFloat;*/
