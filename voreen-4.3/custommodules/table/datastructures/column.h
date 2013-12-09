#include <vector>
#include <string>

template <typename T>
class Column {
public:
    Column(std::string title);
    Column(std::string title, T defaultValue);
    
    size_t numRows() const;
    
    T getValue(size_t row) const;
    
    void setValue(T value, size_t row);
    
private:
    std::vector<T> values_;
    std::string title_;
    T defaultValue_;
};

typedef std::vector<size_t> TableIndexList;
typedef Column<float> ColumnFloat;
