#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <omp.h>

#include "omp_estimator.h"



void report_failure(std::string_view msg) {
    std::cerr << "[ ERROR ]: " << msg << '\n';
    std::exit(EXIT_FAILURE);
}


double f_x(double x) {
    return std::log(std::sin(x));
}


double get_integral(double (*func)(double), double left, double right, double err_val) {
    double area = 0.0;
    double step = right - left;
    double area_prev = std::numeric_limits<double>::max();
    
    omp_estimator::timer_begin();
    while( std::abs(area - area_prev) / 3 >= err_val ) {
        area_prev = area;
        area = 0.0;
        step = step / 2;

        #pragma omp parallel for reduction(+: area) schedule(runtime)
        for (int i = 0; i < (right - left) / step; i++) {
            area += func(left + step / 2 + i * step) * step;
        }
    }
    omp_estimator::timer_end();
    
    return area;
}


double get_integral_no_omp(double (*func)(double), double left, double right, double err_val) {
    double area = 0.0;
    double step = right - left;
    double area_prev = std::numeric_limits<double>::max();
    
    omp_estimator::timer_begin();
    while( std::abs(area - area_prev) / 3 >= err_val ) {
        area_prev = area;
        area = 0.0;
        step = step / 2;
        
        for (int i = 0; i < (right - left) / step; i++) {
            area += func(left + step / 2 + i * step) * step;
        }
    }
    omp_estimator::timer_end();
    
    return area;
}


int main(int argc, char* argv[]) {
    
    if (argc != 4)
        ::report_failure("Invalid number of arguments\n");
    
    double left_bound, right_bound, error_rate;
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
    }
    
    omp_estimator::PerformanceEstimator est;
    auto func = omp_enable_flag ? get_integral : get_integral_no_omp;

    while (!fin.eof()) {
        fin >> left_bound >> right_bound >> error_rate >> std::ws;
        
        if (fin.fail()) {
            fin.close();
            fout.close();
            ::report_failure("Unable to read from file");
        }

        if (error_rate <= 0) {
            fin.close();
            fout.close();
            ::report_failure("error_rate is out of range");
        }

        est.estimate(func, &f_x, left_bound, right_bound, error_rate);

        std::cout << "Time (" << thr_num << " thread(s)): "
                  << est.get_elapsed_time() << " ms\n";
        
        fout << std::any_cast<double>(est.get_return_value()) << '\n';
    };
    
    fin.close();
    fout.close();
    
    return 0;
}