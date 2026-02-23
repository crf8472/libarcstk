#!/usr/bin/env bash

## Get all strings in double quotes througout the code in src/.

grep -rn "\"[^\"]\+\"" src | grep -v "^src/version\.cpp" | grep -v ":[0-9]\+:\s*\([#\*]\)" | grep "\".*[a-z]\+.*\"" > texts.txt

