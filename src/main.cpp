#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <omp.h>

#include "omp_estimator.h"



typedef struct {
    int width, height, max_val;
    std::vector<int> data;
} image;


void report_failure(std::string_view msg) {
    std::cerr << "[ ERROR ]: " << msg << '\n';
    std::exit(EXIT_FAILURE);
}


template <class T>
std::string to_bitstring(T number) {
    std::string result;
    union {
        T value;
        uint8_t alias[sizeof(T)];
    } src {number};
    
    for (auto it: src.alias) {
        for (int i = 0; i < 8; i++)
            if ((it >> i) & 1)
                result.append("1");
            else
                result.append("0");
    }

    return result;
}


class ImageHist {
private:
    int hist_counts[256];
    std::vector<int> img;
public:
    ImageHist() {};
    ImageHist(std::vector<int>& image): hist_counts{0}, img{image} {
        
    };

    std::vector<int> get_counts() {
        std::vector<int> ret(256);
        img.clear();
        std::copy(std::begin(hist_counts), std::end(hist_counts), ret.begin());

        return ret;
    }

    void count(std::vector<int> vec) {
        omp_estimator::timer_begin();
        std::cout << "Hello, world!\n";
        omp_estimator::timer_end();
    }
};


image read_image_from_file(std::string filename) {
    std::ifstream fin(filename);
    std::string tmp;
    image new_image;

    if (!fin.is_open())
        ::report_failure("Unable to open input file");
    
    fin >> tmp;
    if (tmp.compare("P5") != 0)
        ::report_failure("Invalid file format");
    
    fin >> new_image.width >> new_image.height >> new_image.max_val;

    //std::cout << new_image.width << ' ' << new_image.height << ' ' << new_image.max_val;

    while (!fin.eof()) {
        fin >> tmp;
        //std::cout << tmp;
        new_image.data.push_back(std::stoi(tmp));
    }
    
    return new_image;
}

std::array<int, 256> compute_hist_values(image img) {
    std::array<int, 256> result {0};
    //#pragma omp parallel for
    for (int i = 0; i < img.data.size(); i++) {
        result[img.data[i]]++;
    }
    return result;
}


int main(int argc, char* argv[]) {
    auto vec = read_image_from_file(argv[1]);
    /*if (argc != 4)
        ::report_failure("Invalid number of arguments\n");

    bool omp_enable_flag = true;

    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    int thr_num = std::atoi(argv[3]);
    
    if (!fin.is_open() || !fout.is_open())
        ::report_failure("Unable to open file");
    
    if (thr_num < -1)
        ::report_failure("Invalid number of threads\n");
    
    switch (thr_num) {
        case -1: omp_enable_flag = false; break;
        case 0: break;
        default: omp_set_num_threads(thr_num); break;
    }*/
    //std::vector<int> vec{0, 5, 5, 5, 128};
    //auto ret = compute_hist_values(vec);
    /*auto ret = ImageHist().get_counts();
    for (auto it: ret) {
        std::cout << it << ' ';
    }*/
    ImageHist hst {};
    omp_estimator::PerformanceEstimator est;

    //est.estimate(&ImageHist::count, &hst, vec);
    est.estimate(compute_hist_values, vec);
    auto ret = std::any_cast<std::array<int, 256>>(est.get_return_value());
    for (auto i: ret)
        std::cout << i << ' ';
    std::cout << '\n';
    /*std::cout << "Float: " << 2.72 << '\n';
    std::cout << "Bitstring: " << to_bitstring(-2.732f) << '\n';*/
    return 0;
}