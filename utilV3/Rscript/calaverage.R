cmd<-commandArgs()
cmd<-strsplit(cmd," ")
InFile<-cmd[[6]][1]
file<-read.csv(InFile, sep="\t", header=TRUE)
cat(paste("Name","AvgRR",sep="\t"),file=paste(InFile,".avgrr",sep=""),append=TRUE,"\n")

for (rs in 2:ncol(file)) {
   cat(paste(names(file)[rs], mean(file[,rs],na.rm=T),sep="\t"),file=paste(InFile,".avgrr",sep=""),append=TRUE,"\n")
}

