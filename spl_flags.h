/*
 * spl_flags.h - https://github.com/mrsafalpiya/spl
 * no warranty implied; use at your own risk

 * See end of file for license information.
 */

/*
 *  ___       _                 _            _   _
 * |_ _|_ __ | |_ _ __ ___   __| |_   _  ___| |_(_) ___  _ __
 *  | || '_ \| __| '__/ _ \ / _` | | | |/ __| __| |/ _ \| '_ \
 *  | || | | | |_| | | (_) | (_| | |_| | (__| |_| | (_) | | | |
 * |___|_| |_|\__|_|  \___/ \__,_|\__,_|\___|\__|_|\___/|_| |_|
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
 *  _   _
 * | | | |___  __ _  __ _  ___
 * | | | / __|/ _` |/ _` |/ _ \
 * | |_| \__ \ (_| | (_| |  __/
 *  \___/|___/\__,_|\__, |\___|
 *                  |___/
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
 *  _   _                _                 __ _ _
 * | | | | ___  __ _  __| | ___ _ __      / _(_) | ___
 * | |_| |/ _ \/ _` |/ _` |/ _ \ '__|____| |_| | |/ _ \
 * |  _  |  __/ (_| | (_| |  __/ | |_____|  _| | |  __/
 * |_| |_|\___|\__,_|\__,_|\___|_|       |_| |_|_|\___|
 *
 */

#ifndef SPL_FLAGS_H
#define SPL_FLAGS_H

#include <stdio.h>

/*
 *******************************************************************************
 ================================ Global Variables =============================
 *******************************************************************************
 */

extern char *non_flag_arguments[512];
extern int non_flag_arguments_c;
/*
 * non_flag_arguments is an array of pointers in the array of argv which were
 * non-flags
 *
 * non_flag_arguments_c is the counter of pointers defined in non_flag_arguments
 */

extern char *non_defined_flags[512];
extern int non_defined_flags_c;
/*
 * non_defined_flags is an array of pointers in the array of argv which were
 * of type flag but not defined through the flag defining functions
 *
 * non_defined_flags_c is the counter of pointers defined in non_flag_arguments
 */

/*
 *******************************************************************************
 =================================== Functions =================================
 *******************************************************************************
 */

void
spl_flags_toggle(int *value, char short_hand, const char *long_hand, const char *info);
/*
 * spl_flags_toggle defines a flag of type toggle so that the value of the flag
 * gets set to the underlying variable pointed by value
 *
 * short_hand is a character defining the short way of inferring the flag in the
 * program argument
 *
 * long_hand is a string defining the long way of inferring the flag in the
 * program argument
 *
 * info is a piece of info that gets printed as the place holder in the help
 * section of the program
 */

void
spl_flags_int(int *value, char short_hand, const char *long_hand, const char *info);
/*
 * spl_flags_int defines a flag of type int so that the value of the flag gets
 * set to the underlying variable pointed by value
 *
 * short_hand is a character defining the short way of inferring the flag in the
 * program argument
 *
 * long_hand is a string defining the long way of inferring the flag in the
 * program argument
 *
 * info is a piece of info that gets printed as the place holder in the help
 * section of the program
 */

void
spl_flags_str(char **value, char short_hand, const char *long_hand, const char *info);
/*
 * spl_flags_str defines a flag of type string so that the value of the flag
 * gets set to the underlying variable pointed by value
 *
 * short_hand is a character defining the short way of inferring the flag in the
 * program argument
 *
 * long_hand is a string defining the long way of inferring the flag in the
 * program argument
 *
 * info is a piece of info that gets printed as the place holder in the help
 * section of the program
 *
 * NOTE: The value passed SHOULD have a initial value be it empty or a specific
 * value.
 *
 * Example:
 *
 * char *foo = "";
 * spl_flags_str(foo, 'f', "foo", "foobar");
 */

int
spl_flags_parse(int argc, char **argv);
/*
 * spl_flags_parse parses the argv for all the defined flags *previously*.
 * Meaning this function should be called at last AFTER defining all the flags
 * from the functions defined above.
 *
 * Returns 0 if everything went well. But in the case of any problem with the
 * argument, the index of the problematic argv is returned.
 */

void
spl_flags_print_flags(FILE *restrict stream);
/*
 * spl_flags_print_flags prints to the stream the type, defaults and info about
 * all the flags definedw.
 */

#ifdef SPL_FLAGS_DEBUG

extern void
spl_flags_debug_print(FILE *restrict stream);

#endif /* SPL_FLAGS_DEBUG */

#endif /* SPL_FLAGS_H */

/*
 *  ___                 _                           _        _   _
 * |_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __
 *  | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \
 *  | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
 * |___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
 *               |_|
 */

#ifdef SPL_FLAGS_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

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

char *non_flag_arguments[512];
int non_flag_arguments_c;
char *non_defined_flags[512];
int non_defined_flags_c;

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

int
spl_flags_parse(int argc, char **argv)
{
	char *cur_argv;
	flag *cur_flag;
	int is_found;

	non_flag_arguments_c = 0;
	non_defined_flags_c = 0;

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
				/* check if the current argv is the last */
				if ((cur_flag->type != TYPE_TOGGLE) &&
						(i == argc - 1))
					return i;

				switch (cur_flag->type) {
				case TYPE_INT:
					*((int *)cur_flag->value) =
						atoi(argv[++i]);
					break;
				case TYPE_STR:
					*((char **)cur_flag->value) =
						(void *)(argv[++i]);
					break;
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

	return 0;
}

void
spl_flags_print_flags(FILE *restrict stream)
{
	for (int i = 0; i < flags_c; i++) {
		fprintf(stream, "  -%c, --%s (Default: ", flags[i].short_hand, flags[i].long_hand);

		switch (flags[i].type) {
		case TYPE_TOGGLE:
			fprintf(stream, "%s",
					flags[i].value_default.value_int ? "on" : "off");
			break;
		case TYPE_INT:
			fprintf(stream, "%d",
					flags[i].value_default.value_int);
			break;
		case TYPE_STR:
			fprintf(stream, "'%s'",
					flags[i].value_default.value_str);
		}

		fprintf(stream, ")\t%s\n", flags[i].info);
	}
}

#ifdef SPL_FLAGS_DEBUG

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

#endif /* SPL_FLAGS_IMPLEMENTATION */

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
