__kernel void matMulSimple(const uint heightA, const uint widthA, const uint widthB,
                        __global const float* matA,
                        __global const float* matB,
                        __global float* matC)
{
    const size_t idx = get_global_id(0);   // Column
    const size_t idy = get_global_id(1);   // Row
    if (idy >= heightA || idx >= widthB) return;
    
    float sum = 0.0f;
    const size_t matA_idy = idy * widthA;

    for (size_t k = 0; k < widthA; ++k) {
        sum += matA[matA_idy + k] * matB[k * widthB + idx];
    }

    matC[idy * widthB + idx] = sum;
}



__kernel void matMulBlocked(const uint heightA, const uint widthA, const uint widthB,
                        __global const float* matA,
                        __global const float* matB,
                        __global float* matC)
{
    const size_t tx = get_local_id(0);
    const size_t ty = get_local_id(1);
    const size_t idx = get_group_id(0) * BLOCK_SIZE + tx;
    const size_t idy = get_group_id(1) * BLOCK_SIZE + ty;

    __local float subA[BLOCK_SIZE][BLOCK_SIZE + 1];
    __local float subB[BLOCK_SIZE][BLOCK_SIZE + 1];

    float sum = 0.0f;
    const size_t matA_idy = idy * widthA;
    
    for (size_t block = 0; block < widthA; block += BLOCK_SIZE) {

        const size_t by = block + ty;
        const size_t bx = block + tx;
        
        if (idy < heightA && bx < widthA) {
            subA[ty][tx] = matA[matA_idy + bx];
        } else {
            subA[ty][tx] = 0.0f;
        }
        
        // Transpose block
        if (by < widthA && idx < widthB) {
            subB[tx][ty] = matB[by * widthB + idx];
        } else {
            subB[tx][ty] = 0.0f;
        }

        barrier(CLK_LOCAL_MEM_FENCE);

        #pragma unroll
        for (size_t k = 0; k < BLOCK_SIZE; ++k) {
            sum += subA[ty][k] * subB[tx][k];
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }
    
    if (idy < heightA && idx < widthB) {
        matC[idy * widthB + idx] = sum;
    }
}



#define VECTOR_SIZE 8
__kernel void matMulBlockedVectorized(const int heightA, const int widthA, const int widthB,
                        const __global float *matA,
                        const __global float *matB,
                        __global float *matC)
{
    const size_t tx = get_local_id(0);
    const size_t ty = get_local_id(1) * VECTOR_SIZE;

    const size_t idx = get_group_id(0) * BLOCK_SIZE + tx;
    const size_t idy = get_group_id(1) * BLOCK_SIZE + ty;

    __local float subA[BLOCK_SIZE][BLOCK_SIZE + 1];
    __local float subB[BLOCK_SIZE][BLOCK_SIZE + 1];

    float sum[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    for (size_t block = 0; block < widthA; block += BLOCK_SIZE) {
        
        const size_t by = block + ty;
        const size_t bx = block + tx;

        #pragma unroll
        for (size_t w = 0; w < VECTOR_SIZE; ++w) {
            if (idy + w < heightA && bx < widthA) {
                subA[ty + w][tx] = matA[(idy + w) * widthA + bx];
            } else {
                subA[ty + w][tx] = 0.0f;
            }

            // Transpose block
            if (by + w < widthA && idx < widthB) {
                subB[tx][ty + w] = matB[(by + w) * widthB + idx];
            } else {
                subB[tx][ty + w] = 0.0f;
            }
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    
        #pragma unroll
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            float tmp = subB[tx][k];
            sum[0] += subA[ty][k] * tmp;
            sum[1] += subA[ty + 1][k] * tmp;
            sum[2] += subA[ty + 2][k] * tmp;
            sum[3] += subA[ty + 3][k] * tmp;
            sum[4] += subA[ty + 4][k] * tmp;
            sum[5] += subA[ty + 5][k] * tmp;
            sum[6] += subA[ty + 6][k] * tmp;
            sum[7] += subA[ty + 7][k] * tmp;
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (idx < widthB) {
        size_t w_max = (heightA - idy) > VECTOR_SIZE ? VECTOR_SIZE : (heightA - idy);
        
        #pragma unroll
        for (size_t w = 0; w < w_max; ++w)
            matC[(idy  + w) * widthB + idx] = sum[w];
    }
}
#undef VECTOR_SIZE
