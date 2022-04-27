/*
 * spl_flags.h - https://github.com/mrsafalpiya/spl
 * no warranty implied; use at your own risk
 *
 * See end of file for license information.
 */

/*
 * ================
 * | Introduction |
 * ================
 *
 * SPL_FLAGS.H - GNU-style argument parsing inspired by golang's `flag` package.
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
 */

/*
 * ===========
 * | Example |
 * ===========
 *
 * A simple example of program using this library is given below:
 */
#if 0
#include <stdio.h>
#include <stdlib.h>

/* spl - https://github.com/mrsafalpiya */
#define SPL_FLAGS_IMPLEMENTATION
#include "spl_flags.h"

void
print_usage(FILE *stream, char *prog_name)
{
	fprintf(stream, "Usage: %s name\n\n", prog_name);
	fprintf(stream, "Available flags are:\n");
	spl_flags_print_help(stream);
}

int
main(int argc, char **argv)
{
	uint8_t to_print_help;
	int32_t age;
	char   *name, *univ;
	float   gpa;

	spl_flags_argv_parse ag;

	/* Default flag values */
	to_print_help = 0;
	age           = 19;
	univ          = NULL;
	gpa           = 3.65;

	/* Flags */
	spl_flags_toggle(&to_print_help, 'h', "help", "Print help message");
	spl_flags_int(&age, 'a', NULL, "Your age");
	spl_flags_str(&univ, ' ', "university",
	              "University you are studying in");
	spl_flags_float(&gpa, 'g', "gpa", "Your GPA");

	ag = spl_flags_parse(argc, argv);

	/* = Main operations = */
	/* argument checking */
	if (to_print_help) {
		print_usage(stdout, argv[0]);
		exit(EXIT_SUCCESS);
	}

	if (ag.non_flags_c < 1) {
		print_usage(stderr, argv[0]);
		exit(EXIT_FAILURE);
	}

	name = ag.non_flags[0];
	printf("Your name is %s, aged %d studying in %s and you got %.2f GPA\n",
	       name, age, univ, gpa);

	return EXIT_SUCCESS;
}
#endif
/*
 * Here four flags 'to_print_help', 'age', 'university' and 'gpa' are defined.
 *
 * Note that no long-hand argument is defined for `age` and no short-hand
 * argument is defined for `university`.
 *
 * Usage of `spl_flags_argv_parse` is also demonstrated.
 *
 * Output of this program is as follow:
 *
 * $ ./spl-test
 * Usage: ./spl-test name
 *
 * Available flags are:
 *     -h, --help, (Default: Off)  Print help message
 *     -a, (Default: 19)   Your age
 *     --university,       University you are studying in
 *     -g, --gpa, (Default: 3.650000)      Your GPA
 *
 * $ ./spl-test --help
 * Usage: ./spl-test name
 *
 * Available flags are:
 *     -h, --help, (Default: Off)  Print help message
 *     -a, (Default: 19)   Your age
 *     --university,       University you are studying in
 *     -g, --gpa, (Default: 3.650000)      Your GPA
 *
 * $ ./spl-test Safal -a20 --gpa 3.55 --university="Tribhuvan University"
 * Your name is Safal, aged 20 studying in Tribhuvan University and you got 3.55
 * GPA
 */

/*
 * ====================
 * | Header-file mode |
 * ====================
 */

#ifndef SPL_FLAGS_H
#define SPL_FLAGS_H

#include <stdio.h>
#include <stdint.h>

/* constants */
#ifndef SPL_FLAGS_CAP
#define SPL_FLAGS_CAP 256
#endif

/* struct */

/*
 * Struct returned by the `spl_flags_parse()` function.
 *
 * Contains information about the parse including the non-defined flags and
 * non-flags arguments.
 */
typedef struct {
	char  *non_defined_flags_long[512];
	size_t non_defined_flags_long_c;
	char   non_defined_flags_short[512];
	size_t non_defined_flags_short_c;
	char  *non_flags[512];
	size_t non_flags_c;
} spl_flags_argv_parse;

/* function declarations */
void
spl_flags_int(int32_t *value, const char short_hand, const char *long_hand,
              const char *info);
/*
 * Define a new int-type flag.
 */

void
spl_flags_toggle(uint8_t *value, const char short_hand, const char *long_hand,
                 const char *info);
/*
 * Define a new toggle-type flag.
 */

void
spl_flags_float(float *value, const char short_hand, const char *long_hand,
                const char *info);
/*
 * Define a new float-type flag.
 */

void
spl_flags_str(char **value, const char short_hand, const char *long_hand,
              const char *info);
/*
 * Define a new string-type flag.
 */

spl_flags_argv_parse
spl_flags_parse(int argc, char **argv);
/*
 * Parse the `argv` with all the defined flags above this code.
 *
 * Just like the original `argv` passed to the `main` function, the first
 * argument to `argv` will be ignored.
 *
 * Make sure this line is written AFTER defining all the flags.
 *
 * Returns a struct containing info about the non-defined and non-flag arguments
 * in the `argv`.
 */

void
spl_flags_print_help(FILE *stream);
/*
 * Print a well formatted help message on `stream` for all the defined flags.
 */

#endif /* SPL_FLAGS_H */

/*
 * ===================
 * | Implementations |
 * ===================
 */

#ifdef SPL_FLAGS_IMPLEMENTATION

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

/* enum */
typedef enum { FLAG_INT, FLAG_TOGGLE, FLAG_FLOAT, FLAG_STR } flag_type;
typedef enum { VALUE_CURRENT, VALUE_DEFAULT } value_type;
typedef enum {
	NON_DEFINED = 0,
	FLAG_INT_SHORT_NON_EQUAL,
	FLAG_INT_SHORT_EQUAL,
	FLAG_INT_LONG_NON_EQUAL,
	FLAG_INT_LONG_EQUAL,
	FLAG_TOGGLE_SHORT_NON_EQUAL,
	FLAG_TOGGLE_SHORT_EQUAL,
	FLAG_TOGGLE_LONG_NON_EQUAL,
	FLAG_TOGGLE_LONG_EQUAL,
	FLAG_FLOAT_SHORT_NON_EQUAL,
	FLAG_FLOAT_SHORT_EQUAL,
	FLAG_FLOAT_LONG_NON_EQUAL,
	FLAG_FLOAT_LONG_EQUAL,
	FLAG_STR_SHORT_NON_EQUAL,
	FLAG_STR_SHORT_EQUAL,
	FLAG_STR_LONG_NON_EQUAL,
	FLAG_STR_LONG_EQUAL,
} argv_flag_type;

/* macros for `argv_flag_type` */
#define IS_EQUAL_ARG(X) (X % 2 == 0)

/* union */
typedef union {
	int32_t value_int;
	uint8_t value_toggle;
	float   value_float;
	char    value_str[1024];
} flag_value;

/* struct */
typedef struct {
	flag_type   type;
	char        short_hand;
	const char *long_hand;
	uintptr_t   value;
	flag_value  value_default;
	const char *info;
} flag;

typedef struct {
	argv_flag_type flag_type;
	flag          *flag_p;
} argv_flag_info;

/* global variables */
static flag   defined_flags[SPL_FLAGS_CAP];
static size_t defined_flags_c = 0;

/* function implementations */
/*
 * Create a new flag with the properties of the passed arguments.
 *
 * The properties defined are `short_hand`, `long_hand`, `value` and `info.
 * Remaining properties: `type` and `value_default` are to be filled by the
 * caller function.
 *
 * Flags counter is also incremented.
 *
 * Returns pointer to the new flag defined.
 */
flag *
create_new_flag(uintptr_t value, const char short_hand, const char *long_hand,
                const char *info)
{
	assert(defined_flags_c < SPL_FLAGS_CAP);

	defined_flags[defined_flags_c].short_hand = short_hand;
	defined_flags[defined_flags_c].long_hand  = long_hand;
	defined_flags[defined_flags_c].value      = value;
	defined_flags[defined_flags_c].info       = info;

	defined_flags_c++;
	return &defined_flags[defined_flags_c - 1];
}

void
spl_flags_int(int32_t *value, const char short_hand, const char *long_hand,
              const char *info)
{
	flag *new_flag;

	new_flag = create_new_flag((uintptr_t)((void *)value), short_hand,
	                           long_hand, info);

	/* fill remaining properties */
	new_flag->type                    = FLAG_INT;
	new_flag->value_default.value_int = *value;
}

void
spl_flags_toggle(uint8_t *value, const char short_hand, const char *long_hand,
                 const char *info)
{
	flag *new_flag;

	new_flag = create_new_flag((uintptr_t)((void *)value), short_hand,
	                           long_hand, info);

	/* fill remaining properties */
	new_flag->type                       = FLAG_TOGGLE;
	new_flag->value_default.value_toggle = *value;
}

void
spl_flags_float(float *value, const char short_hand, const char *long_hand,
                const char *info)
{
	flag *new_flag;

	new_flag = create_new_flag((uintptr_t)((void *)value), short_hand,
	                           long_hand, info);

	/* fill remaining properties */
	new_flag->type                      = FLAG_FLOAT;
	new_flag->value_default.value_float = *value;
}

void
spl_flags_str(char **value, const char short_hand, const char *long_hand,
              const char *info)
{
	flag *new_flag;

	new_flag = create_new_flag((uintptr_t)((void **)value), short_hand,
	                           long_hand, info);

	/* fill remaining properties */
	new_flag->type = FLAG_STR;
	if (*value != NULL)
		strcpy(new_flag->value_default.value_str, *value);
	else
		new_flag->value_default.value_str[0] = '\0';
}

/*
 * Get the type of flag and the flag itself for the given argv.
 *
 * NOTE: Make sure to pass the 1st index of the argv.
 *
 * Matches the passed argument with all the flags defined.
 *
 * Returns a `argv_flag_info` struct.
 */
argv_flag_info
get_flag_info(char *argv_1)
{
	argv_flag_info flag_info;

	/* initialize flag_info. */
	flag_info.flag_type = NON_DEFINED;
	flag_info.flag_p    = NULL;

	for (size_t i = 0; i < defined_flags_c; i++) {
		if (argv_1[0] == defined_flags[i].short_hand) {
			flag_info.flag_p = &defined_flags[i];

			/* short-hand */
			switch (defined_flags[i].type) {
			case FLAG_INT:
				flag_info.flag_type =
					(argv_1[1] == '=') ?
						FLAG_INT_SHORT_EQUAL :
                                                FLAG_INT_SHORT_NON_EQUAL;
				break;
			case FLAG_TOGGLE:
				flag_info.flag_type =
					(argv_1[1] == '=') ?
						FLAG_TOGGLE_SHORT_EQUAL :
                                                FLAG_TOGGLE_SHORT_NON_EQUAL;
				break;
			case FLAG_FLOAT:
				flag_info.flag_type =
					(argv_1[1] == '=') ?
						FLAG_FLOAT_SHORT_EQUAL :
                                                FLAG_FLOAT_SHORT_NON_EQUAL;
				break;
			case FLAG_STR:
				flag_info.flag_type =
					(argv_1[1] == '=') ?
						FLAG_STR_SHORT_EQUAL :
                                                FLAG_STR_SHORT_NON_EQUAL;
			}

			break;
		}

		if (defined_flags[i].long_hand == NULL)
			continue;
		if (strncmp(argv_1 + 1, defined_flags[i].long_hand,
		            strlen(defined_flags[i].long_hand)) == 0) {
			flag_info.flag_p = &defined_flags[i];

			size_t equal_i = strlen(defined_flags[i].long_hand);
			equal_i++; /* make equal_i point to the potential `=` */

			/* long-hand */
			switch (defined_flags[i].type) {
			case FLAG_INT:
				flag_info.flag_type =
					(argv_1[equal_i] == '=') ?
						FLAG_INT_LONG_EQUAL :
                                                FLAG_INT_LONG_NON_EQUAL;
				break;
			case FLAG_TOGGLE:
				flag_info.flag_type =
					(argv_1[equal_i] == '=') ?
						FLAG_TOGGLE_LONG_EQUAL :
                                                FLAG_TOGGLE_LONG_NON_EQUAL;
				break;
			case FLAG_FLOAT:
				flag_info.flag_type =
					(argv_1[equal_i] == '=') ?
						FLAG_FLOAT_LONG_EQUAL :
                                                FLAG_FLOAT_LONG_NON_EQUAL;
				break;
			case FLAG_STR:
				flag_info.flag_type =
					(argv_1[equal_i] == '=') ?
						FLAG_STR_LONG_EQUAL :
                                                FLAG_STR_LONG_NON_EQUAL;
			}

			break;
		}
	}

	return flag_info;
}

spl_flags_argv_parse
spl_flags_parse(int argc, char **argv)
{
	spl_flags_argv_parse argv_parse;
	uint8_t              flags_terminated;
	char                *cur_argv;

	/* initialize counters */
	argv_parse.non_defined_flags_long_c  = 0;
	argv_parse.non_defined_flags_short_c = 0;
	argv_parse.non_flags_c               = 0;

	flags_terminated = 0;

	for (int i = 1; i < argc; i++) {
		cur_argv = argv[i];

		/*
		 * Check if flags are terminated using the `--` or non-flag type
		 * argument is present.
		 */
		if (flags_terminated || cur_argv[0] != '-') {
			argv_parse.non_flags[argv_parse.non_flags_c++] =
				cur_argv;
			continue;
		}

		/* Check for flag terminator: `--`. */
		if (strcmp(cur_argv, "--") == 0) {
			flags_terminated = 1;
			continue;
		}

	spl_flags_parse_info_parsing:;
		/* Now we have a valid flag-type argument. */
		argv_flag_info fi;
		char          *equal_symbol;
		size_t         lh_len;

		fi = get_flag_info(cur_argv + 1);

		/* Check if we have to use the `equal_symbol`? */
		if (IS_EQUAL_ARG(fi.flag_type))
			equal_symbol = strchr(cur_argv, '=');
		/* Check if we have to find the length of `long_hand` */
		else
			if (fi.flag_p->long_hand != NULL)
				lh_len = strlen(fi.flag_p->long_hand);

		switch (fi.flag_type) {
		case FLAG_INT_SHORT_NON_EQUAL:
			/* Check for non-spaced flag value. */
			if (isalnum(*(cur_argv + 2))) {
				*((int32_t *)fi.flag_p->value) =
					atoi(cur_argv + 2);
				break;
			}

			/* Check if current argv is the last. */
			if (i != argc - 1)
				*((int32_t *)fi.flag_p->value) =
					atoi(argv[++i]);
			break;
		case FLAG_INT_LONG_NON_EQUAL:
			/* Check for non-spaced flag value. */
			cur_argv = cur_argv + lh_len + 2;
			if (isalnum(*(cur_argv))) {
				*((int32_t *)fi.flag_p->value) = atoi(cur_argv);
				break;
			}

			/* Check if current argv is the last. */
			if (i != argc - 1)
				*((int32_t *)fi.flag_p->value) =
					atoi(argv[++i]);
			break;
		case FLAG_TOGGLE_SHORT_NON_EQUAL:
			*((uint8_t *)fi.flag_p->value) =
				!*((uint8_t *)fi.flag_p->value);

			/* Check for further args in current argv. */
			if (isalpha(*(cur_argv + 2))) {
				cur_argv++;
				goto spl_flags_parse_info_parsing;
			}

			break;
		case FLAG_TOGGLE_LONG_NON_EQUAL:
			*((uint8_t *)fi.flag_p->value) =
				!*((uint8_t *)fi.flag_p->value);
			break;
		case FLAG_FLOAT_SHORT_NON_EQUAL:
			/* Check for non-spaced flag value. */
			if (isalnum(*(cur_argv + 2))) {
				*((float *)fi.flag_p->value) =
					atof(cur_argv + 2);
				break;
			}

			/* Check if current argv is the last. */
			if (i != argc - 1)
				*((float *)fi.flag_p->value) = atof(argv[++i]);
			break;
		case FLAG_FLOAT_LONG_NON_EQUAL:
			cur_argv = cur_argv + lh_len + 2;
			if (isalnum(*(cur_argv))) {
				*((float *)fi.flag_p->value) = atof(cur_argv);
				break;
			}

			/* Check if current argv is the last. */
			if (i != argc - 1)
				*((float *)fi.flag_p->value) = atof(argv[++i]);
			break;
		case FLAG_STR_SHORT_NON_EQUAL:
			/* Check for non-spaced flag value. */
			if (isalnum(*(cur_argv + 2))) {
				*((char **)fi.flag_p->value) = (cur_argv + 2);
				break;
			}

			/* Check if current argv is the last. */
			if (i != argc - 1)
				*((char **)fi.flag_p->value) = argv[++i];
			break;
		case FLAG_STR_LONG_NON_EQUAL:
			/* Check for non-spaced flag value. */
			cur_argv = cur_argv + lh_len + 2;
			if (isalnum(*(cur_argv))) {
				*((char **)fi.flag_p->value) = cur_argv;
				break;
			}

			/* Check if current argv is the last. */
			if (i != argc - 1)
				*((char **)fi.flag_p->value) = argv[++i];
			break;
		case FLAG_INT_SHORT_EQUAL:
		case FLAG_INT_LONG_EQUAL:
			*((int32_t *)fi.flag_p->value) = atoi(equal_symbol + 1);
			break;
		case FLAG_TOGGLE_SHORT_EQUAL:
		case FLAG_TOGGLE_LONG_EQUAL:
			*((uint8_t *)fi.flag_p->value) = atoi(equal_symbol + 1);
			break;
		case FLAG_FLOAT_SHORT_EQUAL:
		case FLAG_FLOAT_LONG_EQUAL:
			*((float *)fi.flag_p->value) = atof(equal_symbol + 1);
			break;
		case FLAG_STR_SHORT_EQUAL:
		case FLAG_STR_LONG_EQUAL:
			*((char **)fi.flag_p->value) = equal_symbol + 1;
			break;
		case NON_DEFINED:
			if (cur_argv[1] == '-') {
				argv_parse.non_defined_flags_long
					[argv_parse.non_defined_flags_long_c++] =
					cur_argv + 2;
				continue;
			}

			argv_parse.non_defined_flags_short
				[argv_parse.non_defined_flags_short_c++] =
				*(++cur_argv);

			/* Check for further args in current argv. */
			if (isalpha(*(cur_argv + 1)))
				goto spl_flags_parse_info_parsing;
		}
	}

	return argv_parse;
}

void
spl_flags_print_help(FILE *stream)
{
	for (size_t i = 0; i < defined_flags_c; i++) {
		fprintf(stream, "    ");

		/* flag value */
		if (defined_flags[i].short_hand != ' ')
			fprintf(stream, "-%c, ", defined_flags[i].short_hand);
		if (defined_flags[i].long_hand != NULL)
			fprintf(stream, "--%s, ", defined_flags[i].long_hand);

		/* default value */
		switch (defined_flags[i].type) {
		case FLAG_INT:
			fprintf(stream, "(Default: %d)",
			        defined_flags[i].value_default.value_int);
			break;
		case FLAG_TOGGLE:
			fprintf(stream, "(Default: %s)",
			        defined_flags[i].value_default.value_toggle ==
			                        0 ?
			                "Off" :
                                        "On");
			break;
		case FLAG_FLOAT:
			fprintf(stream, "(Default: %f)",
			        defined_flags[i].value_default.value_float);
			break;
		case FLAG_STR:
			if (defined_flags[i].value_default.value_str[0] != '\0')
				fprintf(stream, "(Default: '%s')",
				        defined_flags[i]
				                .value_default.value_str);
		}

		/* info */
		if (defined_flags[i].info != NULL)
			fprintf(stream, "\t%s", defined_flags[i].info);

		fprintf(stream, "\n");
	}
}

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
