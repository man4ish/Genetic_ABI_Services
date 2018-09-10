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
filterflag=$2

sed -i 's/\r//g' $infile
$PRGDIR/convert_revamp.sh $infile


R -e "Sweave('$PRGDIR/GSS_ReportV3_manish.Rnw')" $infile $filterflag 0
sed '/^$/d' GSS_ReportV3_manish.tex > out
mv out GSS_ReportV3_manish.tex
R CMD pdflatex GSS_ReportV3_manish.tex
R -e "Sweave('$PRGDIR/GSS_ReportV3_manish.Rnw')" $infile $filterflag 1
sed -i 's/\\usepackage{Sweave}/%\\usepackage{Sweave}/' GSS_ReportV3_manish.tex
sed '/^$/d' GSS_ReportV3_manish.tex > out
mv out GSS_ReportV3_manish.tex
R CMD pdflatex GSS_ReportV3_manish.tex
R -e "Sweave('$PRGDIR/GSS_ReportV3_manish.Rnw')" $infile $filterflag 1
sed -i 's/\\usepackage{Sweave}/%\\usepackage{Sweave}/' GSS_ReportV3_manish.tex
sed '/^$/d' GSS_ReportV3_manish.tex > out
mv out GSS_ReportV3_manish.tex
R CMD pdflatex GSS_ReportV3_manish.tex
R CMD pdflatex GSS_ReportV3_manish.tex

# clean up

len=${#infile} 

#echo $len
texfile=${infile:0:$len-4}

#mv -f GSS_ReportV3_manish.pdf $texfile.pdf
pdftk GSS_ReportV3_manish.pdf output $texfile.pdf owner_pw foopass allow printing

rm *aux *log  *toc GSS_ReportV3_manish.pdf
rm -r ./Input ./tmp

