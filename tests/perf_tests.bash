#!/bin/bash

EXEC=./build/omp_lab2.elf
DATA_FOLDER=data
OUTPUT_FILE=$DATA_FOLDER/out.bin


INPUT_FILE=$DATA_FOLDER/lena.pgm
TEST_RESULTS=$DATA_FOLDER/perf_test_chunk_size.txt



echo "[ INFO ] evaluating performance per chunk_size; $TEST_RESULTS"

# Static, chunk 1
echo "[Static, chunk_1]" > $TEST_RESULTS
export OMP_SCHEDULE=static,1
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS

# Static, chunk 2
echo "[Static, chunk_2]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,2
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS

# Static, chunk 3
echo "[Static, chunk_3]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,3
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS

# Static, chunk 4
echo "[Static, chunk_4]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,4
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS

# Static, chunk 8
echo "[Static, chunk_8]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,8
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS

# Static, chunk 16
echo "[Static, chunk_16]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,16
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS

# Static, chunk default
echo "[Static, chunk_inf]" >> $TEST_RESULTS
export OMP_SCHEDULE=static
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS



INPUT_FILE=$DATA_FOLDER/lena.pgm
TEST_RESULTS=$DATA_FOLDER/perf_test.txt


echo "[ INFO ] evaluating performance; $TEST_RESULTS"

# All Threads static
echo "[All thr, static]" > $TEST_RESULTS
export OMP_SCHEDULE=static
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS

# All Threads dynamic
echo "[All thr, dynamic]" >> $TEST_RESULTS
export OMP_SCHEDULE=dynamic
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS

# All Threads guided
echo "[All thr, guided]" >> $TEST_RESULTS
export OMP_SCHEDULE=guided
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1>>$TEST_RESULTS

# 4 Threads
echo "[static, 4_thr]" >> $TEST_RESULTS
export OMP_SCHEDULE=static
$EXEC $INPUT_FILE $OUTPUT_FILE 4 1>>$TEST_RESULTS

# Single Thread
echo "[static, 1_thr]" >> $TEST_RESULTS
export OMP_SCHEDULE=static
$EXEC $INPUT_FILE $OUTPUT_FILE 1 1>>$TEST_RESULTS

# Single Thread, OMP disabled
echo "[1_thr, no_omp]" >> $TEST_RESULTS
$EXEC $INPUT_FILE $OUTPUT_FILE -1 1>>$TEST_RESULTS
