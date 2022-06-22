#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>

#define NEARLY_ZERO 1e-8



//-------------------------------------------------------------------------------------
// Test data generation
//-------------------------------------------------------------------------------------


template <class T = float>
std::enable_if_t<std::is_floating_point_v<T>, std::vector<T> >
generate_test_data(const std::size_t& size, const T& max_val = 1000.0) {
    std::vector<T> vec(size);
    srand(time(0));

    for (auto& it: vec)
        it = rand() * max_val / static_cast<T>(RAND_MAX);

    return vec;
}


//-------------------------------------------------------------------------------------
// CPU prefix sum calculation
//-------------------------------------------------------------------------------------


template <class T>
std::vector<T> prefixSumCPU(const std::vector<T>& vec){
    std::vector<T> result;
    T tmp = static_cast<T>(0.0);

    for (auto& it: vec) {
        tmp = tmp + it;
        result.push_back(tmp);
    }

    return result;
}


//-------------------------------------------------------------------------------------
// Vectors comparison
//-------------------------------------------------------------------------------------


template<class T>
std::enable_if_t<std::is_floating_point_v<T>, bool> is_close(const T& a, const T& b) {
    if (std::fabs(a - b) <= NEARLY_ZERO)
        return true;
    
    union Float {
        std::int64_t int_repr;
        T float_repr;
        Float(T val) : int_repr(0) { float_repr = val; };
    };

    return std::abs(Float(a).int_repr - Float(b).int_repr) <= 6; // 6 ULPs distance is more than enough
}


template <class T>
bool compareVectors(std::vector<T> vec_1, std::vector<T> vec_2) {
    if (vec_1.size() != vec_2.size())
        return false;
    
    bool are_equal = true;

    for (unsigned i = 0; i < vec_1.size(); ++i) {
        if (!is_close(vec_1[i], vec_2[i])) {
            are_equal = false;
            //std::cout << "\n\033[0;31m" << i + 1 << ": " << vec_1[i] << ' ' << vec_2[i] << "\033[0m";
            break;
        }
    }

    return are_equal;
}


//-------------------------------------------------------------------------------------
// Files processing
//-------------------------------------------------------------------------------------


template <class T = float>
std::vector<T> parse_vector_file(const std::string& filename) {
    unsigned vec_size;
    std::vector<T> vec;
    std::ifstream fin(filename);

    if (!fin.is_open())
        throw std::iostream::failure("Unable to read file '" + filename + "'");

    fin >> vec_size;
    vec.resize(vec_size);

    for (auto i = 0; i < vec_size; ++i)
        fin >> vec[i];

    fin.close();

    return vec;
}


template <class T = float>
void save_vector_to_file(const std::vector<T>& vec, const std::string& filename) {
    std::ofstream fout(filename);
    if (!fout.is_open())
        throw std::iostream::failure("Unable to write file '" + filename + "'");
    
    fout << std::fixed;

    for (auto& it: vec)
        fout << it << ' ';

    fout.close();
}


//-------------------------------------------------------------------------------------
// Vectors padding
//-------------------------------------------------------------------------------------


template<class T>
std::vector<T> make_zero_padding(const std::vector<T>& vec, const unsigned& pad_size) {
    std::vector<T> vec_out(vec.size() + pad_size);
    
    for (unsigned i = 0; i < vec.size(); ++i) {
        vec_out[i] = vec[i];
    }
    
    return vec_out;
}

template<class T>
std::vector<T> remove_padding(const std::vector<T>& vec, const unsigned& pad_size) {
    std::vector<T> vec_out(vec.size() - pad_size);
    
    for (unsigned i = 0; i < vec_out.size(); ++i) {
        vec_out[i] = vec[i];
    }
    
    return vec_out;
}