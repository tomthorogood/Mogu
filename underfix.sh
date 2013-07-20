LISTING=`grep -rl __ src/*`

for f in $LISTING
do
    vi $f
done
