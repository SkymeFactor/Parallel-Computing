#pragma once
#include <iostream>
#include <cmath>
#include <fstream>
#include "Matrix.h"

#define NEARLY_ZERO 1e-8



template <typename T = float>
std::enable_if_t<std::is_floating_point_v<T>, std::pair<Matrix<T>, Matrix<T>> >
generateTestData(const int& m, const int& k, const int& n, const T& max_val = 1000.0) {
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


template <typename T>
Matrix<T> mat_mul_cpu(const Matrix<T>& matA, const Matrix<T>& matB) {
    if (matA.getWidth() != matB.getHeight())
        throw std::invalid_argument("Matrices have incompatible size");
    
    Matrix<T> matC;
    matC.setSize(matA.getHeight(), matB.getWidth());

    for (int i = 0; i < matC.getHeight(); ++i) {
        for (int k = 0; k < matA.getWidth(); ++k) {
            for (int j = 0; j < matC.getWidth(); ++j) {
                matC[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }

    return matC;
};


template<class T>
std::enable_if_t<std::is_floating_point_v<T>, bool> is_close(const T& a, const T& b) {
    if (std::fabs(a - b) <= NEARLY_ZERO)
        return true;
    
    union Float {
        std::int64_t int_repr;
        T float_repr;
        Float(T val) : int_repr(0) { float_repr = val; };
    };

    return std::abs(Float(a).int_repr - Float(b).int_repr) <= 5; // 5 ULPs distance is more than enough
};

template <class T>
bool compareMatrices(Matrix<T> matA, Matrix<T> matB) {
    if (matA.getHeight() != matB.getHeight() || matA.getWidth() != matB.getWidth())
        return false;
    
    bool are_equal = true;

    float* test_arr = &matA[0][0];
    float* pOutputVector = &matB[0][0];

    for (int i = 0; i < matA.getHeight() * matA.getWidth(); ++i) {
        if (!is_close(test_arr[i], pOutputVector[i])) {
            are_equal = false;
            std::cout << i + 1 << ": " << pOutputVector[i] << ' ' << test_arr[i] << '\n';
            break;
            //std::cout << "\033[0;0;31m";
        }
        //std::cout << i + 1 << ": " << pOutputVector[i] << ' ' << test_arr[i] << '\n';
        //std::cout << "\033[0m";
    }

    return are_equal;
};


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

    for (int i = 0; i < mat.getHeight(); ++i)
        for (int j = 0; j < mat.getWidth(); j++)
            fout << mat[i][j] << (j < mat.getWidth() - 1 ? ' ' : '\n');
};