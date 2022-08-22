/*
 ===============================================================================
 |                                 spl_path.h                                  |
 |                     https://github.com/mrsafalpiya/spl                      |
 |                                                                             |
 |         Path manipulation library inspired by golang's Path library         |
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
 *         #define SPLP_IMPL
 *
 * before you include this file in *one* C or C++ file to create the
 * implementation.
 */

/*
 ===============================================================================
 |                              HEADER-FILE MODE                               |
 ===============================================================================
 */

#ifndef SPL_PATH_H
#define SPL_PATH_H

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif

/*
 ===============================================================================
 |                                   Options                                   |
 ===============================================================================
 */

/* = SPL_PATH = */
#ifndef SPLP_DEF
#define SPLP_DEF /* You may want `static` or `static inline` here */
#endif

/*
 ===============================================================================
 |                            Function Declarations                            |
 ===============================================================================
 */

/*
 * Returns pointer to the last element of path.
 *
 * Trailing slashes are removed before extracting the last element. If the path
 * is empty, "." is returned. If the path consists entirely of slashes, "/" is
 * returned.
 *
 * `*len` is filled with the length of the base. NULL can be passed but it is
 * encouraged to work with its length.
 *
 * As a simple pointer within the original path is returned without any
 * cleaning, the pointer returned may not be the proper base (containing
 * trailing slashes) in which case `*len` is encouraged to be non-NULL.
 *
 * If `to_dup` is non-zero, a new dynamically allocated string with 'strndup()'
 * is returned which has to be free'ed later.
 */
SPLP_DEF char *
splp_base(const char *path, int to_dup, int *len);

/*
 * Returns the shortest path name equivalent to path by purely lexical
 * processing. It applies the following rules iteratively until no further
 * processing can be done:
 *
 * 1. Replace multiple slashes with a single slash.
 * 2. Eliminate each . path name element (the current directory).
 * 3. Eliminate each inner .. path name element (the parent directory) along
 *    with the non-.. element that precedes it.
 * 4. Eliminate .. elements that begin a rooted path:
 *    that is, replace "/.." by "/" at the beginning of a path.
 *
 * The returned path ends in a slash only if it is the root "/". If the result
 * of this process is an empty string, "." is returned.
 *
 * See also Rob Pike, "Lexical File Names in Plan 9" or "Getting Dot-Dot Right"
 * https://9p.io/sys/doc/lexnames.html
 *
 * The returned string is always a dynamically allocated string and thus has to
 * be free'ed later.
 */
SPLP_DEF char *
splp_clean(const char *path);

/*
 * Returns the path's directory i.e. all but the last element of path.
 *
 * If the path is empty, "." is returned. If the path consists entirely of
 * slashes followed by non-slashes, a single slash is returned. In any other
 * case, the returned path doesn't end in a slash.
 *
 * `*len` is filled with the length of the dir. NULL can be passed but idk why
 * would you ever want to do it.
 *
 * Except in the case of path being empty, the original path is returned without
 * any cleaning. So, the pointer returned may not be the proper dir (containing
 * trailing slashes along with the base). Thus `*len` should be non-NULL.
 *
 * If `to_dup` is non-zero, a new dynamically allocated string with 'strndup()'
 * is returned which has to be free'ed later.
 */
SPLP_DEF char *
splp_dir(const char *path, int to_dup, int *len);

/*
 * Returns the file name extension used by path.
 *
 * NULL is returned if there is no dot OR the dot is at the end.
 *
 * If `to_dup` is non-zero, a new dynamically allocated string with 'strndup()'
 * is returned which has to be free'ed later.
 */
SPLP_DEF char *
splp_ext(const char *path, int to_dup);

/* Returns whether the path is absolute. */
SPLP_DEF int
splp_is_abs(const char *path);

/* Joins any number of path elements which ends with NULL into a single path,
 * separating them with slashes.
 *
 * Empty elements are ignored. The result is passed through 'clean()'. If the
 * argument list is empty or all its elements are empty, an empty string is
 * returned.
 *
 * The returned string is always a dynamically allocated string and thus has to
 * be free'ed later.
 */
SPLP_DEF char *
splp_join(const char *paths, ...);
/* TODO: Make non-dynamic version of 'join()' */

#endif /* SPL_PATH_H */

/*
 ===============================================================================
 |                             IMPLEMENTATION MODE                             |
 ===============================================================================
 */

#ifdef SPLP_IMPL

/*
 ===============================================================================
 |                          Function Implementations                           |
 ===============================================================================
 */

SPLP_DEF char *
splp_base(const char *path, int to_dup, int *len)
{
	/* NULL pointer or empty path */
	if (!path || path[0] == '\0') {
		if (len)
			*len = 1;
		if (to_dup)
			return strdup(".");
		return ".";
	}

	/* get pointer to the end */
	char *endp = (char *)path + strlen(path) - 1;

	/* strip trailing slashes */
	while (endp > path && *endp == '/')
		endp--;

	/* in case only slashes were provided */
	if (endp == path && *endp == '/') {
		if (len)
			*len = 1;
		if (to_dup)
			return strdup("/");
		return "/";
	}

	/* point to the start of the base */
	char *startp = endp;
	while (startp > path && *(startp - 1) != '/')
		startp--;

	if (len)
		*len = endp - startp;

	if (to_dup)
		return strndup(startp, endp - startp + 1);
	return startp;
}

SPLP_DEF char *
splp_clean(const char *path)
{
	/* NULL pointer or empty path */
	if (!path || path[0] == '\0')
		return strdup(".");

	int is_rooted = path[0] == '/';

	size_t path_len = strlen(path);

	char *buf = malloc(PATH_MAX * sizeof(char)); /* '+ 1' for \0 */

	/* reading from path; 'r' is index of next byte to process.
	 * writing to buf;    'w' is index of next byte to write.
	 * 'dotdot' is index in 'buf' where .. must stop, either becuase it is
	 * the leading slash or it is a leading ../../.. prefix. */
	size_t r      = 0;
	size_t w      = 0;
	size_t dotdot = 0;

	if (is_rooted) {
		buf[w++] = '/';
		r        = 1;
		dotdot   = 1;
	}

	while (r < path_len) {
		switch (path[r]) {
		case '/':
			/* empty path element */
			r++;
			break;
		case '.':
			if (r + 1 == path_len || path[r + 1] == '/') {
				/* . element */
				r++;
				break;
			}
			if (path[r + 1] == '.' &&
			    (r + 2 == path_len || path[r + 2] == '/')) {
				/* .. element: remove to last / */
				r += 2;

				if (w > dotdot) {
					w--;
					while ((w > dotdot) && (buf[w] != '/'))
						w--;
					break;
				}

				if (!is_rooted) {
					if (w > 0)
						buf[w++] = '/';
					buf[w++] = '.';
					buf[w++] = '.';

					dotdot = w;
				}

				break;
			}
			break;
		default:
			/* real path element */
			/* add slash if needed */
			if ((is_rooted && w != 1) || (!is_rooted && w != 0))
				buf[w++] = '/';

			/* copy element */
			for (; (r < path_len) && (path[r] != '/'); r++)
				buf[w++] = path[r];
		}
	}

	/* turn empty string into "." */
	if (w == 0)
		buf[w++] = '.';

	buf[w] = '\0';

	return buf;
}

SPLP_DEF char *
splp_dir(const char *path, int to_dup, int *len)
{
	/* NULL pointer or empty path */
	if (!path || path[0] == '\0') {
		if (len)
			*len = 1;
		if (to_dup)
			return strdup(".");
		return ".";
	}

	/* point to the last slash */
	char *last_slash = strrchr(path, '/');

	/* if there are no slashes (the path given is base only) */
	if (!last_slash) {
		if (len)
			*len = 1;
		if (to_dup)
			return strdup(".");
		return ".";
	}

	/* strip trailing slashes */
	while (last_slash > path && *last_slash == '/')
		last_slash--;

	if (len)
		*len = last_slash - path;

	if (to_dup)
		return strndup(path, last_slash - path + 1);
	return (char *)path;
}

SPLP_DEF char *
splp_ext(const char *path, int to_dup)
{
	/* NULL pointer or empty path */
	if (!path || path[0] == '\0')
		return NULL;

	for (int i = strlen(path) - 1; i >= 0 && path[i] != '/'; i--) {
		if (i > 0 && path[i - 1] == '.') {
			if (to_dup)
				return strdup(path + i);
			return (char *)path + i;
		}
	}

	return NULL;
}

SPLP_DEF int
splp_is_abs(const char *path)
{
	/* NULL pointer or empty path */
	if (!path)
		return 0;

	return strlen(path) > 0 && path[0] == '/';
}

SPLP_DEF char *
splp_join(const char *paths, ...)
{
	va_list ap;

	char  path[PATH_MAX];
	path[0] = '\0';
	char *cur_path       = (char *)paths;

	va_start(ap, paths);
	for (; cur_path; cur_path = va_arg(ap, char *)) {
		if (cur_path[0] == '\0')
			continue;

		if (path[0] != '\0')
			strcat(path, "/");
		strcat(path, cur_path);
	}
	va_end(ap);

	/* empty path */
	if (path[0] == '\0')
		return strdup("");

	return splp_clean(path);
}

#endif /* SPLP_IMPL */

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
