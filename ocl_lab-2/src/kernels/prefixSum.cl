#define LOG_NUM_BANKS 4
#define CONFLICT_FREE_OFFSET(n) ((n) >> (LOG_NUM_BANKS))

__kernel void
prefixSum(const __global float* input, __global float* output, __global float* block_sums) {
	// In order to store the sum, we need 513 elements + conflict free offset
	__local float temp[BLOCK_SIZE + 1 + CONFLICT_FREE_OFFSET(BLOCK_SIZE) + 1];

	const uint tx1 = get_local_id(0);						// The first element in range 0 - 256
	const uint tx2 = tx1 + (BLOCK_SIZE / 2);				// The second element in range 256 - 512
	const uint txd = 2 * tx1;								// tx1 doubled
	const uint idx1 = get_group_id(0) * BLOCK_SIZE + tx1;	// Global thread id for element 1
	const uint idx2 = idx1 + (BLOCK_SIZE / 2);				// Global thread id for element 2

	uint offset = 1;

	// Cache the computational window in shared memory
	temp[tx1 + CONFLICT_FREE_OFFSET(tx1)] = input[idx1];
	temp[tx2 + CONFLICT_FREE_OFFSET(tx2)] = input[idx2];

	// Build the sum in place up the tree
	for (uint d = BLOCK_SIZE >> 1; d > 0; d >>= 1) {
		barrier (CLK_LOCAL_MEM_FENCE);

		if (tx1 < d) {
			uint ai = offset * (txd + 1) - 1;
			uint bi = offset * (txd + 2) - 1;
			ai += CONFLICT_FREE_OFFSET(ai);
			bi += CONFLICT_FREE_OFFSET(bi);

			temp[bi] += temp[ai];
		}
		offset *= 2;
	}

	// Spare the last element
	if (tx1 == 0) {
		uint last_element_offset = BLOCK_SIZE - 1 + CONFLICT_FREE_OFFSET(BLOCK_SIZE - 1);
		block_sums[get_group_id(0)] = temp[last_element_offset];

		temp[BLOCK_SIZE + CONFLICT_FREE_OFFSET(BLOCK_SIZE)] = temp[last_element_offset];
		temp[last_element_offset] = 0;
	}

	// Traverse down the tree building the scan in place
	for (uint d = 1; d < BLOCK_SIZE; d *= 2) {
		offset >>= 1;
		barrier (CLK_LOCAL_MEM_FENCE);

		if (tx1 < d) {
			uint ai = offset * (txd + 1) - 1;
			uint bi = offset * (txd + 2) - 1;
			ai += CONFLICT_FREE_OFFSET(ai);
			bi += CONFLICT_FREE_OFFSET(bi);

			float tmp = temp[ai];
			temp[ai] = temp[bi];
			temp[bi] += tmp;
		}
	}

	barrier (CLK_LOCAL_MEM_FENCE);

	// write the results back to global memory

	output[idx1] = temp[tx1 + 1 + CONFLICT_FREE_OFFSET(tx1 + 1)];
	output[idx2] = temp[tx2 + 1 + CONFLICT_FREE_OFFSET(tx2 + 1)];
}



__kernel void
spreadBlockSums(__global float* data, const __global float* block_sums) {
	const uint idx = (get_group_id(0) + 1) * BLOCK_SIZE + get_local_id(0);
	
	// Missing synchronization step between blocks during the SCAN
	float prev_sum = 0;
	#pragma unroll
	for (uint i = 0; i <= get_group_id(0); ++i)
		prev_sum += block_sums[i];

	// Add the previous blocks reduction to elements in current block
	#pragma unroll
	for (uint i = 0; i < BLOCK_SIZE; i += 128) {
		data[idx + i] += prev_sum;
	}
}