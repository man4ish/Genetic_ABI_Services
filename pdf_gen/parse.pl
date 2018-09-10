#!/usr/bin/perl
%hash = ();

open(fh,$ARGV[0]) or die "could not open data file\n";
open fp, ">./Input/Infile.txt" or die $!;
open sampleinfo, ">./Input/Sample_Information.txt" or die $!;
%diseasehash=();
%drughash=();
while($input = <fh>)
{
  chomp($input);
  @a= split("\t",$input);
  if($a[0] !~ /#/)
  {
      $hash{$a[0]} = $input;
      print fp "$input\n";
  }
  elsif($input =~ /Summary:Disease/)
  {
    chomp($input);
    $input=~s/#Summary:Disease	//;
    @parray = split("\t",$input);
    $diseasehash{$parray[0]} = $input;
  }
  elsif($input =~ /Summary:Trait/)
  {
    print($input);
    chomp($input);
    $input=~s/#Summary:Trait	//;
    @parray = split("\t",$input);
    $diseasehash{$parray[0]} = $input;
  }
  else
  {
    print sampleinfo "$input\n";
  }
}
close(fh);
close(fp);
close(sampleinfo);

################################################################

open(fh2,$ARGV[1]) or die "could not open coh file\n";
open cfile, ">./Input/disease_risk" or die $!;
while($input2 = <fh2>)
{
  #chomp($input2);
  @a2= split("\t",$input2);
  if($hash{$a2[1]} ne "")
  {
    print cfile  "$a2[1]\t$a2[2]\t$a2[3]\t$a2[4]\n";
  }
}
close(fh);
close(cfile);

##################################################################
open DiseaseFile, ">./Input/DiseaseSummary.txt" or die $!;
@pop2darray;
 for my $population ( reverse sort keys %diseasehash ) {
        @disarray = split("\t",$diseasehash{$population});        
        print DiseaseFile "$disarray[0]\t$disarray[1]\t$disarray[3]\t$disarray[4]\n";
    }
close DiseaseFile;

