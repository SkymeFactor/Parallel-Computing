#include <iostream>
#include <omp.h>

int main(int argc, char* argv[]) {
    #pragma omp parallel
    {
        #pragma omp critical
        std::cout << "Thread " << omp_get_thread_num() << '\n';
    }
    
    return 0;
}