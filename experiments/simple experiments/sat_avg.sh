#!/bin/bash

sum=0
count=0
num=0
for i in $(ls $@) 
do 
#	printf "$i\n"
	./sat "$i"
#	echo "$?"
	num="$?"
	sum=`expr $sum + $num`
  count=`expr $count + 1`
  if [ "$num" == 0 ]
	then
	        rm "$i"
	fi
done
#avg=`expr $sum/$count`
#printf "Avg= $avg \n"
RESULT=$(awk "BEGIN {printf \"%.2f\",${sum}/${count}}")
printf "$RESULT \n"
exit 0