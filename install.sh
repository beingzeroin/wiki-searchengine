#!/bin/bash 
# Script by Phinfinity <rndanish@gmail.com>
# Roll Number : 201001085


if [ "$#" -eq '0' -o "$#" -gt '2'  ]
then
	echo "Usage : $0 path_to_wiki_xml [output_xml_path]" 1>&2
	exit 2
fi

INPUT_XML=$(readlink -f $1)
if [ "$#" -eq '2' ]
then
	OUTPUT_XML=$(readlink -f $2)
else
	OUTPUT_XML="searchindex.dat"
fi

if [ -L $0 ] ; then
	SCRIPT_DIR=$(dirname $(readlink -f $0)) ;
else
	SCRIPT_DIR=$(dirname $0) ;
fi ;
cd $SCRIPT_DIR
#Check for PV command
PV="cat"
command -v pv > /dev/null && PV="pv"

make clean
make
echo "Removing any previous temporary files"
rm -f tempfile-*;
time $PV $INPUT_XML | ./xmlparse2 | ./tokenizer
time ./mergeindex $OUTPUT_XML
echo "Index Complete removing temporary files"
rm -f tempfile-*;
echo "Index Saved to $OUTPUT_XML"
