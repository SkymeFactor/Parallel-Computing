#include <iostream>
#include <omp.h>

#include "omp_estimator.h"
#include "P5_Image.h"
#include "utilities.h"


Image_Hist compute_histogram(P5_Image img) {
    Image_Hist hist_result(img.max_val + 1, 0);
    
    omp_estimator::timer_begin();
    #pragma omp parallel
    {
        Image_Hist _hist_private(img.max_val + 1, 0);

        #pragma omp for schedule(runtime)
        for (size_t i = 0; i < img.data.size(); ++i) {
            _hist_private[img.data[i]]++;
        }

        #pragma omp critical
        {
            for (auto i = 0; i < hist_result.size(); ++i)
                hist_result[i] += _hist_private[i];
        }
    }
    omp_estimator::timer_end();

    return hist_result;
}


Image_Hist compute_histogram_no_omp(P5_Image img) {
    Image_Hist hist_result(img.max_val + 1, 0);
    
    omp_estimator::timer_begin();
    for (size_t i = 0; i < img.data.size(); ++i) {
        hist_result[img.data[i]]++;
    }
    omp_estimator::timer_end();

    return hist_result;
}


int main(int argc, char* argv[]) {
    if (argc != 4)
        ::report_failure("Invalid number of arguments\n");

    bool omp_enable_flag = true;
    int thr_num = std::atoi(argv[3]);
    
    if (thr_num < -1)
        ::report_failure("Invalid number of threads\n");
    
    switch (thr_num) {
        case -1: omp_enable_flag = false; break;
        case 0: break;
        default: omp_set_num_threads(thr_num); break;
    }
    
    auto func = omp_enable_flag ? compute_histogram : compute_histogram_no_omp;
    auto img = read_P5_image_from_file(argv[1]);
    omp_estimator::PerformanceEstimator est;

    est.estimate(func, img);
    auto ret = std::any_cast<Image_Hist>(est.get_return_value());
    
    save_histogram(argv[2], ret);
    std::cout << "Time (" << thr_num << " thread(s)): "
                  << est.get_elapsed_time() << " ms\n";

    return 0;
}