#!/bin/bash

red=$(tput setaf 1)
green=$(tput setaf 2)
reset=$(tput sgr0)

# Make sure that we are in the root of this project
cd $(dirname ${BASH_SOURCE[0]})
cd ..

# Create necessarry directories
if [ ! -d "data" ]; then
    mkdir data
fi

# Make sure that we have all python modules installed
echo; echo "-- Python modules check --"; echo
python3 -m pip install -r requirements.txt

if [ "$?" -ne "0" ]; then
    echo; echo "[ ${red}ERROR${reset} ]: Cannot install required modules, aborted"
else
    echo; echo "[ ${green}OK${reset} ]: All modules checked"

    # Perform the sanity check black-box test
    if [ -e "./tests/sanity_check_black-box.bash" ]; then

        if [ -e "./data/lena.pgm" ] && [ -e "./data/baboon.pgm" ] && [ -e "./data/pepper.pgm" ]; then
            echo; echo "-- Sanity check --"; echo
            source ./tests/sanity_check_black-box.bash

            if [ "$?" -ne "0" ]; then
                echo; echo "[ ${red}ERROR${reset} ]: Sanity check failed, following results may not be reliable"
            else
                echo; echo "[ ${green}OK${reset} ]: All tests have completed successfully"
            fi
        else
            echo; echo "[ ${red}ERROR${reset} ]: Missing test image files"
        fi

    else
        echo; echo "[ ${red}ERROR${reset} ]: Missing file with functional tests script"
    fi

    # Execute performance evaluating
    if [ -e "./tests/sanity_check_black-box.bash" ]; then
        echo; echo "-- Performance measurement --"; echo
        source ./tests/perf_tests.bash

        if [ "$?" -ne "0" ]; then
            echo; echo "[ ${red}ERROR${reset} ]: Evaluation failed"
        else
            echo; echo "[ ${green}OK${reset} ]: Evaluation is complete"
        fi

    else
        echo; echo "[ ${red}ERROR${reset} ]: Missing file with performance measurement script"
    fi


    if [ -e "./tests/sanity_check_black-box.bash" ]; then
            # Interpret obtained results as charts
            echo; echo "-- Charts assembling --"; echo
            python3 ./tools/build_charts.py

            if [ "$?" -ne "0" ]; then
                echo; echo "[ ${red}ERROR${reset} ]: Charts has not been built"
            else
                echo; echo "[ ${green}OK${reset} ]: Charts has been assembled successfully"
            fi

    else
        echo; echo "[ ${red}ERROR${reset} ]: Missing file with charts building script or requirements.txt"
    fi
    
    echo; echo "-- Complete --"

fi