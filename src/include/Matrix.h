#pragma once
#include <vector>
#include <algorithm>

template<class T = float>
class Matrix {
private:
    class _Row {
        T* data;
        const unsigned width;
    public:
        _Row(T* data, const unsigned& width): data{data}, width{width} {};
        
        T& operator[](const int& i) {
            if (i < 0 || i >= width)
                throw std::out_of_range("Matrix column index is out of range");
            return data[i];
        }
    };

    mutable std::vector<T> data;
    unsigned int width;
    unsigned int height;
public:
    Matrix() = default;
    inline _Row operator[](const int& i) const {
        if (i < 0 || i >= height)
            throw std::out_of_range("Matrix row index is out of range");
        return _Row(&data[i * width], width);
    };

    inline void setSize(const unsigned& height, const unsigned& width) {
        this->height = height;
        this->width = width;

        data.resize(height * width);
        std::fill(data.begin(), data.end(), 0.0f);
    };

    inline void setSizeNoResizing(const unsigned& height, const unsigned& width) {
        this->height = height;
        this->width = width;
    }

    inline unsigned int getWidth()  const noexcept { return width; };
    inline unsigned int getHeight() const noexcept { return height; };
    inline unsigned int getSizeInBytes() const noexcept { return width * height * sizeof(T); };
    
    inline auto begin() noexcept { return data.begin(); };
    inline auto end() noexcept { return data.end(); };
};
