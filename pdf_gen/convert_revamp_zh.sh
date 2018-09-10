#!/bin/sh
#toplessitemize
sed ':a; N; $!ba; s| *&lt;br \/&gt;\n *&lt;ul&gt;|\\begin{mylist}[beforeskip=-7pt,afterskip=-13pt]|g' $1 |sed ':a; N; $!ba; s| *&lt;/p&gt;\n *&lt;ul&gt;|\\begin{mylist}[beforeskip=-7pt,afterskip=-13pt]|g' |sed  -e 's|&lt;ul&gt;|\\begin{mylist}[beforeskip=-7pt,afterskip=-13pt]|g' |sed ':a; N; $!ba; s| *&lt;/p&`gt;\n *&lt;p&gt;|~\\\\~\\\\ \n|g' |sed -e 's|&amp;mdash;|\\textemdash{}|g' | perl -pe  's|&lt;strong&gt;(.*?)&lt;/strong&gt;|\\textbf{\1}|g'| perl -pe  's|&lt;em&gt;(.*?)&lt;/em&gt;|\\textit{\1}|g'  | perl -pe  's|&lt;br /&gt;|~\\\\ \n|g' |perl -pe  's|&lt;span style="text-decoration: underline;"&gt;(.*?)&lt;/span&gt;|\\underline{\1}|g' |perl -p -i -e 's/&amp;gt;/\\textgreater/g' |perl -p -i -e 's/&amp;lt;/\\textless/g'|sed  -e 's|&lt;ul&gt;|\\begin{mylist}[beforeskip=-7pt,afterskip=-13pt]|g' -e 's|&lt;/ul&gt;|\\end{mylist}|g' -e 's|&lt;li&gt;|\\item |g' -e 's|&lt;/li&gt;||g' -e 's|&lt;ol&gt;|\\begin{enumerate}|g' -e 's|&lt;/ol&gt;|\\end{enumerate}|g' -e 's|&lt;p&gt;||g' -e 's|&lt;/p&gt;|\\newline\n |' -e '$!N;s|\(&lt;\)\(strong&gt;\)\([^\n]*\)\n\s*\1/\2|\\textbf{\3}|;P;D'|sed 's|\\newline<\/Text>|</Text>|g' > $1.fmt
#sed -i 's/%/\\%/g' $1.fmt

