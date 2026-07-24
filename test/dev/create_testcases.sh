#!/usr/bin/bash

## Bulk-create testcases

set -euo pipefail


## Safely create testcases for a single class
create_testcase()
{
    local filenames

    filenames=$(ruby create_testcase.rb $@ 2>&1) || {
        printf "Error: File creation failed\n" >&2
        return 1
    }

    printf "%s\n" "${filenames}"
}


## Create all testcases
create_all_testcases()
{
	local filename="$1"
	local outfile=""

	while IFS='' read -r -a LINE;
	do
		for FILE in TESTCASE_template_*.cpp ;
		do
			outfile="${LINE##*CLASS=}.${FILE##TESTCASE_template_}"
			create_testcase "${FILE} ${outfile} ${LINE[@]}"
		done

	done < "${filename}"
}

## -----------------------------------------------------------------------------

[[ -r testcases_list.txt ]] || {
	printf "No testcases_list.txt found\n";
	return 1;
}

## Generate XML output for function lists
doxygen Doxyfile

printf "%s\n" "CREATE FILES:"

## Bulk-create testcases for all classes
create_all_testcases testcases_list.txt

