cmd<-commandArgs()
cmd<-strsplit(cmd," ")
print(cmd[[6]][1])
InFile<-cmd[[6]][1]
titleflag<-cmd[[7]][1]
library(XML)
doc = xmlTreeParse(paste(InFile,sep=""), useInternal = TRUE, encoding="UTF-8")
top = xmlRoot(doc)
PatientSampleIdValue<-xmlValue(top[["GeneralInformation"]][["SampleId"]])
art = top[["Summary"]][["Results"]]
data<-xmlSApply(art, function(x) xmlSApply(x, xmlValue))
tilte<-""
title<-"SampleID"
relativerisk<-""
relativerisk<-PatientSampleIdValue
ChipType<-xmlValue(top[["GeneralInformation"]][["ChipType"]])
for (rs in 1:ncol(data)) {
   disname<-xmlGetAttr(top[["Summary"]][["Results"]][rs][["Result"]],"Name")
   title<-paste(title,disname,sep="\t")
   relativerisk<-paste(relativerisk,data[4,rs],sep="\t")
}

if(titleflag==1) {
   cat(title,file=paste(ChipType,".csv",sep=""),append=TRUE,"\n")
}
cat(relativerisk,file=paste(ChipType,".csv",sep=""),append=TRUE,"\n")



