# ShellInterfaceFileLocatorC <br />
<pre>
-findstuff.c is a file written in C that displays a shell and allows for commands to find files. <br />
-These commands include: <br />
  -find <filename> -f:ABC -s => find the given filename, both f and s are optional arguments. <br />
  -find <”text”> -f:ABC -s => find the given string in a file, both f and s are optional arguments.<br />
    -f:txt => using the -f:ABC flag tells the program to only looks at or in files with the ABC extension.<br />
    -s => using the -s flag tells the program to look in all sub-directories of the current directory for the string or filename.<br />
  -list => lists all running child processes and what they try to do. Also displays their serial number.<br />
  -kill <num>  => kills a child process, and so ends its finding attemps. In order to get the num argument, use list.<br />
  -quit or q => quits the program and all child processes immediately.<br />
  
  <br />
  <br />
  
-Included in this repository are a few .txt files I used for testing as well as a sub-directory I used for testing.<br />

Example Inputs:

list                        => list the child processes.
kill 1                      => kill the child process with serial number 1.
quit                        => quits the program.
q                           => quits the program.

find findstuff.c            => look for the file findstuff.c in the current directory.
find findstuff.c -t:c       => does the same thing as the previous line (file extension already specified).
find textfile.txt -s        => look for the file textfile.txt in all sub-directories and the current one.
find textfile.txt -t:txt -s => ultimately does the same thing as the previous line (file ending already specified).

find "word"                 => look for the string "word" in all files in the current directory.
find "word" -t:txt          => looks for the string "word" in the current directory, but only in files that have the extension txt.
find "word" -s              => look for the string "word" in all sub-directories and the current one.
find "word" -t:txt -s       => looks for the string "word" in all sub-directories and the current one, but only in files that have the extension txt.
  
  
  

<pre>
