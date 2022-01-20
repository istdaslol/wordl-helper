# wordl-helper
Program that runs through a wordlist to find matches to a given input

# How to use
-c \<count\> the amount of chars (char-count) needed, up to 16  
-p \<pattern\> known arrangement of chars '_' as wildcard '\_ro__' as example. Can't exceed the char-count,  as by now it has to be the same length as char-count  
-x \<chars\> chars that should be excluded as a following order -x aeiou to exclude all vowels  
-k \<chars\> chars that are known to exists  within the word but not position for pattern  
-l \<list_name\> name of the textfile with the wordlist  

  the list_name and count are necessary, excluded, included and pattern are optional.

# Wordlist
the included list is very old but works fast and has most of what's needed  
I recomend using an own, newer list like  
-> https://github.com/dwyl/english-words/
