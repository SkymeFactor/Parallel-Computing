#!/bin/bash

EXEC=./build/omp_lab1.elf
OUTPUT_FILE=data/out.txt
INPUT_FILE=data/test_input.txt

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
$EXEC /dev/stdin $OUTPUT_FILE -1 1> /dev/null <<< '0 3.14159265358979323846 0.00001'
assert "$(<$OUTPUT_FILE)" "-2.17757"

# POSITIVE: single thread, omp, TC_2
$EXEC /dev/stdin $OUTPUT_FILE 1 1> /dev/null <<< '0 3.14159265358979323846 0.00001'
assert "$(<$OUTPUT_FILE)" "-2.17757"

# POSITIVE: 4 threads, TC_3
$EXEC /dev/stdin $OUTPUT_FILE 4 1> /dev/null <<< '0 3.14159265358979323846 0.00001'
assert "$(<$OUTPUT_FILE)" "-2.17757"

# POSITIVE: all threads, TC_4
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '0 3.14159265358979323846 0.00001'
assert "$(<$OUTPUT_FILE)" "-2.17757"



echo; echo "GROUP: real file read consistency"

if [ ! -f "$INPUT_FILE" ]; then
    echo "0 3.14159265358979323846 0.00001" > $INPUT_FILE
fi

# POSITIVE: all threads, TC_5
$EXEC $INPUT_FILE $OUTPUT_FILE 0 1> /dev/null
assert "$(<$OUTPUT_FILE)" "-2.17757"



echo; echo "GROUP: float error_rate"

# POSITIVE: error_rate is 0.0000001, TC_6
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '0 3.14159265358979323846 0.0000001'
assert "$(<$OUTPUT_FILE)" "-2.17759"

# POSITIVE: error_rate is 0.00001, TC_7
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '0 3.14159265358979323846 0.00001'
assert "$(<$OUTPUT_FILE)" "-2.17757"

# POSITIVE: error_rate is 0.001, TC_8
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '0 3.14159265358979323846 0.001'
assert "$(<$OUTPUT_FILE)" "-2.17546"

# POSITIVE: error_rate is 0.1, TC_9
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '0 3.14159265358979323846 0.1'
assert "$(<$OUTPUT_FILE)" "-1.90539"



echo; echo "GROUP: int error_rate"

# POSITIVE: error_rate is 1  (integer), TC_10
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '0 3.14159265358979323846 1'
assert "$(<$OUTPUT_FILE)" "-1.08879"

# POSITIVE: error_rate is 10 (integer), TC_11
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '0 3.14159265358979323846 10'
assert "$(<$OUTPUT_FILE)" "-1.08879"



echo; echo "GROUP: valid interval"

# POSITIVE: interval is negative and valid, TC_12
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '-6 -4 0.00001'
assert "$(<$OUTPUT_FILE)" "-0.51071"

# POSITIVE: interval is positive and valid, TC_13
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '7 8 0.00001'
assert "$(<$OUTPUT_FILE)" "-0.11316"

# POSITIVE: trailing newline, TC_14
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '''1 2 0.00001

'''
assert "$(<$OUTPUT_FILE)" "-0.04550"



echo; echo "GROUP: invalid interval"

# POSITIVE: area is infinit within the negative plane, TC_15
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '-0.00001 3.14159265358979323846 0.00001'
assert "$(<$OUTPUT_FILE)" "-nan"

# POSITIVE: area is infinit within the positive plane, TC_16
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '0 3.1417 0.00001'
assert "$(<$OUTPUT_FILE)" "-nan"

# POSITIVE: area is infinit on both ends, TC_17
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '-0.00001 3.1417 0.00001'
assert "$(<$OUTPUT_FILE)" "-nan"

# POSITIVE: area is infinit on the whole extended, TC_18
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '4 6 0.00001'
assert "$(<$OUTPUT_FILE)" "-nan"

# POSITIVE: distance between left and right bounds is zero, TC_19
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null <<< '3.14159265358979323846 3.14159265358979323846 0.00001'
assert "$(<$OUTPUT_FILE)" "0.00000"



echo; echo "GROUP: invalid input"

# NEGATIVE: no arguments provided, TC_20
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null 2> $OUTPUT_FILE <<< ''
assert "$(<$OUTPUT_FILE)" "[ ERROR ]: Unable to read from file"

# NEGATIVE: not enough arguments, TC_21
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null 2> $OUTPUT_FILE <<< '0 3.14159265358979323846'
assert "$(<$OUTPUT_FILE)" "[ ERROR ]: Unable to read from file"

# NEGATIVE: invalid argument type, TC_22
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null 2> $OUTPUT_FILE <<< '0 3.14159265358979323846 err'
assert "$(<$OUTPUT_FILE)" "[ ERROR ]: Unable to read from file"

# NEGATIVE: error_rate is zero, TC_23
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null 2> $OUTPUT_FILE <<< '0 3.14159265358979323846 0'
assert "$(<$OUTPUT_FILE)" "[ ERROR ]: error_rate is out of range"

# NEGATIVE: error_rate is negative, TC_24
$EXEC /dev/stdin $OUTPUT_FILE 0 1> /dev/null 2> $OUTPUT_FILE <<< '0 3.14159265358979323846 -0.1'
assert "$(<$OUTPUT_FILE)" "[ ERROR ]: error_rate is out of range"



echo; echo "Total tests passed: ${passed} / ${total_counter}"

# Return success or failure if script is sourced
if [ "$(basename $0)" != "sanity_check_black-box.bash" ]; then
    if [ "$passed" -eq "$total_counter" ]; then
        return 0
    else
        return 1
    fi
fi
