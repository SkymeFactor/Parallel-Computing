#!/bin/bash

EXEC=./build/omp_lab2.elf
DATA_FOLDER=data
OUTPUT_FILE=$DATA_FOLDER/out.bin
INPUT_FILE=$DATA_FOLDER/lena.pgm

passed=0
total_counter=0


assert() {
    local expected="$1"
    local actual="$2"
    local msg="${3-}"

    ((total_counter+=1))

    if [ "$expected" == "$actual" ]; then
        echo "[ SUCCESS ]: Test case TC_${total_counter}"
        ((passed+=1))
    else
        echo "[ FAILURE ]: Test case TC_${total_counter}"
        [ "${#msg}" -gt 0 ] && echo "$msg"
    fi
}



echo "GROUP: multi-threading consistency"

# POSITIVE: single thread, no omp, TC_1
$EXEC $INPUT_FILE $OUTPUT_FILE -1 1> /dev/null
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "True"

# POSITIVE: single thread, omp, TC_2
$EXEC $INPUT_FILE $OUTPUT_FILE 1 1> /dev/null
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "True"

# POSITIVE: 4 threads, TC_3
$EXEC $INPUT_FILE $OUTPUT_FILE 4 1> /dev/null
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "True"

# POSITIVE: all threads, TC_4
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1> /dev/null
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "True"



echo; echo "GROUP: images consistency"

# POSITIVE: baboon.pgm == baboon.pgm, TC_5
INPUT_FILE=$DATA_FOLDER/baboon.pgm
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1> /dev/null
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "True"

# POSITIVE: pepper.pgm == pepper.pgm, TC_6
INPUT_FILE=$DATA_FOLDER/pepper.pgm
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1> /dev/null
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "True"

# POSITIVE: lena.pgm == lena.pgm, TC_7
INPUT_FILE=$DATA_FOLDER/lena.pgm
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1> /dev/null
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "True"

# NEGATIVE: lena.pgm == pepper.pgm, TC_8
INPUT_FILE=$DATA_FOLDER/lena.pgm
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1> /dev/null
INPUT_FILE=$DATA_FOLDER/pepper.pgm
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "False"

# NEGATIVE: lena.pgm == baboon.pgm, TC_9
INPUT_FILE=$DATA_FOLDER/lena.pgm
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1> /dev/null
INPUT_FILE=$DATA_FOLDER/baboon.pgm
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "False"

# NEGATIVE: baboon.pgm == pepper.pgm, TC_10
INPUT_FILE=$DATA_FOLDER/baboon.pgm
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1> /dev/null
INPUT_FILE=$DATA_FOLDER/pepper.pgm
assert "$(python3 ./tools/build_and_check_histogram.py $INPUT_FILE $OUTPUT_FILE)" "False"



echo; echo "Total tests passed: ${passed} / ${total_counter}"

# Return success or failure if script is sourced
if [ "$(basename $0)" != "sanity_check_black-box.bash" ]; then
    if [ "$passed" -eq "$total_counter" ]; then
        return 0
    else
        return 1
    fi
fi
