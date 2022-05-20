#include "omp_estimator.h"


namespace omp_estimator {

    std::chrono::_V2::steady_clock::time_point PerformanceEstimator::begin;
    std::chrono::_V2::steady_clock::time_point PerformanceEstimator::end;


    double PerformanceEstimator::get_elapsed_time() {
        return avg_duration;
    };

    std::any PerformanceEstimator::get_return_value() {
        return return_value;
    }
    

}   // end of namespace omp_estimator