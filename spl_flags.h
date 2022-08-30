/*
 ===============================================================================
 |                                 spl_flags.h                                 |
 |                      https://github.com/mrsafalpiya/spl                     |
 |                                                                             |
 |             GNU-style argument parsing inspired by golang's flag            |
 |                                                                             |
 |                  No warranty implied; Use at your own risk                  |
 |                   See end of file for license information.                  |
 ===============================================================================
 *
 * https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html
 */

/*
 ===============================================================================
 |                               Version History                               |
 ===============================================================================
 *
 - v0.2 (Current)
     - Added 'splf_warn_ignored_args()'.
 - v0.1
 */

/*
 ===============================================================================
 |                                    Usage                                    |
 ===============================================================================
 *
 * Just include this header file and you are done.
 *
 * See the example code below and check out the options.
 */

/*
 ===============================================================================
 |                                 Example code                                |
 ===============================================================================
 */
#if 0
#include <stdio.h>
#include <stdlib.h>

/* spl - https://github.com/mrsafalpiya/spl */
#include "dep/spl_flags.h"

int
main(int argc, char **argv)
{
	/* Initialize values on flags */
	int   to_print_help = 0;
	int   to_greet      = 0;
	int   age           = 20;
	float gpa           = 3.6;
	char *univ          = "Tribhuvan University";
	char *name          = NULL;

	/* Set flags */
	splf_toggle(&to_print_help, 'h', "help", "Print the help message");
	splf_toggle(&to_greet, 'g', "greet", "To greet");
	splf_int(&age, 'a', NULL, "Your age");
	splf_float(&gpa, ' ', "gpa", "Your gpa");
	splf_str(&univ, 'u', "university", "Your university");

	splf_info f_info = splf_parse(argc, argv);

	/* Printing any gotchas in parsing */
	if (splf_print_gotchas(f_info, stderr))
		exit(EXIT_FAILURE);

	/* Check if -h flag was passed */
	if (to_print_help) {
		fprintf(stdout, "Usage: %s name\n\nAvailable options are:\n",
		        argv[0]);
		splf_print_help(stdout);
		exit(EXIT_SUCCESS);
	}

	/* Ignore any argument after the name */
	splf_warn_ignored_args(f_info, stderr, 1);

	/* Check if the user gave us a name as an argument */
	if (f_info.non_flag_arguments_c == 0) {
		fprintf(stderr, "Usage: %s name\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	name = f_info.non_flag_arguments[0];

	if (to_greet)
		printf("Greetings!\n");

	printf("Your name is %s aged %d studying in %s and you got %.2f gpa.\n",
	       name, age, univ, gpa);

	return 0;
}

#endif

/* OUTPUT:
$ ./bin/flags
Usage: ./bin/flags name

$ ./bin/flags -h
Usage: ./bin/flags name

Available options are:
    -h, --help, (Default: Off)  Print the help message
    -g, --greet, (Default: Off) To greet
    -a, (Default: 20)   Your age
    --gpa, (Default: 3.60)      Your gpa
    -u, --university, (Default: 'Tribhuvan University') Your university

$ ./bin/flags --help
Usage: ./bin/flags name

Available options are:
    -h, --help, (Default: Off)  Print the help message
    -g, --greet, (Default: Off) To greet
    -a, (Default: 20)   Your age
    --gpa, (Default: 3.60)      Your gpa
    -u, --university, (Default: 'Tribhuvan University') Your university

$ ./bin/flags Safal
Your name is Safal aged 20 studying in Tribhuvan University and you got 3.60 gpa.

$ ./bin/flags Safal Piya
WARNING: Following arguments are ignored: "Piya".
Your name is Safal aged 20 studying in Tribhuvan University and you got 3.60 gpa.

$ ./bin/flags Safal -a=21 --gpa 3.2 -u "Prime College"
Your name is Safal aged 21 studying in Prime College and you got 3.20 gpa.

$ ./bin/flags Safal -ga=21
Greetings!
Your name is Safal aged 21 studying in Tribhuvan University and you got 3.60 gpa.

$ ./bin/flags Safal -ga=21 -u
No value given on the short flag 'u'
*/

/*
 ===============================================================================
 |                                   Options                                   |
 ===============================================================================
 */

/* = splf = */
#ifndef SPLF_DEF
#define SPLF_DEF /* You may want `static` or `static inline` here */
#endif

#define SPLF_MAX 256 /* Max number of flags to support */

#ifndef SPLF_FLOAT_PRECISION
#define SPLF_FLOAT_PRECISION "2" /* Float precision on 'print_help()' */
#endif

/* values on toggle */
#ifndef SPLF_TOGGLE_0_STR
#define SPLF_TOGGLE_0_STR \
	"Off" /* String representation for toggle 0 on 'print_help()' */
#endif

#ifndef SPLF_TOGGLE_1_STR
#define SPLF_TOGGLE_1_STR \
	"On" /* String representation for toggle 1 on 'print_help()' */
#endif

/*
 ===============================================================================
 |                       HEADER-FILE/IMPLEMENTATION MODE                       |
 ===============================================================================
 */

#ifndef SPLF_H
#define SPLF_H

#include <stdlib.h>
#include <string.h>

/*
 ===============================================================================
 |                                     Data                                    |
 ===============================================================================
 */

/* = Enums = */
typedef enum {
	SPL_FLAG_TYPE_TOGGLE,
	SPL_FLAG_TYPE_INT,
	SPL_FLAG_TYPE_FLOAT,
	SPL_FLAG_TYPE_STR
} spl_flag_type;

typedef enum {
	NONE,
	SHORT_NON_EQUAL,
	SHORT_EQUAL,
	LONG_NON_EQUAL,
	LONG_EQUAL
} splf_arg_type;

/* = Unions = */
typedef union {
	int   tog_num;
	float float_num;
	char  str[1024];
} splf_def_value;

/* = Structs = */
typedef struct {
	spl_flag_type  type;
	char           short_hand;
	char           long_hand[256];
	splf_def_value def_value;
	char           info[512];
	void          *data_ptr;
} spl_flag_entry;

typedef struct {
	char *non_defined_flags_long[256];
	char  non_defined_flags_short[256];
	char *non_flag_arguments[256];
	int   non_defined_flags_long_c;
	int   non_defined_flags_short_c;
	int   non_flag_arguments_c;
	char *non_value_flag_long;
	char  non_value_flag_short; /* '\0' = empty */
} splf_info;

/* = Global variables = */
static spl_flag_entry splf[SPLF_MAX];
static int            splf_c = 0;

/*
 ===============================================================================
 |                            Function Declarations                            |
 ===============================================================================
 */

/* Creates a toggle-type flag. */
SPLF_DEF void
splf_toggle(int *f_toggle, const char short_hand, const char *long_hand,
            const char *info);

/* Creates a int-type flag. */
SPLF_DEF void
splf_int(int *f_int, const char short_hand, const char *long_hand,
         const char *info);

/* Creates a float-type flag. */
SPLF_DEF void
splf_float(float *f_float, const char short_hand, const char *long_hand,
           const char *info);

/* Creates a string-type flag. */
SPLF_DEF void
splf_str(char **f_str, const char short_hand, const char *long_hand,
         const char *info);

/*
 * Parses all the flags with the given argc and argv.
 *
 * Make sure to pass the argc and argv from the 'main' without any
 * modifications.
 */
SPLF_DEF splf_info
splf_parse(int argc, char **argv);

/*
 * Sometimes the user provides more arguments (namely 'non_flag_arguments') than
 * anticipated and are ignored.  Warn about it.
 *
 * `index` is the index of the 'non_flag_arguments' from which the arguments are
 * ignored.
 */
SPLF_DEF void
splf_warn_ignored_args(splf_info f_info, FILE *stream, int index);

/* Outputs the help message to the given `stream`. */
SPLF_DEF void
splf_print_help(FILE *stream);

/*
 * Outputs any gotchas in the parsing to the given `stream` by reading the
 * 'non_defined_flags_long_c', 'non_defined_flags_short_c',
 * 'non_flag_arguments_c', 'non_value_flag_long', 'non_value_flag_short' fields
 * of the 'splf_info' struct.
 *
 * Returns 1 if any gotchas was found.
 */
SPLF_DEF int
splf_print_gotchas(splf_info f_info, FILE *stream);

/*
 ===============================================================================
 |                           Function Implementations                          |
 ===============================================================================
 */

#define SPLF_ADD_MACRO                                     \
	splf[splf_c].short_hand = short_hand;              \
	if (long_hand != NULL)                             \
		strcpy(splf[splf_c].long_hand, long_hand); \
	else                                               \
		splf[splf_c].long_hand[0] = '\0';          \
	strcpy(splf[splf_c].info, info);                   \
	++splf_c;

SPLF_DEF void
splf_toggle(int *f_toggle, const char short_hand, const char *long_hand,
            const char *info)
{
	splf[splf_c].type              = SPL_FLAG_TYPE_TOGGLE;
	splf[splf_c].data_ptr          = (void *)f_toggle;
	splf[splf_c].def_value.tog_num = *f_toggle;
	SPLF_ADD_MACRO
}

SPLF_DEF void
splf_int(int *f_int, const char short_hand, const char *long_hand,
         const char *info)
{
	splf[splf_c].type              = SPL_FLAG_TYPE_INT;
	splf[splf_c].data_ptr          = (void *)f_int;
	splf[splf_c].def_value.tog_num = *f_int;
	SPLF_ADD_MACRO
}

SPLF_DEF void
splf_float(float *f_float, const char short_hand, const char *long_hand,
           const char *info)
{
	splf[splf_c].type                = SPL_FLAG_TYPE_FLOAT;
	splf[splf_c].data_ptr            = (void *)f_float;
	splf[splf_c].def_value.float_num = *f_float;
	SPLF_ADD_MACRO
}

SPLF_DEF void
splf_str(char **f_str, const char short_hand, const char *long_hand,
         const char *info)
{
	splf[splf_c].type     = SPL_FLAG_TYPE_STR;
	splf[splf_c].data_ptr = (void **)f_str;
	if (*f_str != NULL)
		strcpy(splf[splf_c].def_value.str, *f_str);
	SPLF_ADD_MACRO
}

SPLF_DEF splf_info
splf_parse(int argc, char **argv)
{
	int is_double_dash = 0;
	char *cur_arg;

	splf_info f_info = { 0 };

	for (int i = 1; i < argc; i++) {
		int is_long_arg = 0;
		char *equal_ch;

		/* Check if double dash was present or the current argv is NOT a
		 * flag type argument */
		if (is_double_dash || argv[i][0] != '-') {
			f_info.non_flag_arguments[f_info.non_flag_arguments_c++] =
				argv[i];
			continue;
		}

		/* Check for double dash */
		if (strcmp(argv[i], "--") == 0) {
			is_double_dash = 1;
			continue;
		}

		/* = Now we have a valid flag argument = */
		cur_arg = argv[i] + 1;

		/* Check if long or short type argument */
		if (cur_arg[0] == '-') {
			is_long_arg = 1;
			++cur_arg;
		}

	flag_proc: /* I really have to use goto */
		/* Check for equal sign */
		equal_ch = (cur_arg[1] == '=') ? cur_arg + 1 : NULL;
		if (is_long_arg)
			equal_ch = strchr(cur_arg, '=');

		splf_arg_type a_type = NONE;
		/* Iterate through defined flags and search for valid flag */
		for (int j = 0; j < splf_c; j++) {
			/* Get the argument type */
			if (is_long_arg) {
				if (equal_ch &&
				    ((strncmp(cur_arg, splf[j].long_hand,
				              equal_ch - cur_arg)) == 0))
					a_type = LONG_EQUAL;
				else if ((strcmp(cur_arg, splf[j].long_hand)) ==
				         0)
					a_type = LONG_NON_EQUAL;
			} else {
				if (cur_arg[0] == splf[j].short_hand) {
					if (equal_ch)
						a_type = SHORT_EQUAL;
					else
						a_type = SHORT_NON_EQUAL;
				}
			}

			if (a_type == NONE)
				continue;

			/* Perform proper action according to argument type */
			switch (a_type) {
			case LONG_EQUAL:
			case SHORT_EQUAL:
				switch (splf[j].type) {
				case SPL_FLAG_TYPE_TOGGLE:
					*((int *)splf[j].data_ptr) =
						atoi(equal_ch + 1);
					break;
				case SPL_FLAG_TYPE_INT:
					*((int *)splf[j].data_ptr) =
						atoi(equal_ch + 1);
					break;
				case SPL_FLAG_TYPE_FLOAT:
					*((float *)splf[j].data_ptr) =
						atof(equal_ch + 1);
					break;
				case SPL_FLAG_TYPE_STR:
					*((char **)splf[j].data_ptr) =
						equal_ch + 1;
					break;
				}
				break;
			case LONG_NON_EQUAL:
			case SHORT_NON_EQUAL:
				switch (splf[j].type) {
				case SPL_FLAG_TYPE_TOGGLE:
					*((int *)splf[j].data_ptr) =
						!*((int *)splf[j].data_ptr);
					/* Check if other arguments can be
					 * parsed after cur_arg but in the
					 * current argv */
					if (a_type == SHORT_NON_EQUAL &&
					    (*(cur_arg + 1) != '\0')) {
						a_type = NONE;
						j = -1; /* reiterate flags */
						++cur_arg;
						equal_ch = (cur_arg[1] == '=') ?
						                   cur_arg + 1 :
						                   NULL;
						continue;
					}
					break;
				case SPL_FLAG_TYPE_INT:
					if (i == argc - 1) {
						if (a_type == LONG_NON_EQUAL)
							f_info.non_value_flag_long =
								cur_arg;
						else if (cur_arg[1] == '\0') {
							f_info.non_value_flag_short =
								*cur_arg;
							return f_info;
						}
					}

					if (a_type == SHORT_NON_EQUAL &&
					    *(cur_arg + 1) != '\0') {
						*((int *)splf[j].data_ptr) =
							atoi(cur_arg + 1);
						break;
					}

					*((int *)splf[j].data_ptr) =
						atoi(argv[++i]);
					break;
				case SPL_FLAG_TYPE_FLOAT:
					if (i == argc - 1) {
						if (a_type == LONG_NON_EQUAL)
							f_info.non_value_flag_long =
								cur_arg;
						else if (cur_arg[1] == '\0') {
							f_info.non_value_flag_short =
								*cur_arg;
							return f_info;
						}
					}

					if (a_type == SHORT_NON_EQUAL &&
					    *(cur_arg + 1) != '\0') {
						*((float *)splf[j].data_ptr) =
							atof(cur_arg + 1);
						break;
					}

					*((float *)splf[j].data_ptr) =
						atof(argv[++i]);
					break;
				case SPL_FLAG_TYPE_STR:
					if (i == argc - 1) {
						if (a_type == LONG_NON_EQUAL)
							f_info.non_value_flag_long =
								cur_arg;
						else if (cur_arg[1] == '\0') {
							f_info.non_value_flag_short =
								*cur_arg;
							return f_info;
						}
					}

					if (a_type == SHORT_NON_EQUAL &&
					    *(cur_arg + 1) != '\0') {
						*((char **)splf[j].data_ptr) =
							cur_arg + 1;
						break;
					}

					*((char **)splf[j].data_ptr) =
						argv[++i];
					break;
				}
				break;
			case NONE:
				break;
			}

			if (a_type != NONE)
				break;
		}

		/* Check if the argument was an undefined flag */
		if (a_type == NONE) {
			if (is_long_arg)
				f_info.non_defined_flags_long
					[f_info.non_defined_flags_long_c++] =
					cur_arg;
			else {
				f_info.non_defined_flags_short
					[f_info.non_defined_flags_short_c++] =
					*cur_arg;
				/* Check if other arguments can be parsed after
				 * cur_arg but in the current argv */
				if (*(cur_arg + 1) != '\0') {
					++cur_arg;
					goto flag_proc;
					continue;
				}
			}
		}
	}

	return f_info;
}

SPLF_DEF void
splf_warn_ignored_args(splf_info f_info, FILE *stream, int index)
{
	if (index >= f_info.non_flag_arguments_c) {
		return;
	}
	fprintf(stream, "WARNING: Following arguments are ignored: ");

	for (int i = index; i < f_info.non_flag_arguments_c; i++)
		fprintf(stream, "\"%s\"%c", f_info.non_flag_arguments[i],
		        i == f_info.non_flag_arguments_c - 1 ? '.' : ' ');
	fprintf(stream, "\n");
}

SPLF_DEF void
splf_print_help(FILE *stream)
{
	for (int i = 0; i < splf_c; i++) {
		fprintf(stream, "    ");

		/* flag value */
		if (splf[i].short_hand != ' ')
			fprintf(stream, "-%c, ", splf[i].short_hand);
		if (splf[i].long_hand[0] != '\0')
			fprintf(stream, "--%s, ", splf[i].long_hand);

		/* default value */
		switch (splf[i].type) {
		case SPL_FLAG_TYPE_TOGGLE:
			fprintf(stream, "(Default: %s)",
			        splf[i].def_value.tog_num == 0 ?
			                SPLF_TOGGLE_0_STR :
			                SPLF_TOGGLE_1_STR);
			break;
		case SPL_FLAG_TYPE_INT:
			fprintf(stream, "(Default: %d)",
			        splf[i].def_value.tog_num);
			break;
		case SPL_FLAG_TYPE_FLOAT:
			fprintf(stream,
			        "(Default: %0." SPLF_FLOAT_PRECISION "f)",
			        splf[i].def_value.float_num);
			break;
		case SPL_FLAG_TYPE_STR:
			if (splf[i].def_value.str[0] != '\0')
				fprintf(stream, "(Default: '%s')",
				        splf[i].def_value.str);
		}

		/* info */
		if (splf[i].info[0] != '\0')
			fprintf(stream, "\t%s", splf[i].info);

		fprintf(stream, "\n");
	}
}

SPLF_DEF int
splf_print_gotchas(splf_info f_info, FILE *stream)
{
	int ret_value = 0;

	/* Checking for an empty valued flag */
	if (f_info.non_value_flag_long != NULL) {
		fprintf(stream, "No value given on the long flag '%s'\n",
		        f_info.non_value_flag_long);
		ret_value = 1;
	}
	if (f_info.non_value_flag_short != '\0') {
		fprintf(stream, "No value given on the short flag '%c'\n",
		        f_info.non_value_flag_short);
		ret_value = 1;
	}

	/* Check if any not defined or non-flag arguments are passed */
	if (f_info.non_defined_flags_long_c > 0) {
		fprintf(stream, "Following long flags are undefined: ");
		for (int i = 0; i < f_info.non_defined_flags_long_c; i++)
			fprintf(stream, "%s ",
			        f_info.non_defined_flags_long[i]);
		fprintf(stream, "\n");
		ret_value = 1;
	}
	if (f_info.non_defined_flags_short_c > 0) {
		fprintf(stream, "Following short flags are undefined: ");
		for (int i = 0; i < f_info.non_defined_flags_short_c; i++)
			fprintf(stream, "%c ",
			        f_info.non_defined_flags_short[i]);
		fprintf(stream, "\n");
		ret_value = 1;
	}

	return ret_value;
}

#endif /* SPLF_H */

/*
 ===============================================================================
 |                 License - Public Domain (www.unlicense.org)                 |
 ===============================================================================
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
