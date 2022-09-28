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
 - v0.2
     - Modified identifier names from 'spl_str*' to 'spl_str_*'.
     - Added `does_begin_with()`, `does_begin_with_case()`, `does_end_with()`,
       `does_end_with_case()`, `toupper()`, `toupper_dup()`, `tolower()`,
       `tolower_dup()`.
 - v0.1
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
 * `len` is the length of the `str`. It can also be the number of bytes you want
 * to clean. Pass -1 if you want the function to calculate it.
 *
 * Cleaning is done on the original string in destructive manner (moving and
 * editing bytes within the string).
 *
 * NOTE: NOT to be called on a read-only memory as it WILL CAUSE SEGFAULT. In
 * which case run 'spl_str_clean_dup()'.
 */
SPL_STR_DEF void
spl_str_clean(char *str, int len);

/* Same as 'spl_str_clean()' but first a duplicate of the original string is
 * created. Thus this function can be called on read-only strings too. As the
 * result is a dynamically allocated string, it has to be free'ed later. */
SPL_STR_DEF char *
spl_str_clean_dup(const char *str, int len);

/*
 * Because 'strndup()' is not standard.
 *
 * `len` is the length of the `str`. It can also be the number of bytes you want
 * to duplicate. Pass -1 if you want the function to calculate it.
 */
SPL_STR_DEF char *
spl_str_dup(const char *str, int len);

/*
 * Check if the given string `str` begins with the given `begin_str`.
 *
 * `len` is the length of the `begin_str`. It can also be the number of bytes
 * you want to compare. Pass -1 if you want the function to calculate it.
 *
 * Returns 1 if it does begin with it OR 0 if it doesn't.
 */
SPL_STR_DEF int
spl_str_does_begin_with(const char *str, const char *begin_str,
                        int len_begin_str);

/* Case insensitive version of 'does_begin_with()'. */
SPL_STR_DEF int
spl_str_does_begin_with_case(const char *str, const char *begin_str,
                             int len_begin_str);

/*
 * Check if the given string `str` ends with the given `end_str`.
 *
 * `str_len` is the length of the `str`. `cmp_len` is the length of the
 * `end_str`. It can also be the number of bytes you want to compare. Pass -1
 * if you want the function to calculate it.
 *
 * Returns 1 if it does begin with it OR 0 if it doesn't.
 */
SPL_STR_DEF int
spl_str_does_end_with(const char *str, const char *end_str, int len_str,
                      int len_end_str);

/* Case insensitive version of 'does_end_with()'. */
SPL_STR_DEF int
spl_str_does_end_with_case(const char *str, const char *end_str, int len_str,
                           int len_end_str);

/*
 * Change the whole string to uppercase.
 *
 * Operation is done on the original string in destructive manner (editing
 * bytes within the string). Safe to pass NULL.
 *
 * NOTE: NOT to be called on a read-only memory as it WILL CAUSE SEGFAULT. In
 * which case run 'spl_str_toupper_dup()'.
 */
SPL_STR_DEF void
spl_str_toupper(char *str, int len);

/* Same as 'spl_str_toupper()' but first a duplicate of the original string is
 * created. Thus this function can be called on read-only strings too. As the
 * result is a dynamically allocated string, it has to be free'ed later. */
SPL_STR_DEF char *
spl_str_toupper_dup(const char *str, int len);

/*
 * Change the whole string to lowercase.
 *
 * Operation is done on the original string in destructive manner (editing
 * bytes within the string). Safe to pass NULL.
 *
 * NOTE: NOT to be called on a read-only memory as it WILL CAUSE SEGFAULT. In
 * which case run 'spl_str_tolower_dup()'.
 */
SPL_STR_DEF void
spl_str_tolower(char *str, int len);

/* Same as 'spl_str_tolower()' but first a duplicate of the original string is
 * created. Thus this function can be called on read-only strings too. As the
 * result is a dynamically allocated string, it has to be free'ed later. */
SPL_STR_DEF char *
spl_str_tolower_dup(const char *str, int len);

#endif /* SPL_STR_H */

/*
 ===============================================================================
 |                             IMPLEMENTATION MODE                             |
 ===============================================================================
 */

#ifdef SPL_STR_IMPL

#include <ctype.h>

/*
 ===============================================================================
 |                          Function Implementations                           |
 ===============================================================================
 */

SPL_STR_DEF void
spl_str_clean(char *str, int len)
{
	if (len < 0)
		len = strlen(str);

	/* Remove leading whitespaces */
	int   lead_ws_c = 0;
	char *startp    = str;
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
spl_str_clean_dup(const char *str, int len)
{
	if (len < 0)
		len = strlen(str);

	char *str_cleaned = spl_str_dup(str, len);
	spl_str_clean(str_cleaned, len);

	return str_cleaned;
}

SPL_STR_DEF char *
spl_str_dup(const char *str, int len)
{
	if (len < 0)
		len = strlen(str);

	char *str_duped = malloc(len + 1);
	if (!str_duped)
		return NULL;

	memcpy(str_duped, str, len);
	str_duped[len] = '\0';

	return str_duped;
}

SPL_STR_DEF int
spl_str_does_begin_with(const char *str, const char *begin_str,
                        int len_begin_str)
{
	if (len_begin_str < 0)
		len_begin_str = strlen(begin_str);

	if (!strncmp(str, begin_str, len_begin_str))
		return 1;
	return 0;
}

SPL_STR_DEF int
spl_str_does_begin_with_case(const char *str, const char *begin_str,
                             int len_begin_str)
{
	if (len_begin_str < 0)
		len_begin_str = strlen(begin_str);

	for (int i = 0; i < len_begin_str; i++) {
		if (tolower(begin_str[i]) != tolower(str[i]))
			return 0;
	}
	return 1;
}

SPL_STR_DEF int
spl_str_does_end_with(const char *str, const char *end_str, int len_str,
                      int len_end_str)
{
	if (len_str < 0)
		len_str = strlen(str);
	if (len_end_str < 0)
		len_end_str = strlen(end_str);

	if (len_end_str > len_str)
		return 0;

	for (int i = 0; i < len_end_str; i++) {
		if (str[len_str - 1 - i] != end_str[len_end_str - 1 - i])
			return 0;
	}
	return 1;
}

SPL_STR_DEF int
spl_str_does_end_with_case(const char *str, const char *end_str, int len_str,
                           int len_end_str)
{
	if (len_str < 0)
		len_str = strlen(str);
	if (len_end_str < 0)
		len_end_str = strlen(end_str);

	if (len_end_str > len_str)
		return 0;

	for (int i = 0; i < len_end_str; i++) {
		if (tolower(str[len_str - 1 - i]) !=
		    tolower(end_str[len_end_str - 1 - i]))
			return 0;
	}
	return 1;
}

SPL_STR_DEF void
spl_str_toupper(char *str, int len)
{
	if (len < 0)
		len = strlen(str);

	for (int i = 0; i < len; i++)
		str[i] = toupper(str[i]);
}

SPL_STR_DEF char *
spl_str_toupper_dup(const char *str, int len)
{
	if (len < 0)
		len = strlen(str);

	char *str_touppered = spl_str_dup(str, len);
	spl_str_toupper(str_touppered, len);

	return str_touppered;
}

SPL_STR_DEF void
spl_str_tolower(char *str, int len)
{
	if (len < 0)
		len = strlen(str);

	for (int i = 0; i < len; i++)
		str[i] = tolower(str[i]);
}

SPL_STR_DEF char *
spl_str_tolower_dup(const char *str, int len)
{
	if (len < 0)
		len = strlen(str);

	char *str_tolowered = spl_str_dup(str, len);
	spl_str_tolower(str_tolowered, len);

	return str_tolowered;
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
