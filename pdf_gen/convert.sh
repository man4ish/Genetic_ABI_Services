#!/bin/sh
sed ':a; N; $!ba; s| *&lt;/p&gt;\n *&lt;ul&gt;|\\\\[0.1\\baselineskip]\\begin{minipage}{\\textwidth}\\begin{itemize}|g' $1|sed ':a; N; $!ba; s| *&lt;/p&gt;\n *&lt;p&gt;|\\newline\\newline|g' |sed -e 's|&amp;mdash;|\\textemdash{}|g' | perl -pe  's|&lt;strong&gt;(.*?)&lt;/strong&gt;|\\textbf{\1}|g'| perl -pe  's|&lt;em&gt;(.*?)&lt;/em&gt;|\\textit{\1}|g'  | perl -pe  's|&lt;br /&gt;|\\newline |g' |perl -pe  's|&lt;span style="text-decoration: underline;"&gt;(.*?)&lt;/span&gt;|\\underline{\1}|g' |perl -p -i -e 's/&amp;gt;/\\textgreater/g' |perl -p -i -e 's/&amp;lt;/\\textless/g'|sed  -e 's|&lt;ul&gt;|\\\\[0.1\\baselineskip]\\begin{minipage}{\\textwidth}\\begin{itemize}|g' -e 's|&lt;/ul&gt;|\\end{itemize}\\end{minipage}\\\\[0.5\\baselineskip]|g' -e 's|&lt;li&gt;|\\item |g' -e 's|&lt;/li&gt;||g' -e 's|&lt;ol&gt;|\\begin{enumerate}|g' -e 's|&lt;/ol&gt;|\\end{enumerate}|g' -e 's|&lt;p&gt;||g' -e 's|&lt;/p&gt;|\\newline|' -e '$!N;s|\(&lt;\)\(strong&gt;\)\([^\n]*\)\n\s*\1/\2|\\textbf{\3}|;P;D'  > $1.fmt

