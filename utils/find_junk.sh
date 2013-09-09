#!/bin/bash

all_files=`git ls-files .`
for f in $all_files
do
    if [ -e $f ]
    then
        result=`head -1 $f | grep "GITIGNORE"`
    else
        result=""
    fi
    if [ $result ]
    then
        echo "File $f is listed as if it should be ignored, but was committed."
        echo "Try: echo \"$f\" >> .gitignore && git rm --cached $f"
        exit 1
    fi
done
exit 0
