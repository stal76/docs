#!/bin/bash

extensions_del="aux;log;out;synctex.gz;toc"

deleted_files=0
for ext in $(echo $extensions_del | tr ";" "\n")
do
	for fname in $(find . -name "*.$ext")
	do
		echo "    delete: $fname"
		rm $fname
		deleted_files=$(($deleted_files+1))
	done
done
echo "Deleted files: $deleted_files"

updated_files=0
for dirname in $(ls -1)
do
	if [ -d $dirname ]
	then
		work_pdf="$dirname/$dirname.pdf"
		final_pdf="../$dirname.pdf"
		if [ -f $work_pdf ]
		then
			if [ -f $final_pdf ]
			then
				diff -b $work_pdf $final_pdf > /dev/null
				if [ $? -eq 0 ]
				then
					continue
				fi
			fi
			echo "    update: $work_pdf -> $final_pdf"
			cp $work_pdf $final_pdf
			updated_files=$(($updated_files+1))
		fi
	fi
done
echo "Updated files: $updated_files"
