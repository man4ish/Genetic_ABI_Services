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

CHINESE_LANG="<Language>zh</Language>"

sed -i 's/\r//g' $infile

tmp=$(grep $CHINESE_LANG $1)
if [ -n "$tmp" ]; then
   $PRGDIR/convert_revamp_zh.sh $infile
else
   $PRGDIR/convert_revamp.sh $infile
fi

R -e "Sweave('$PRGDIR/GSS_Report_Revamp.Rnw')" $infile $filterflag 0
sed '/^$/d' GSS_Report_Revamp.tex > out
mv out GSS_Report_Revamp.tex
R CMD pdflatex GSS_Report_Revamp.tex
R -e "Sweave('$PRGDIR/GSS_Report_Revamp.Rnw')" $infile $filterflag 1
sed -i 's/\\usepackage{Sweave}/%\\usepackage{Sweave}/' GSS_Report_Revamp.tex
sed '/^$/d' GSS_Report_Revamp.tex > out
mv out GSS_Report_Revamp.tex
R CMD pdflatex GSS_Report_Revamp.tex
R -e "Sweave('$PRGDIR/GSS_Report_Revamp.Rnw')" $infile $filterflag 1
sed -i 's/\\usepackage{Sweave}/%\\usepackage{Sweave}/' GSS_Report_Revamp.tex
sed '/^$/d' GSS_Report_Revamp.tex > out
mv out GSS_Report_Revamp.tex
R CMD pdflatex GSS_Report_Revamp.tex
R CMD pdflatex GSS_Report_Revamp.tex

#R CMD pdflatex GSS_Report_Revamp.tex
# clean up
#echo $infile
len=${#infile} 

#echo $len
texfile=${infile:0:$len-4}

#echo $texfile
#mv -f GSS_Report_Revamp.pdf $texfile.pdf
pdftk GSS_Report_Revamp.pdf output $texfile.pdf owner_pw foopass allow printing

rm *aux *log  *toc #GSS_Report_Revamp.pdf
rm -r ./Input ./tmp

