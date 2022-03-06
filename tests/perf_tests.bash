#!/bin/bash

EXEC=./build/omp_lab1.elf
DATA_FOLDER=data
OUTPUT_FILE=$DATA_FOLDER/out.txt


INPUT_FILE='''0 3.14159265358979323846 0.000001'''
TEST_RESULTS=$DATA_FOLDER/perf_test_chunk_size.txt



echo "[ INFO ] evaluating performance per chunk_size; $TEST_RESULTS"

# Static, chunk 1
echo "[Static, chunk_1]" > $TEST_RESULTS
export OMP_SCHEDULE=static,1
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 2
echo "[Static, chunk_2]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,2
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 3
echo "[Static, chunk_3]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,3
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 4
echo "[Static, chunk_4]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,4
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 8
echo "[Static, chunk_8]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,8
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 16
echo "[Static, chunk_16]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,16
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk default
echo "[Static, chunk_inf]" >> $TEST_RESULTS
export OMP_SCHEDULE=static
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE



INPUT_FILE='''
0 3.14159265358979323846 0.1
0 3.14159265358979323846 0.01
0 3.14159265358979323846 0.001
0 3.14159265358979323846 0.0001
0 3.14159265358979323846 0.00001
0 3.14159265358979323846 0.000001
'''
TEST_RESULTS=$DATA_FOLDER/perf_test_chunk_size_dynamic.txt


echo "[ INFO ] evaluating performance per chunk_size dynamically; $TEST_RESULTS"

# Static, chunk 1
echo "[Static, chunk_1]" > $TEST_RESULTS
export OMP_SCHEDULE=static,1
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 2
echo "[Static, chunk_2]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,2
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 3
echo "[Static, chunk_3]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,3
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 4
echo "[Static, chunk_4]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,4
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 8
echo "[Static, chunk_8]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,8
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk 16
echo "[Static, chunk_16]" >> $TEST_RESULTS
export OMP_SCHEDULE=static,16
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# Static, chunk default
echo "[Static, chunk_inf]" >> $TEST_RESULTS
export OMP_SCHEDULE=static
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE



INPUT_FILE='''
0 3.14159265358979323846 0.1
0 3.14159265358979323846 0.01
0 3.14159265358979323846 0.001
0 3.14159265358979323846 0.0001
0 3.14159265358979323846 0.00001
0 3.14159265358979323846 0.000001
'''
TEST_RESULTS=$DATA_FOLDER/perf_test_dynamic.txt


echo "[ INFO ] evaluating performance dynamically; $TEST_RESULTS"

# All Threads static
echo "[All thr, static]" > $TEST_RESULTS
export OMP_SCHEDULE=static
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# All Threads dynamic
echo "[All thr, dynamic]" >> $TEST_RESULTS
export OMP_SCHEDULE=dynamic
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# All Threads guided
echo "[All thr, guided]" >> $TEST_RESULTS
export OMP_SCHEDULE=guided
$EXEC /dev/stdin $OUTPUT_FILE 0 1>>$TEST_RESULTS <<< $INPUT_FILE

# 4 Threads
echo "[4 thr, static]" >> $TEST_RESULTS
export OMP_SCHEDULE=static
$EXEC /dev/stdin $OUTPUT_FILE 4 1>>$TEST_RESULTS <<< $INPUT_FILE

# Single Thread
echo "[1 thr, static]" >> $TEST_RESULTS
export OMP_SCHEDULE=static
$EXEC /dev/stdin $OUTPUT_FILE 1 1>>$TEST_RESULTS <<< $INPUT_FILE

# Single Thread, OMP disabled
echo "[1 thr, no omp]" >> $TEST_RESULTS
$EXEC /dev/stdin $OUTPUT_FILE -1 1>>$TEST_RESULTS <<< $INPUT_FILE
