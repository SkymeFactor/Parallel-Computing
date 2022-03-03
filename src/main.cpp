#include <iostream>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <omp.h>


double f_x(double x) {
    return std::log(std::sin(x));
}

int execution_time = -1;

template<class T>
double get_integral(T func, float a, float b, double err_val) {
    double area = 0.0;
    double step = b - a;
    double area_prev = 1.0;
    
    auto t1 = std::chrono::high_resolution_clock::now();
    while( (area_prev - area) / 3 >= err_val ) {
        area_prev = area;
        area = 0.0;
        step = step / 2;
        #pragma omp parallel shared(a, b, step, area)
        {
            #pragma omp for reduction(+: area) // schedule(runtime)
            for (int i = 0; i < (b - a) / step; i++) {
                area += func(a + step / 2 + i * step) * step;
            }
            /*#pragma omp single
            std::cout << "Step = " << step << "; Area: " << area << '\n';*/
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    
    /*std::cout << "Cycles: " << (int)((b - a) / step) << '\n';
    std::cout << "Duration: " << duration.count() << " ms\n";*/
    execution_time = duration.count();
    
    return area;
}

template<class T>
double get_integral_no_omp(T func, float a, float b, double err_val) {
    double area = 0.0;
    double step = b - a;
    double area_prev = 1.0;
    
    auto t1 = std::chrono::high_resolution_clock::now();
    while( (area_prev - area) / 3 >= err_val ) {
        area_prev = area;
        area = 0.0;
        step = step / 2;
        for (int i = 0; i < (b - a) / step; i++) {
            area += func(a + step / 2 + i * step) * step;
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    execution_time = duration.count();
    
    return area;
}

int main(int argc, char* argv[]) {
    /// TODO: work with files
    if (argc == 4) {
        std::string in_filename {argv[1]};
        std::string out_filename {argv[2]};
        //std::cout << in_filename << '\n';
        //std::cout << out_filename << '\n';
        omp_set_num_threads(std::atoi(argv[3]));
    } else {
        /// TODO: logging function
        std::cerr << "[ ERROR ] Invalid number of arguments\n";
        std::exit(1);
    }

    double avg_time = 0;
    //std::cout << std::fixed << std::setprecision(10);
    for (int i = 0; i < 100; i++) {
        get_integral_no_omp(f_x, 0, M_PI, 0.00001);
        avg_time += execution_time;
    }
    //std::cout << get_integral(f_x, 0, M_PI, 0.00001 ) << '\n';
    printf("Time (%i thread(s)): %g ms\n", std::atoi(argv[3]), avg_time / 100);
    
    return 0;
}