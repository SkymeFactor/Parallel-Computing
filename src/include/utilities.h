#pragma once
#include <iostream>
#include <cmath>
#include <fstream>
#include "Matrix.h"

#define NEARLY_ZERO 1e-8



//-------------------------------------------------------------------------------------
// Test data generation
//-------------------------------------------------------------------------------------


template <typename T = float>
std::enable_if_t<std::is_floating_point_v<T>, std::pair<Matrix<T>, Matrix<T>> >
generate_test_data(const int& m, const int& k, const int& n, const T& max_val = 1000.0) {
    Matrix<T> mat_in1, mat_in2;
    srand(time(0));
    mat_in1.setSize(m, k);
    mat_in2.setSize(k, n);

    for (auto& it: mat_in1)
        it = rand() * max_val / static_cast<T>(RAND_MAX);

    for (auto& it: mat_in2)
        it = rand() * max_val / static_cast<T>(RAND_MAX);
    
    return {mat_in1, mat_in2};
};


//-------------------------------------------------------------------------------------
// CPU matrix multiplication
//-------------------------------------------------------------------------------------


template <typename T>
Matrix<T> mat_mul_cpu(const Matrix<T>& matA, const Matrix<T>& matB) {
    if (matA.getWidth() != matB.getHeight())
        throw std::invalid_argument("Matrices have incompatible size");
    
    Matrix<T> matC;
    matC.setSize(matA.getHeight(), matB.getWidth());

    for (unsigned i = 0; i < matC.getHeight(); ++i) {
        for (unsigned k = 0; k < matA.getWidth(); ++k) {
            for (unsigned j = 0; j < matC.getWidth(); ++j) {
                matC[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }

    return matC;
};


//-------------------------------------------------------------------------------------
// Matrices comparison
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
};


template <class T>
bool compareMatrices(Matrix<T> matA, Matrix<T> matB) {
    if (matA.getHeight() != matB.getHeight() || matA.getWidth() != matB.getWidth())
        return false;
    
    bool are_equal = true;

    float* test_arr = &matA[0][0];
    float* pOutputVector = &matB[0][0];

    for (unsigned i = 0; i < matA.getHeight() * matA.getWidth(); ++i) {
        if (!is_close(test_arr[i], pOutputVector[i])) {
            are_equal = false;
            //std::cout << i + 1 << ": " << pOutputVector[i] << ' ' << test_arr[i] << '\n';
            break;
        }
    }

    return are_equal;
};


//-------------------------------------------------------------------------------------
// Files processing
//-------------------------------------------------------------------------------------


template <class T = float>
std::pair<Matrix<T>, Matrix<T>> parse_matrices_file(const std::string& filename) {
    Matrix<T> a, b;
    T n, k, m, tmp;
    std::ifstream fin(filename);
    if (!fin.is_open())
        throw std::iostream::failure("Unable to read file " + filename);

    fin >> n >> k >> m;
    a.setSize(m, k);
    b.setSize(k, n);

    for (auto& it: a) {
        fin >> tmp;
        it = tmp;
    }

    for (auto& it: b) {
        fin >> tmp;
        it = tmp;
    }

    fin.close();

    return std::make_pair(a, b);
};


template <class T>
void save_matrix_to_file(const Matrix<T>& mat, const std::string& filename) {
    std::ofstream fout(filename);
    if (!fout.is_open())
        throw std::iostream::failure("Unable to write file " + filename);
    
    fout << mat.getWidth() << ' ' << mat.getHeight() << '\n';
    fout << std::fixed;

    for (unsigned i = 0; i < mat.getHeight(); ++i)
        for (unsigned j = 0; j < mat.getWidth(); j++)
            fout << mat[i][j] << (j < mat.getWidth() - 1 ? ' ' : '\n');
    
    fout.close();
};


//-------------------------------------------------------------------------------------
// Matrices padding
//-------------------------------------------------------------------------------------


template<class T>
Matrix<T> make_zero_padding(const Matrix<T>& mat, const unsigned& pad_size_h, const unsigned& pad_size_w) {
    Matrix<T> mat_out;
    mat_out.setSize(mat.getHeight() + pad_size_h, mat.getWidth() + pad_size_w);     // Zeroed by default
    
    for (unsigned h = 0; h < mat.getHeight(); ++h)
        for (unsigned w = 0; w < mat.getWidth(); ++w) {
            mat_out[h][w] = mat[h][w];
        }
    
    return mat_out;
}

template<class T>
Matrix<T> remove_padding(const Matrix<T>& mat, const unsigned& height, const unsigned& width) {
    Matrix<T> mat_out;
    mat_out.setSize(height, width);
    
    for (unsigned h = 0; h < mat_out.getHeight(); ++h)
        for (unsigned w = 0; w < mat_out.getWidth(); ++w) {
            mat_out[h][w] = mat[h][w];
        }
    
    return mat_out;
}