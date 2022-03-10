#pragma once
#include <chrono>
#include <memory>
#include <functional>
#include <any>

namespace omp_estimator {
    
    class PerformanceEstimator {
    private:
        int num_iterations;

        double avg_duration = -1;
        std::any return_value;

        static std::chrono::_V2::steady_clock::time_point begin;
        static std::chrono::_V2::steady_clock::time_point end;
    public:
        PerformanceEstimator(int iterations = 100): num_iterations(iterations) {};
        
        template <class T, class... Args>
        void estimate(T (*callable)(Args...), Args... args);
        
        template <class T>
        void set_return_value(std::function<T()> &callable);
        void set_return_value(std::function<void()> &callable) { return; };

        double get_elapsed_time();
        std::any get_return_value();
        
        inline friend void timer_begin();
        inline friend void timer_end();
    };


    template <class T, class... Args>
    void PerformanceEstimator::estimate(T (*callable)(Args...), Args... args) {
        int64_t execution_time = 0;
        
        // Standardizing callable
        std::function<T()> func = std::bind(callable, std::forward<Args>(args)...);
        set_return_value(func);
        
        // Warming up
        for (int i = 0; i < 10; i++) {
            func();
        }

        // Estimation
        for (int i = 0; i < num_iterations; i++) {
            func();
            execution_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
        }

        // Averaging and conversion from nanosec to millisec
        this->avg_duration = static_cast<double>(execution_time) / num_iterations / 1000000;
    };
    
    
    template <class T>
    void PerformanceEstimator::set_return_value(std::function<T()> &callable) {
        this->return_value = callable();
    };


    inline void timer_begin() {
        omp_estimator::PerformanceEstimator::begin = std::chrono::_V2::steady_clock::now();
    }

    inline void timer_end() {
        omp_estimator::PerformanceEstimator::end = std::chrono::_V2::steady_clock::now();
    }


}   // end of namespace omp_estimator
