#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LEN 16

/**
* Global variavles with default value
*/
int word_length = 0;
char* excluded_chars = NULL;
char* wordlist = NULL;
char* pattern = NULL;

/**
* function to print a help message
*/
static void print_help(void)
{
	printf("wordle <arguments>\n"
		   " -c <count> the amount of chars (char-count) needed, up to 16\n"
		   " -p <pattern> known arrangement of chars _ as wildcard\n"
		   "              _ro__ as example. Cant exceed the char-count\n"
		   "			  as by now it has to be the same length as char-count\n"
		   " -x <chars> chars that should be excluded as a following order\n"
		   "            -x aeiou to exclude all vowels\n"
		   " -l <list_name> name of the textfile with the wordlist\n");
}

/**
* function to parse the arguments and also check them
* exits with error if arguments were wrong / missing
*/
static void read_arguments(int argc, char* argv[])
{
	int i;
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'h')
			{
				print_help();
				exit(EXIT_SUCCESS);
			}
			else if (argv[i][1] == 'c' && argv[i + 1][0] != '-')
			{
				if (i + 1 < argc)
				{
					i++;
					word_length = atoi(argv[i]);
				}
			}
			else if (argv[i][1] == 'p' && argv[i + 1][0] != '-')
			{
				if (i + 1 < argc)
				{
					i++;
					pattern = argv[i];
				}
			}
			else if (argv[i][1] == 'x' && argv[i + 1][0] != '-')
			{
				if (i + 1 < argc)
				{
					i++;
					excluded_chars = argv[i];
				}
			}
			else if (argv[i][1] == 'l')
			{
				if (i + 1 < argc && argv[i + 1][0] != '-')
				{
					i++;
					wordlist = argv[i];
				}
			}
			else
			{
				fprintf(stderr, "Unknown parameter %s", argv[i]);
			}
		}
	}
	if (wordlist == NULL)
	{
		fprintf(stderr, "missing wordlist\n");
		print_help();
		exit(EXIT_FAILURE);
	}
	if (word_length <= 0)
	{
		fprintf(stderr, "no or 0 count entered\n", MAX_WORD_LEN);
		exit(EXIT_FAILURE);
	}
	if (word_length > MAX_WORD_LEN)
	{
		fprintf(stderr, "char-count greater than %d\n", MAX_WORD_LEN);
		exit(EXIT_FAILURE);
	}
	if (pattern != NULL && strlen(pattern) != word_length)
	{
		fprintf(stderr, "pattern longer that char-count\n");
		exit(EXIT_FAILURE);
	}
}

/**
* function that finds word(s) according to the arguments
*/
static void find_word(void)
{
	// open file
	FILE* wordlist_stream = fopen(wordlist, "r");
	// allocate memory for the max possible word-size and wrote it 0 
	char* current_word = calloc(MAX_WORD_LEN, sizeof(char));
	
	// declare variables needed
	bool excluded_char_found;
	bool pattern_found;
	int i;
	int j;
	
	// while EOF isn't reached read every line at max_word_len from file
	while (fgets(current_word, MAX_WORD_LEN, wordlist_stream))
	{
		// initialize variables
		excluded_char_found = false;
		pattern_found = true;
		
		// if the word is long enough, need to check for \n at the end for valid results
		if ((strlen(current_word) == word_length && current_word[word_length - 1] != '\n')
			|| (strlen(current_word) == word_length + 1 && current_word[word_length] == '\n'))
		{
			// if excluded chare were passed
			if (excluded_chars != NULL)
			{
				// loop over every char in the word to check, terminate early if excluded char were found
				i = 0;
				while (!excluded_char_found && i < word_length)
				{
					// loop over every char in the charArr of excluded chars, terminate early if excluded char were found
					j = 0;
					while (!excluded_char_found && j < strlen(excluded_chars))
					{
						// check if char is one of the excluded ones
						excluded_char_found = current_word[i] == excluded_chars[j];
						j++;
					}
					i++;
				}
				// if no excluded chars were found and no pattern was passed print word else search for pattern
				if (!excluded_char_found && pattern == NULL)
				{
					printf("Word found: %s", current_word);
				}
				else if (!excluded_char_found)
				{
					// loop over every char in the word and search for pattern
					i = 0;
					while (pattern_found && i < word_length)
					{
						// if char in pattern is a wildcard skip it
						if (pattern[i] == '_')
						{
							i++;
							continue;
						}
						else
						{
							pattern_found = current_word[i] == pattern[i];
						}
						i++;
					}
					// if the pattern was found within the word print it
					if (pattern_found)
					{
						printf("Word found: %s", current_word);
					}
				}
			}
			// if no excluded chars were passed check if a pattern was passed if so, check for pattern
			else if (pattern != NULL)
			{
				i = 0;
				while (pattern_found && i < word_length)
				{
					if (pattern[i] == '_')
					{
						i++;
						continue;
					}
					else
					{
						pattern_found = current_word[i] == pattern[i];
					}
					i++;
				}
				if (pattern_found)
				{
					printf("Word found: %s", current_word);
				}
			}
		}
	}
	// close file
	fclose(wordlist_stream);
}

int main(int argc, char* argv[])
{
	// start timer
	clock_t prg_start = clock();
	
	// parse arguments
	read_arguments(argc, argv);
	
	// debug info
	printf("\nword_len: %d\nexluded_chars: %s\nword_list: %s\npattern: %s\n\n",
			word_length, excluded_chars, wordlist, pattern);
	
	// search for word(s)
	find_word();
	
	// stop timer
	clock_t prg_end = clock();
	// print total time 
	printf("total time to find word(s): %.2f Seconds", (float)(prg_end - prg_start) / CLOCKS_PER_SEC);
	return EXIT_SUCCESS;
}
