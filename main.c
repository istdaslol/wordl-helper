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
 * checks if a given word is the length stored in the global variable word_length
 * accounts for '\n' at the end of the string
 * @param word that needs to be checked
 * @return  true if is the needed length, false otherwise
 */
bool test_len(char* word)
{
	return ((strlen(word) == word_length && word[word_length - 1] != '\n')
			|| (strlen(word) == word_length + 1 && word[word_length] == '\n'));
}

/**
 * checks if a given word has any of the excluded chars entered at start
 * @param word that is going to be checked
 * @return true if no excluded chars were found, false otherwise
 */
bool test_excluded_chars(const char* word)
{
	bool excluded_char_found = false;
	int i = 0;
	int j;

	while (!excluded_char_found && i < word_length)
	{
		j = 0;
		while (!excluded_char_found && j < strlen(excluded_chars))
		{
			excluded_char_found = word[i] == excluded_chars[j];
			j++;
		}
		i++;
	}
	return !(excluded_char_found);
}

/**
 * checks if a word has the entered pattern
 * @param word that's going to be checked
 * @return true if the pattern was found, false otherwise
 */
bool test_pattern(const char* word)
{
	bool pattern_found = true;
	int i = 0;

	while (pattern_found && i < word_length)
	{
		if (pattern[i] == '_')
		{
			i++;
			continue;
		}
		else
		{
			pattern_found = word[i] == pattern[i];
		}
		i++;
	}
	return pattern_found;
}

/**
 * checks if a given word has all of the included chars entered at start
 * @param word that's going to be checked
 * @return true if ALL chars were found, false otherwise
 */
bool test_included_chars(char* word)
{
	bool included_char_found;
	int i = 0;
	int j;
	int included_chars_count = 0;
	while (i < strlen(included_chars))
	{
		j = 0;
		included_char_found = false;
		while (j < strlen(word) && !included_char_found)
		{
			included_char_found = included_chars[i] == word[j];
			j++;
		}
		i++;
		included_chars_count += (included_char_found) ? 1 : 0;
	}

	return included_chars_count == i;
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
		test_success = test_len(current_word);

		if (test_success && excluded_chars != NULL)
		{
			test_success = test_excluded_chars(current_word);;
		}

		if (test_success && included_chars != NULL)
		{
			test_success = test_included_chars(current_word);
		}

		if (test_success && pattern != NULL)
		{
			test_success = test_pattern(current_word);
		}

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
