#!/bin/bash

# Generate a PNG image for state-machine found in object dump.
# This script needs plantuml.jar, which can be download at http://plantuml.com/download

if [ $# -lt 2 ]
then
    echo "Usage: $0 <objdump.txt> <output_folder>"
    exit
fi

# Script parameters
OBJ_FILE_PATH=$1
OUT_FOLDER_PATH=$2

# Config
BOSCE=./bosce
PLANTUML_JAR=plantuml.jar
FSM_LIST_FILENAME=fsm_list.txt

# Generate fsm list
${BOSCE} --list -O ${OBJ_FILE_PATH} | grep "^    .*" > ${OUT_FOLDER_PATH}/${FSM_LIST_FILENAME}

# Foreach fsm
while read s;
do
    echo "Processing $s"
    ${BOSCE} --stm $s -g plantuml -O ${OBJ_FILE_PATH} > "$OUT_FOLDER_PATH/$s.txt"
    cat "$s.txt" | java -jar ${PLANTUML_JAR} -tpng -pipe > "$OUT_FOLDER_PATH/$s.png"
done < ${OUT_FOLDER_PATH}/${FSM_LIST_FILENAME}
