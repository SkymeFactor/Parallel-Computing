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
        if (idx < widthB && by < widthA) {
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

    float8 sum = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    for (size_t block = 0; block < widthA; block += BLOCK_SIZE) {
        
        const size_t by = block + ty;
        const size_t bx = block + tx;

        #pragma unroll
        for (size_t w = 0; w < VECTOR_SIZE; w++) {
            if (idy + w < heightA && bx < widthB) {
                subA[ty + w][tx] = matA[(idy + w) * widthA + bx];
            } else {
                subA[ty + w][tx] = 0.0f;
            }

            if (idx < widthB && by + w < widthA) {
                subB[tx][ty + w] = matB[(by + w) * widthB + idx];
            } else {
                subB[tx][ty + w] = 0.0f;
            }
        }

        barrier(CLK_LOCAL_MEM_FENCE);

        #pragma unroll
        for (int k = 0; k < BLOCK_SIZE; ++k) {
            sum.s0 += subA[ty + 0][k] * subB[tx][k];
            sum.s1 += subA[ty + 1][k] * subB[tx][k];
            sum.s2 += subA[ty + 2][k] * subB[tx][k];
            sum.s3 += subA[ty + 3][k] * subB[tx][k];
            sum.s4 += subA[ty + 4][k] * subB[tx][k];
            sum.s5 += subA[ty + 5][k] * subB[tx][k];
            sum.s6 += subA[ty + 6][k] * subB[tx][k];
            sum.s7 += subA[ty + 7][k] * subB[tx][k];
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (idx < widthB) {
        if (idy + 0 < heightA)
            matC[(idy  + 0) * widthB + idx] = sum.s0;
        if (idy + 1 < heightA)
            matC[(idy  + 1) * widthB + idx] = sum.s1;
        if (idy + 2 < heightA)
            matC[(idy  + 2) * widthB + idx] = sum.s2;
        if (idy + 3 < heightA)
            matC[(idy  + 3) * widthB + idx] = sum.s3;
        if (idy + 4 < heightA)
            matC[(idy  + 4) * widthB + idx] = sum.s4;
        if (idy + 5 < heightA)
            matC[(idy  + 5) * widthB + idx] = sum.s5;
        if (idy + 6 < heightA)
            matC[(idy  + 6) * widthB + idx] = sum.s6;
        if (idy + 7 < heightA)
            matC[(idy  + 7) * widthB + idx] = sum.s7;
    }
}
#undef VECTOR_SIZE
