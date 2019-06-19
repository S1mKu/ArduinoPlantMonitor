#!/bin/bash

NO_ARGS=0
MAX_ARGS=2

USAGE="Use this script in order to integrate an arduino sketch as library into an existing arduino sketchbook.\n\n USAGE: tolib <SKETCH_DIR> (<SKETCHBOOK_DIR>) \n\t- SKETCH_DIR \t\tthe sketch to turn into a lib\n\t- SKETCHBOOK_DIR \t(optional) the sketchbook where the integration of the sketch as library takes place. Defaults to the current working directory of the user."

# check for arg count
# check whether correct directories are found
# 1. check first arg - needs to be dir
# 2. check second arg
#    - no second arg: does current working directory contain dir 'libraries'?
#    - with second arg: is second arg dir and does it contain dir 'libraries'?
if [[ $# -eq "$NO_ARGS" ]] || [[ $# -gt "$MAX_ARGS" ]]; then
	echo -e $USAGE
	exit 0
elif ! [[ -d "$1" ]] || ! [[ -e "$1/$(basename "$1").ino" ]]; then
	echo "... 3"
	exit 0
elif [[ $# -eq "1" ]] && ! [[ -d "libraries" ]]; then
	echo "... 4"
	exit 0
elif [[ $# -eq "2" ]] && ! [[ -d "$(dirname "$2")/$(basename "$2")/libraries" ]]; then
	echo "... 5"
	exit 0
fi

SKETCH_DIR=$1
SKETCH_NAME=$(basename "$1")
SKETCHBOOK_DIR=${2:-"."}

echo -e \
"-----------------------------------------------\n"\
	"Sketch directory: \t"$SKETCH_DIR"\n"\
	"Sketch name: \t\t"$SKETCH_NAME"\n"\
	"Sketchbook directory: \t"$SKETCHBOOK_DIR"\n"\
"-----------------------------------------------\f"

if [ -d "$SKETCHBOOK_DIR/libraries/$SKETCH_NAME" ]; then
	read -p "SKETCHBOOK_DIR/libraries/$SKETCH_NAME/ already exists, continuing would override all duplicate files. Do you want to proceed (y/n)? " CHOICE
	case "$CHOICE" in
		y|Y ) 	echo -e "\nCONTINUING\f";;
		n|N ) 	echo -e "\nABORT"
				exit 0;;
		* 	) 	echo -e "\nInvalid input! TERMINATING PROGRAM"
				exit 0;;
	esac
fi

cp -rv $SKETCH_DIR $SKETCHBOOK_DIR/libraries/

if ! [ -d "$SKETCHBOOK_DIR/libraries/$SKETCH_NAME/examples" ]; then
	mkdir "$SKETCHBOOK_DIR/libraries/$SKETCH_NAME/examples"
fi

mv -v $SKETCHBOOK_DIR/libraries/$SKETCH_NAME/*.ino \
      $SKETCHBOOK_DIR/libraries/$SKETCH_NAME/examples/
