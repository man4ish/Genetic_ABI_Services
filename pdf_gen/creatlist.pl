#!/usr/bin/perl
open (LFILE, '>lists.tiex');
for ($i=0; $i< 50; $i++)
{
   print LFILE "$i\n";
} 
close (LFILE); 
