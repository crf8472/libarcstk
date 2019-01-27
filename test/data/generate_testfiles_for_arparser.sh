#!/bin/bash


## This script generates input files for all testcases of ARParser.
## Each copy of the specified base file will be broken at a specific position
## to test the parsers ability to handle the error correct.
##
## Requirements: coreutils (head, tail)


## Configuration

## Input file to generate the broken testfiles
FILE=dBAR-015-001b9178-014be24e-b40d2d0f.bin

## Blocks in this file are 148 bytes long each (header+triplets)
## 13 bytes header + 15 tracks * 9 bytes/track
(( block=148 ))


## Constants

## ARBlock headers are 13 bytes long
(( header_len=13 ))

## ARTriplets are 9 bytes long
(( triplet_len=9 ))


## --- BEGIN


## Generate the 13 Datasets broken Within the Header.
## 'H' is start of the header - {0..13} bytes so each break position is created.
## Note that 13 means that triplets are expected after the complete header but
## none are there.
(( counter=1 ))
while [[ $counter -le $header_len ]]
do
	## Create first block intact, then add an incomplete header of the
	## second block
	(( bytes=$block+$counter ))

	NAME=$(printf "%0${#header_len}d" $counter)
	OUTFILE="${FILE%.bin}_H+$NAME.bin"

	printf "H+$NAME bytes (%${#header_len}d bytes total): $OUTFILE\n" $bytes

	/usr/bin/head -c "$bytes" "$FILE" > "$OUTFILE"

	((counter++))
done

## Generate the 9 Datasets broken Within a Triplet.
## 'T' is start of a triplet + {0..9} bytes so each break position is created.
## Note that 0 means that a triplet is expected but none is there.
(( counter=$triplet_len ))
while [[ $counter -ge 1 ]]
do
	## Create first block intact, then add a second block with complete header
	## but broken triplets
	(( bytes=$block-$counter+$block ))

	(( offset_T=$triplet_len-$counter ))
	OUTFILE="${FILE%.bin}_T+$offset_T.bin"

	printf "T+$offset_T  bytes ($bytes bytes total): $OUTFILE\n"

	/usr/bin/head -c "$bytes" "$FILE" > "$OUTFILE"

	((counter--))
done

## END

