#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LEN 16

int word_length = 0;
char* excluded_chars = NULL;
char* wordlist = NULL;
char* pattern = NULL;

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
void find_word(void)
{
	FILE* wordlist_stream = fopen(wordlist, "r");
	char* current_word = calloc(MAX_WORD_LEN, sizeof(char));
	bool excluded_char_found;
	bool pattern_found;
	int i;
	int j;
	while (fgets(current_word, MAX_WORD_LEN, wordlist_stream))
	{
		excluded_char_found = false;
		pattern_found = true;
		if ((strlen(current_word) == word_length && current_word[word_length - 1] != '\n')
			|| (strlen(current_word) == word_length + 1 && current_word[word_length] == '\n'))
		{
			if (excluded_chars != NULL)
			{
				i = 0;
				while (!excluded_char_found && i < word_length)
				{
					j = 0;
					while (!excluded_char_found && j < strlen(excluded_chars))
					{
						excluded_char_found = current_word[i] == excluded_chars[j];
						j++;
					}
					i++;
				}
				if (!excluded_char_found && pattern == NULL)
				{
					printf("Word found: %s", current_word);
				}
				else if (!excluded_char_found)
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

	fclose(wordlist_stream);
}

int main(int argc, char* argv[])
{
	clock_t prg_start = clock();
	read_arguments(argc, argv);
	// debug
	printf("\nword_len: %d\nexluded_chars: %s\nword_list: %s\npattern: %s\n\n",
			word_length, excluded_chars, wordlist, pattern);
	find_word();
	clock_t prg_end = clock();
	printf("total time to find word(s): %.2f Seconds", (float)(prg_end - prg_start) / CLOCKS_PER_SEC);
	return EXIT_SUCCESS;
}
