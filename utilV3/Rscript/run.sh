#!/bin/sh

FILE=$1.csv
 
if [ -f $FILE ];
then
   rm $FILE
fi

AVFILE=$1.csv.avgrr

if [ -f $AVFILE ];
then
   rm $AVFILE
fi

counter=0
flname="/opt/synamatix/gss/TestGRR/"$1"/"$1"*.xml"
echo $flname
for file in $flname
do
if [ $counter -eq 0 ]; then
   Rscript run.R $file 1
else
   Rscript run.R $file 0
fi
counter=$((counter+1))
done

Rscript calaverage.R $1.csv



