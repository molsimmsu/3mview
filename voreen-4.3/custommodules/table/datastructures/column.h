#include <vector>
#include <string>

class ColumnBase {
public:
	ColumnBase(std::string title) :
	    title_(title)
	{}

    std::string getTitle() const {
        return title_;
    }
    
    void setTitle(std::string title) {
        title_ = title;
    }

private:
    std::string title_;
};

template <typename T>
class Column : public ColumnBase {
public:
    Column(std::string title) :
        ColumnBase(title)
    {}
    
    Column(std::string title, T defaultValue) :
        ColumnBase(title),
        defaultValue_(defaultValue)
    {}
    
    size_t numRows() const {
        return values_.size();
    }
    
    T getValue(size_t row) const {
        if (row >= numRows())
            return defaultValue_;
            
        return values_[row];
    }
    
    void setValue(T value, size_t row) {
        
    }
    
private:
    std::vector<T> values_;
    T defaultValue_;
};
