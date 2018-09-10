#!/usr/bin/perl
open(fh, $ARGV[0]) or die "could not open file";
while($input=<fh>)
{
  @a=split("\t",$input);
  if($a[0] < 10)
  {
    print($a[3]."_".$a[4]."_0".$a[0],"\t$input");
  }
  else {
    print($a[3]."_".$a[4]."_".$a[0],"\t$input");
  }
}
close(fh);
