#!/bin/bash 
# Script by Phinfinity <rndanish@gmail.com>
if [[ "$#" -ne '1' ]]
then
	echo "Usage : $0 path_to_wiki_xml" 1>&2
	exit 2
fi
echo "Removing any previous temporary files"
rm -f tempfile-*;
time pv $1 | ./xmlparse2 | ./tokenizer
time ./mergeindex
echo "Index Complete removing temporary files"
rm -f tempfile-*;


