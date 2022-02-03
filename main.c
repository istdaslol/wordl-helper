#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LEN 16

/**
 * Global variables to store arguments passed by to user, and used by every function
 */
int word_length = 0;
int matches = 0;
char* excluded_chars = NULL;
char* included_chars = NULL;
char* wordlist = NULL;
char* pattern = NULL;
char* included_chars_missing = NULL;

/**
 * prints the help message
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
		   " -k <chars> chars that are known to exists  within the word but not position for pattern\n"
		   " -l <list_name> name of the textfile with the wordlist\n");
}

/**
 * Parses the user arguments and sets global variables
 * May also exit the program with error message if there is faulty input
 * @param argc number of arguments
 * @param argv vector with strings of arguments
 */
static void read_arguments(int argc, char* argv[])
{
	// Loop over every argument
	int i;
	for (i = 1; i < argc; i++)
	{
		// if it starts with an '-' it's an option and start checken wich one it is
		if (argv[i][0] == '-')
		{
			// -h
			if (argv[i][1] == 'h')
			{
				print_help();
				exit(EXIT_FAILURE);
			}
				// -c check also if there is a next entry and that it isn't an option
			else if (argv[i][1] == 'c' && i + 1 < argc && argv[i + 1][0] != '-')
			{
				i++;
				word_length = atoi(argv[i]);
			}

				// -p check also if there is a next entry and that it isn't an option
			else if (argv[i][1] == 'p' && i + 1 < argc && argv[i + 1][0] != '-')
			{
				i++;
				pattern = argv[i];
			}
				// -x check also if there is a next entry and that it isn't an option
			else if (argv[i][1] == 'x' && i + 1 < argc && argv[i + 1][0] != '-')
			{
				i++;
				excluded_chars = argv[i];
			}
				// -k check also if there is a next entry and that it isn't an option
			else if (argv[i][1] == 'k' && i + 1 < argc && argv[i + 1][0] != '-')
			{
				i++;
				included_chars = argv[i];
			}
				// -l check also if there is a next entry and that it isn't an option
			else if (argv[i][1] == 'l' && i + 1 < argc && argv[i + 1][0] != '-')
			{
				i++;
				wordlist = argv[i];
			}
				// if no match was found print error message and exit
			else
			{
				fprintf(stderr, "Unknown parameter %s", argv[i]);
				exit(EXIT_FAILURE);
			}
		}
	}
	/**
	 * Checks the input after every argument was parsed to allow random order
	 */
	// check if wordlist was passed
	if (wordlist == NULL)
	{
		fprintf(stderr, "missing wordlist\n");
		print_help();
		exit(EXIT_FAILURE);
	}
	// check if word_length was passed and is greater than 0
	if (word_length <= 0)
	{
		fprintf(stderr, "no or 0 count entered\n");
		exit(EXIT_FAILURE);
	}
	// check if word_length doesn't exceed the max_word_length
	if (word_length > MAX_WORD_LEN)
	{
		fprintf(stderr, "char-count greater than %d\n", MAX_WORD_LEN);
		exit(EXIT_FAILURE);
	}
	// as for now check if pattern is the same length as the word_length
	if (pattern != NULL && strlen(pattern) != word_length)
	{
		fprintf(stderr, "pattern longer that char-count\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * Checks if a given char is in the excluded list
 * @param curr_char character to be tested
 * @return true if it is in the list, false otherwise
 */
bool is_excluded_char(const char* curr_char)
{
	int i = 0;
	while (excluded_chars[i] != '\0')
	{
		if (excluded_chars[i] == *curr_char)
		{
			return true;
		}
		i++;
	}
	return false;
}

/**
 * checks if a given char in in the included list and replaces in in the list so no duplicates are a positive check
 * @param curr_char character to be tested
 * @return true if in the list, false otherwise
 */
bool is_included_char(const char* curr_char)
{
	int i = 0;
	while (included_chars_missing[i] != '\0')
	{
		if (included_chars_missing[i] == *curr_char)
		{
			included_chars_missing[i] = '_';
			return true;
		}
		i++;
	}
	return false;
}

/**
 * checks if a given String applies to all filters given by the user
 * @param word String to be tested
 * @return true if the word is valid, false otherwise
 */
bool test_word(const char* word)
{
	int i = 0;
	int included_chars_count = 0;

	bool is_valid = true;

	if (included_chars != NULL)
	{
		included_chars_missing = malloc(26 * sizeof(char));
		strcpy(included_chars_missing, included_chars);
	}

	while (word[i] != '\0' && word[i] != '\n' && is_valid)
	{
		if (pattern != NULL) // pattern was submitted
		{
				if (pattern[i] == '_') // position is a wildcard
				{
					if (excluded_chars != NULL)
					{
						is_valid = !(is_excluded_char(&word[i]));
					}

					if (is_valid && included_chars != NULL)
					{
						included_chars_count += is_included_char(&word[i]) ? 1 : 0;
					}
				}
				else // position has to be a given char
				{
					is_valid = word[i] == pattern[i];
				}
		}

		else // no pattern just check chars
		{
			if (excluded_chars != NULL)
			{
				is_valid = !(is_excluded_char(&word[i]));
			}

			if (included_chars != NULL)
			{
				included_chars_count += is_included_char(&word[i]) ? 1 : 0;
			}
		}

		i++;
		// check if word isn't larger than the maximum length
		if (is_valid)
		{
			is_valid = i <= word_length;
		}
	}

	// check if word is the needed length
	if (is_valid)
	{
		is_valid = i == word_length;
	}

	// check if all included chars were found
	if (is_valid && included_chars != NULL)
	{
		is_valid = included_chars_count == strlen(included_chars);
	}

	return is_valid;
}

/**
 * reads every line of the wordlist and checks if the word matches the criteria
 * if so, it's printed otherwise nothing happens
 */
void find_word(void)
{
	FILE* wordlist_stream = fopen(wordlist, "r");
	char* current_word = calloc(MAX_WORD_LEN, sizeof(char));

	// check if file could be opened and memory for the word storage was allocated
	if (wordlist_stream == NULL)
	{
		fprintf(stderr, "Couldn't open Wordlist-file %s", wordlist);
		exit(EXIT_FAILURE);
	}
	if (current_word == NULL)
	{
		fprintf(stderr, "Couldn't allocate memory");
		exit(EXIT_FAILURE);
	}

	bool test_success;
	/**
	 * reads a word from file and starts checking consecutive the entered criteria
	 * if all match the word gets printed, otherwise nothing
	 */
	while (fgets(current_word, MAX_WORD_LEN, wordlist_stream))
	{
		test_success = test_word(current_word);

		if (test_success)
		{
			matches++;
			printf("Word found: %s", current_word);
		}
	}

	fclose(wordlist_stream);
}

int main(int argc, char* argv[])
{
	// start timer for total runtime
	clock_t prg_start = clock();

	// parse arguments
	read_arguments(argc, argv);

	// debug info
	printf("\nword_len: %d\nexcluded_chars: %s\nincluded_chars: %s\npattern: %s\nword_list: %s\n\n",
			word_length, excluded_chars, included_chars, pattern, wordlist);

	// find word
	find_word();

	// stop timer for total runtime
	clock_t prg_end = clock();
	// print total runtime timer
	printf("total time to find %d word(s): %.2f Seconds", matches, (float)(prg_end - prg_start) / CLOCKS_PER_SEC);

	return EXIT_SUCCESS;
}
