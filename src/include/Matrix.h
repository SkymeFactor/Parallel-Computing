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
        constexpr _Row(T* data, const unsigned& width): data{data}, width{width} {};
        
        constexpr T& operator[](const unsigned& i) {
            return (i < width) ? data[i] : throw std::out_of_range("Matrix column index is out of range");
        }
    };

    mutable std::vector<T> data;
    unsigned int width;
    unsigned int height;
public:
    constexpr Matrix() = default;
    constexpr _Row operator[](const unsigned& i) const {
        if (i >= height)
            throw std::out_of_range("Matrix row index is out of range");
        return _Row(&data[i * width], width);
    };

    inline void setSize(const unsigned& height, const unsigned& width) {
        this->height = height;
        this->width = width;

        data.resize(height * width);
        std::fill(data.begin(), data.end(), 0.0f);
    };

    constexpr unsigned int getWidth()  const noexcept { return width; };
    constexpr unsigned int getHeight() const noexcept { return height; };
    constexpr unsigned int getSizeInBytes() const noexcept { return width * height * sizeof(T); };
    
    inline auto begin() noexcept { return data.begin(); };
    inline auto end() noexcept { return data.end(); };
};
