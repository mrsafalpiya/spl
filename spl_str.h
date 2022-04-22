/*
 * spl_str.h - https://github.com/mrsafalpiya/spl
 * no warranty implied; use at your own risk

 * See end of file for license information.
*/

/*
 * ================
 * | Introduction |
 * ================
 *
 * SPL_STR.H - Some common string manipulations
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
 *         #define SPL_STR_IMPLEMENTATION
 *
 * before you include this file in *one* C or C++ file to create the
 * implementation.
*/

/*
 * ====================
 * | Header-file mode |
 * ====================
*/

#ifndef SPL_STR_H
#define SPL_STR_H

char *
spl_str_clean(char *str);
/*
 * Trims leading and trailing *space* class from the passed string `str`.
 *
 * Returns pointer to the cleaned string.
 */

#endif /* SPL_STR_H */

/*
 * ===================
 * | Implementations |
 * ===================
*/

#ifdef SPL_STR_IMPLEMENTATION

#include <string.h>
#include <ctype.h>

char *
spl_str_clean(char *str)
{
	int i;

	i = 0;

	/* trim leading whitespaces */
	while (isspace(str[i]) != 0)
		i++;
	str += i;

	/* trim trailing whitespaces */
	i = strlen(str) - 1;
	while (isspace(str[i]) != 0)
		i--;
	str[++i] = '\0';

	return str;
}

#endif /* SPL_STR_IMPLEMENTATION */

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
