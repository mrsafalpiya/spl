/*
 * spl_utils.h - https://github.com/mrsafalpiya/spl
 * no warranty implied; use at your own risk

 * See end of file for license information.
*/

/*
 * ================
 * | Introduction |
 * ================
 *
 * SPL_UTILS.H - Collection of simple utility functions that I will probably
 * use in all of my C programs
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
 *         #define SPL_UTILS_IMPLEMENTATION
 *
 * before you include this file in *one* C or C++ file to create the
 * implementation.
*/

/*
 * ====================
 * | Header-file mode |
 * ====================
*/

#ifndef SPL_UTILS_H
#define SPL_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

/* = functions = */

void
spl_utils_die(const char *fmt, ...);
/*
 * Writes out the passed format to stderr and kill the program
 * Copied from git.suckless.org/dwm
 *
 * Automatically adds a new line to the end
 *
 * If a semicolon `:` is present at the end of the string, perror() is also
 * called appending the error
*/

#endif /* SPL_UTILS_H */

/*
 * ===================
 * | Implementations |
 * ===================
*/

#ifdef SPL_UTILS_IMPLEMENTATION

/* functions */
void
spl_utils_die(const char *fmt, ...)
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

#endif /* SPL_UTILS_IMPLEMENTATION */

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
