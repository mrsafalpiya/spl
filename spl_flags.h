/*
 * spl_flags.h - https://github.com/mrsafalpiya/spl
 * no warranty implied; use at your own risk

 * See end of file for license information.
*/

/*
 * ================
 * | Introduction |
 * ================
 *
 * SPL_FLAGS.H - GNU-style argument parsing with golang like functions
 *
 * Author: Safal Piya
*/

/*
 * Take a look at the "Tips for scanning the library" section of the `README.md`
 * file for some tips and tricks to scan this library header file quickly and
 * easily
*/

/*
 * =========
 * | Usage |
 * =========
 *
 * Do this:
 *
 *         #define SPL_FLAGS_IMPLEMENTATION
 *
 * before you include this file in *one* C or C++ file to create the
 * implementation.
 *
 * Define the SPL_FLAGS_MAX_C to the number of flags you are going to have
 * The default is 128
*/

/*
 * ====================
 * | Header-file mode |
 * ====================
*/

#ifndef SPL_FLAGS_H
#define SPL_FLAGS_H

#include <stdlib.h>

/* = global variables = */
static char *non_flag_arguments[512];
static int non_flag_arguments_c = 0;

static char *non_defined_flags[512];
static int non_defined_flags_c = 0;

/* = functions = */
void
spl_flags_toggle(int *value, char short_hand, const char *long_hand, const char *info);

void
spl_flags_int(int *value, char short_hand, const char *long_hand, const char *info);

void
spl_flags_str(char **value, char short_hand, const char *long_hand, const char *info);

void
spl_flags_parse(int argc, char **argv);

#endif /* SPL_FLAGS_H */

/*
 * ===================
 * | Implementations |
 * ===================
*/

#ifdef SPL_FLAGS_IMPLEMENTATION

/* constants */
#ifndef SPL_FLAGS_MAX_C
#define SPL_FLAGS_MAX_C 128
#endif

#define ARG_SHORT_NON_EQUAL  1
#define ARG_SHORT_EQUAL      2
#define ARG_LONG_NON_EQUAL   3
#define ARG_LONG_EQUAL       4

/* enums */
typedef enum {
	TYPE_TOGGLE,
	TYPE_INT,
	TYPE_STR,
} flag_type;

/* unions */
typedef union {
	int   value_int;
	char  *value_str;
} flag_value;

/* structs */
typedef struct {
	flag_type   type;
	char        short_hand;
	const char  *long_hand;
	const char  *info;
	void        *value;
	flag_value  value_default;
} flag;

/* global variables */
static flag flags[SPL_FLAGS_MAX_C];
static int flags_c;

/* functions */
void
new_flag(void *value, char short_hand, const char *long_hand, const char *info)
{
	++flags_c;

	flags[flags_c - 1].short_hand = short_hand;
	flags[flags_c - 1].long_hand = long_hand;
	flags[flags_c - 1].value = value;
	flags[flags_c - 1].info = info;
}

void
spl_flags_toggle(int *value, char short_hand, const char *long_hand, const char *info)
{
	new_flag((void *)value, short_hand, long_hand, info);

	flags[flags_c - 1].type = TYPE_TOGGLE;
	flags[flags_c - 1].value_default.value_int = *value;
}

void
spl_flags_int(int *value, char short_hand, const char *long_hand, const char *info)
{
	new_flag((void *)value, short_hand, long_hand, info);

	flags[flags_c - 1].type = TYPE_INT;
	flags[flags_c - 1].value_default.value_int = *value;
}

void
spl_flags_str(char **value, char short_hand, const char *long_hand, const char *info)
{
	new_flag((void *)value, short_hand, long_hand, info);

	flags[flags_c - 1].type = TYPE_STR;
	flags[flags_c - 1].value_default.value_str = *value;
}

/*
 * Checks if the given argv is a valid defined flag
 *
 * Make sure the argv points to the data after the dash -
 *
 * Returns the type of argument it is: ARG_SHORT_NON_EQUAL, ARG_SHORT_EQUAL,
 * ARG_LONG_NON_EQUAL, ARG_LONG_EQUAL
 *
 * Returns 0 if it isn't a defined argument
*/
int
is_valid_defined_flag(char *argv, char short_hand, char const *long_hand)
{
	char *long_hand_str;

	if (argv[0] == short_hand) {
		if (argv[1] == '=')
			return ARG_SHORT_EQUAL;
		return ARG_SHORT_NON_EQUAL;
	}

	long_hand_str = strdup(long_hand);
	if (argv[0] == '-' && (strcmp(argv + 1, long_hand_str) == 0))
		return ARG_LONG_NON_EQUAL;

	strcat(long_hand_str, "=");
	if (argv[0] == '-' && (strcmp(argv + 1, long_hand_str) == 0))
		return ARG_LONG_EQUAL;

	return 0;
}

void
spl_flags_parse(int argc, char **argv)
{
	char *cur_argv;
	flag *cur_flag;
	int is_found;

	for (int i = 1; i < argc; i++) {
		cur_argv = argv[i];
		/* check if the current argument isn't a flag */
		if (cur_argv[0] != '-') {
			++non_flag_arguments_c;
			non_flag_arguments[non_flag_arguments_c - 1] = argv[i];

			continue;
		}

cur_argv_next_char:
		++cur_argv;
		for (int j = 0; j < flags_c; j++) {
			is_found = 0;

			cur_flag = flags + j;
			int flag_type = is_valid_defined_flag(cur_argv,
				cur_flag->short_hand, cur_flag->long_hand);

			switch (flag_type) {
			case ARG_SHORT_NON_EQUAL:
			case ARG_LONG_NON_EQUAL:
				switch (cur_flag->type) {
				case TYPE_TOGGLE:
					*((int *)cur_flag->value) =
						!*((int *)cur_flag->value);
					switch (flag_type) {
					case ARG_SHORT_NON_EQUAL:
						/* check if secondary flags are
						 * available in the current argv */
						if (*(cur_argv + 1) != '\0') {
							++cur_argv;
							continue;
						}
					default:
						break;
					}
					break;
				case TYPE_INT:
					*((int *)cur_flag->value) =
						atoi(argv[++i]);
					break;
				case TYPE_STR:
					*((char **)cur_flag->value) =
						(void *)(argv[++i]);
				}

				is_found = 1;
				break;
			case ARG_SHORT_EQUAL:
			case ARG_LONG_EQUAL:
				cur_argv = strchr(cur_argv, '='); /* Correct me if I'm wrong but it SHOULD return a pointer to the = char. Right? */
				++cur_argv; /* point to the actual data */

				switch (cur_flag->type) {
				case TYPE_TOGGLE:
					continue; /* a toggle type cannot reach this code */
				case TYPE_INT:
					*((int *)cur_flag->value) =
						atoi(cur_argv);
					break;
				case TYPE_STR:
					*((char **)cur_flag->value) =
						(void *)(cur_argv);
				}

				++i;
				is_found = 1;

				break;
			default:
				continue;
			}

			if (is_found)
				break;
		}

		/* handle flag-type but non-defined arguments */
		if (!is_found) {
			++non_defined_flags_c;

			/* check if the argument is a short or a long type */
			if (argv[i][1] == '-') {
				non_defined_flags[non_defined_flags_c - 1] =
					argv[i];
			} else {
				non_defined_flags[non_defined_flags_c - 1] =
					strdup(cur_argv);
				non_defined_flags[non_defined_flags_c - 1][1] =
					'\0';
				if (*(cur_argv + 1) != '\0')
					goto cur_argv_next_char;
			}
		}
	}
}

#endif /* SPL_FLAGS_IMPLEMENTATION */

#ifdef SPL_FLAGS_DEBUG

#include <stdio.h>

/*
 * Print all the current flags defined, it's details and it's current value
 *
 * Also prints all the non-flag arguments and non-defined flag arguments
*/
void
spl_flags_debug_print(FILE *restrict stream)
{
	fprintf(stream, "= DEFINED FLAGS =\n");
	for (int i = 0; i < flags_c; i++) {
		fprintf(stream, "#%d of %d:\n", i+1, flags_c);

		fprintf(stream, "Info: %s\n", flags[i].info);
		fprintf(stream, "Type: ");

		switch (flags[i].type) {
		case TYPE_TOGGLE:
			fprintf(stream, "Toggle");
			break;
		case TYPE_INT:
			fprintf(stream, "Number");
			break;
		case TYPE_STR:
			fprintf(stream, "String");
		}

		fprintf(stream, "\tShort hand: %c\tLong hand: %s\n",
			flags[i].short_hand, flags[i].long_hand);

		switch (flags[i].type) {
		case TYPE_TOGGLE:
			fprintf(stream, "'%d' (Default: '%d')",
				*((int *)flags[i].value),
				flags[i].value_default.value_int);
			break;
		case TYPE_INT:
			fprintf(stream, "'%d' (Default: '%d')",
				*((int *)flags[i].value),
				flags[i].value_default.value_int);
			break;
		case TYPE_STR:
			fprintf(stream, "'%s' (Default: '%s')",
				*((char **)flags[i].value),
				flags[i].value_default.value_str);
		}

		fprintf(stream, "\n\n");
	}

	fprintf(stream, "= NON-FLAG ARGUMENTS =\n");
	for (int i = 0; i < non_flag_arguments_c; i++) {
		fprintf(stream, "'%s'\n", non_flag_arguments[i]);
	}
	fprintf(stream, "\n");

	fprintf(stream, "= NON-DEFINED FLAGS PASSED =\n");
	for (int i = 0; i < non_defined_flags_c; i++) {
		fprintf(stream, "'%s'\n", non_defined_flags[i]);
	}
}

#endif /* SPL_FLAGS_DEBUG */

/*
 * ===============================================
 * | License - Public Domain (www.unlicense.org) |
 * ===============================================
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
*/
