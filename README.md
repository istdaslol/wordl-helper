# wordl-helper
Programm that runs through a wordlist to find matches to a given input

# How to use
-c <count> the amount of chars (char-count) needed, up to MAX_WORD_LEN  
-p <pattern> known arrangement of chars _ as wildcard _ro__ as example. Cant exceed the char-count, as by now it has to be the same length as char-count  
-x <chars> chars that should be excluded as a following order -x aeiou to exclude all vowels  
-l <list_name> name of the textfile with the wordlist  

  the list_name is necessary, count has a default of 16, exclude and pattern are optional.
MAX_WORD_LEN  is by default 128, if no count is given as an argument it'll read up to 128 chars per line otherwise the entered count
