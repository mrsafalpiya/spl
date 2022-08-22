/*
 ===============================================================================
 |                                 spl_utils.h                                 |
 |                     https://github.com/mrsafalpiya/spl                      |
 |                                                                             |
 |                              Utility functions                              |
 |                                                                             |
 |                  No warranty implied; Use at your own risk                  |
 |                  See end of file for license information.                   |
 ===============================================================================
 */

/*
 ===============================================================================
 |                               Version History                               |
 ===============================================================================
 *
 - v0.1 (Current)
 */

/*
 ===============================================================================
 |                                    Usage                                    |
 ===============================================================================
 *
 * Do this:
 *
 *         #define SPLU_IMPL
 *
 * before you include this file in *one* C or C++ file to create the
 * implementation.
 */

/*
 ===============================================================================
 |                                   Options                                   |
 ===============================================================================
 */

/* = SPLU = */
#ifndef SPLU_DEF
#define SPLU_DEF /* You may want `static` or `static inline` here */
#endif

/*
 ===============================================================================
 |                              HEADER-FILE MODE                               |
 ===============================================================================
 */

#ifndef SPLU_H
#define SPLU_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

/*
 ===============================================================================
 |                            Function Declarations                            |
 ===============================================================================
 */

/*
 * Writes out the passed format to stderr and kill the program.
 * Copied from git.suckless.org/dwm.
 *
 * Automatically adds a new line to the end.
 *
 * If a semicolon `:` is present at the end of the string, perror() is also
 * called appending the error.
 */
SPLU_DEF void
splu_die(const char *fmt, ...);

#endif /* SPLU_H */

/*
 ===============================================================================
 |                             IMPLEMENTATION MODE                             |
 ===============================================================================
 */

#ifdef SPLU_IMPL

/*
 ===============================================================================
 |                          Function Implementations                           |
 ===============================================================================
 */

SPLU_DEF void
splu_die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt) - 1] == ':') {
		fputc(' ', stderr);
		if (errno != 0)
			perror(NULL);
		else
			fprintf(stderr, "Something went wrong\n");
	} else {
		fputc('\n', stderr);
	}

	exit(EXIT_FAILURE);
}

#endif /* SPLU_IMPL */

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
