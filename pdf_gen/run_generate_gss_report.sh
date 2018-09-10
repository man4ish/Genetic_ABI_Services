#!/bin/sh
# run.sh <InputFile> <QR Code> <CRD File> 

# resolve links
PRG="$0"
while [ -h "$PRG" ] ; do
	ls=`ls -ld "$PRG"`
	link=`expr "$ls" : '.*-> \(.*\)$'`
	if expr "$link" : '.*/.*' > /dev/null; then
		PRG="$link"
	else
		PRG=`dirname "$PRG"`/"$link"
	fi
done

PRGDIR=`dirname "$PRG"`

mkdir ./Input
infile=$1
QRfile=$2
CRDfile=$3


perl $PRGDIR/parse.pl $infile $CRDfile
R -e "Sweave('$PRGDIR/GSS_Report.Rnw')" ./Input/DiseaseSummary.txt ./Input/Infile.txt ./Input/disease_risk $QRfile ./Input/Sample_Information.txt $PRGDIR ./Input bar
R CMD pdflatex GSS_Report.tex

# clean up

len=${#infile} 
texfile=${infile:0:$len-4} 
mv GSS_Report.pdf $texfile.pdf
rm *aux *log *tex
rm -r ./Input
#rm $PRGDIR/image/barplot* $PRGDIR/image/population.pdf

