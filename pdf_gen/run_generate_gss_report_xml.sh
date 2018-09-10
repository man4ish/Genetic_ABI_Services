#!/bin/bash
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
mkdir ./tmp
infile=$1

$PRGDIR/convert.sh $1

R -e "Sweave('$PRGDIR/GSS_MultiLingual_Report.Rnw')" $1.fmt 0
sed -i 's/\\usepackage{Sweave}/%\\usepackage{Sweave}/' GSS_MultiLingual_Report.tex
R CMD pdflatex GSS_MultiLingual_Report.tex

# rename pdf and clean up
len=${#infile} 
texfile=${infile:0:$len-4}
mv -f GSS_MultiLingual_Report.pdf $texfile.pdf
rm *aux *log *tex
#rm -r ./Input ./tmp
#rm $PRGDIR/image/barplot* $PRGDIR/image/population.pdf

if [ "$2" -eq "1" ]   #generate Interpret Report
then
   R -e "Sweave('$PRGDIR/GSS_MultiLingual_Report.Rnw')" $1.fmt $2
   sed -i 's/\\usepackage{Sweave}/%\\usepackage{Sweave}/' GSS_MultiLingual_Report.tex
   R CMD pdflatex GSS_MultiLingual_Report.tex

   # rename pdf and clean up
   len=${#infile}
   texfile=${infile:0:$len-4}
   mv -f GSS_MultiLingual_Report.pdf $texfile.interpretor.pdf
   rm *aux *log *tex
fi
