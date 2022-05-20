#pragma once
#include <iostream>
#include <fstream>


template <class T>
void report_failure(const T msg) {
    std::cerr << "[ ERROR ]: " << msg << '\n';
    std::exit(EXIT_FAILURE);
}

template <class T>
void write_vector_to_file(std::string filename, std::vector<T> data) {
    std::ofstream fout(filename, std::ios_base::binary);
    
    for (auto it: data) {
        fout.write(reinterpret_cast<char*>(&it), sizeof(it));
    }

    fout.close();
}