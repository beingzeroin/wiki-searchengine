#!/bin/bash 
# Script by Phinfinity <rndanish@gmail.com>
if [[ "$#" -ne '1' ]]
then
	echo "Usage : $0 path_to_wiki_xml" 1>&2
	exit 2
fi
time pv enwiki-latest-pages-articles.xml | ./xmlparse2 | ./tokenizer
time ./mergeindex

