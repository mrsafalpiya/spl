/*
 ===============================================================================
 |                                  spl_str.h                                  |
 |                     https://github.com/mrsafalpiya/spl                      |
 |                                                                             |
 |                   The missing string manipulation library                   |
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
 *         #define SPL_STR_IMPL
 *
 * before you include this file in *one* C or C++ file to create the
 * implementation.
 */

/*
 ===============================================================================
 |                              HEADER-FILE MODE                               |
 ===============================================================================
 */

#ifndef SPL_STR_H
#define SPL_STR_H

#include <stdlib.h>
#include <string.h>

/*
 ===============================================================================
 |                                   Options                                   |
 ===============================================================================
 */

/* = SPL_STR = */
#ifndef SPL_STR_DEF
#define SPL_STR_DEF /* You may want `static` or `static inline` here */
#endif

/*
 ===============================================================================
 |                            Function Declarations                            |
 ===============================================================================
 */

/*
 * Remove leading and trailing whitespaces from the string.
 *
 * Cleaning is done on the original string in destructive manner (moving and
 * editing bytes within the string). Safe to pass NULL.
 *
 * NOTE: NOT to be called on a read-only memory as it WILL CAUSE SEGFAULT. In
 * which case run 'spl_strclean_dup()'.
 */
SPL_STR_DEF void
spl_strclean(char *str);

/*
 * Remove leading and trailing whitespaces from the string.
 *
 * Cleaning is done by creating a duplicate of the original string. Thus can be
 * called on read-only strings too. As the result is a dynamically allocated
 * string, it has to be free'ed later. Safe to pass NULL which will return NULL.
 */
SPL_STR_DEF char *
spl_strclean_dup(const char *str);

/* 'strdup()' */
SPL_STR_DEF char *
spl_strdup(const char *str);

#endif /* SPL_STR_H */

/*
 ===============================================================================
 |                             IMPLEMENTATION MODE                             |
 ===============================================================================
 */

#ifdef SPL_STR_IMPL

/*
 ===============================================================================
 |                          Function Implementations                           |
 ===============================================================================
 */

SPL_STR_DEF void
spl_strclean(char *str)
{
	if (!str)
		return;

	size_t len = strlen(str);

	/* Remove leading whitespaces */
	size_t lead_ws_c = 0;
	char  *startp    = str;
	while (*startp == ' ') {
		lead_ws_c++;
		startp++;
	}
	if (lead_ws_c != 0) {
		memmove(str, str + lead_ws_c, (len - lead_ws_c) * sizeof(char));
		str[len - lead_ws_c + 1] = '\0';
	}

	/* Remove trailing whitespaces */
	char *endp = str + (len - lead_ws_c) - 1;
	while (*endp == ' ')
		endp--;
	*(endp + 1) = '\0';
}

SPL_STR_DEF char *
spl_strclean_dup(const char *str)
{
	if (!str)
		return NULL;

	char *ret = spl_strdup(str);
	spl_strclean(ret);
	return ret;
}

SPL_STR_DEF char *
spl_strdup(const char *str)
{
	size_t len = strlen(str) + 1;

	char *ret = malloc(len);
	if (!ret)
		return NULL;

	return memcpy(ret, str, len);
}

#endif /* SPL_STR_IMPL */

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
