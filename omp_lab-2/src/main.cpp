#include <iostream>
#include <omp.h>
#include <memory.h>

#include "omp_estimator.h"
#include "P5_Image.h"
#include "utilities.h"


Image_Hist compute_histogram(P5_Image img) {
    Image_Hist hist_result(img.max_val + 1, 0);

    omp_estimator::timer_begin();   // Start of the time measurement

    uint8_t* data = img.data.data();
    auto len = img.max_val + 1;
    std::size_t remainder = img.data.size() % 4;
    std::uint32_t hist[len * omp_get_max_threads()];
    memset(hist, 0, sizeof(std::uint32_t) * len * omp_get_max_threads());

    #pragma omp parallel
    {
        unsigned thr_offset = static_cast<unsigned>(len * omp_get_thread_num());

        #pragma omp for schedule(runtime)
        for (std::size_t i = 0; i < img.data.size() - remainder; i += 4) {
            ++hist[thr_offset + static_cast<unsigned>(data[i])];
            ++hist[thr_offset + static_cast<unsigned>(data[i + 1])];
            ++hist[thr_offset + static_cast<unsigned>(data[i + 2])];
            ++hist[thr_offset + static_cast<unsigned>(data[i + 3])];
        }
    }

    switch (remainder) {
        case 3: ++hist[data[img.data.size() - 3]];
        case 2: ++hist[data[img.data.size() - 2]];
        case 1: ++hist[data[img.data.size() - 1]];
        case 0: break;
    }
    
    remainder = hist_result.size() % 4;
    for (int thr = 0; thr < omp_get_max_threads(); ++thr) {
        std::size_t thr_offset = thr * len;
        
        for (std::size_t i = 0; i < hist_result.size() - remainder; i += 4) {
            hist_result[i] += hist[thr_offset + i];
            hist_result[i + 1] += hist[thr_offset + i + 1];
            hist_result[i + 2] += hist[thr_offset + i + 2];
            hist_result[i + 3] += hist[thr_offset + i + 3];
        }
        
        switch (remainder) {
            case 3: hist_result[hist_result.size() - 3] += hist[thr_offset + hist_result.size() - 3];
            case 2: hist_result[hist_result.size() - 2] += hist[thr_offset + hist_result.size() - 2];
            case 1: hist_result[hist_result.size() - 1] += hist[thr_offset + hist_result.size() - 1];
            case 0: break;
        }
    }

    omp_estimator::timer_end();     // End of the time measurement

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