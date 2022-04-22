/*
 * spl_path.h - https://github.com/mrsafalpiya/spl
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
 * SPL_PATH.H - Simple string path manipulation library inspired by golang's
 * path library
 *
 * Mostly a port from
 * https://cs.opensource.google/go/go/+/refs/tags/go1.17.8:src/path/path.go
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
 *         #define SPL_PATH_IMPLEMENTATION
 *
 * before you include this file in *one* C or C++ file to create the
 * implementation.
 */

/*
 *  _   _                _                 __ _ _
 * | | | | ___  __ _  __| | ___ _ __      / _(_) | ___
 * | |_| |/ _ \/ _` |/ _` |/ _ \ '__|____| |_| | |/ _ \
 * |  _  |  __/ (_| | (_| |  __/ | |_____|  _| | |  __/
 * |_| |_|\___|\__,_|\__,_|\___|_|       |_| |_|_|\___|
 *
 */

#ifndef SPL_PATH_H
#define SPL_PATH_H

/*
 *******************************************************************************
 =================================== Functions =================================
 *******************************************************************************
 */

char *
spl_path_clean(char *path, int len, char *clean_path);
/*
 * Returns the shortest path name equivalent to path by purely lexical
 * processing. It applies the following rules iteratively until no further
 * processing can be done:
 *
 *	1. Replace multiple slashes with a single slash.
 *	2. Eliminate each . path name element (the current directory).
 *	3. Eliminate each inner .. path name element (the parent directory)
 *	   along with the non-.. element that precedes it.
 *	4. Eliminate .. elements that begin a rooted path:
 *	   that is, replace "/.." by "/" at the beginning of a path.
 *
 * The returned path ends in a slash only if it is the root "/".
 *
 * If the result of this process is an empty string, Clean
 * returns the string ".".
 *
 * See also Rob Pike, ``Lexical File Names in Plan 9 or
 * Getting Dot-Dot Right,''
 * https://9p.io/sys/doc/lexnames.html
 *
 * len argument specifies the length through which the processing is to be done.
 * Pass 0 to make the function process throughout the whole string.
 *
 * If the clean_path passed is NULL, a new string is malloc'ed which has to be
 * free'ed later.
 */

char *
spl_path_join(char *path, ...);
/*
 * Join joins any number of path elements into a single path, separating them
 * with slashes.
 * Empty elements are ignored.
 *
 * NOTE: The function call should have a NULL at the end of the arguments.
 * Example: spl_path_join("foo", "bar", "buzz", NULL);
 *
 * The result is Cleaned. However, if the argument list is empty or all its
 * elements are empty, Join returns an empty string.
 *
 * Function returns a new malloc'ed string which has to be free'ed later.
 */

char *
spl_path_dir(char *path, char *dir);
/*
 * Returns all but the last element of path, typically the path's directory.
 * The path is Cleaned and trailing slashes are removed.
 *
 * If the path is empty, Dir returns ".".
 * If the path consists entirely of slashes followed by non-slash bytes, Dir
 * returns a single slash. In any other case, the returned path does not end in
 * a slash.
 *
 * If the dir passed is NULL, a new string is malloc'ed which has to be free'ed
 * later.
 */

char *
spl_path_base(char *path);
/*
 * Returns the last element of path.
 * Trailing slashes are removed before extracting the last element.
 *
 * If the path is empty, Base returns ".".
 * If the path consists entirely of slashes, Base returns "/".
 *
 * Returned string is a new string malloc'ed which has to be free'ed later.
 */

void
spl_path_change_base(char *path, char *new_base);
/*
 * Changes the base of the given string `path` to a new base `base`.
 *
 * In case of empty or base-only string, the `new_base` is filled in the `path`.
 */

#endif /* SPL_PATH_H */

/*
 *  ___                 _                           _        _   _
 * |_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __
 *  | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \
 *  | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
 * |___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
 *               |_|
 */

#ifdef SPL_PATH_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* functions */
char *
spl_path_clean(char *path, int len, char *clean_path)
{
	char *buf_string;
	int   is_rooted, n, r, w, dotdot;
	/*
	 * reading from path; r = index of next byte to process
	 * writing to buf; w = index of next byte to write
	 * dotdot = index in buf_string where .. must stop, either because it is
	 * the leading slash or it is a leading ../../.. prefix.
	 */

	/* Check if the clean_path is NULL and allocate memory for the buffer */
	if (clean_path == NULL)
		buf_string = malloc(4096 * sizeof(char));
	else {
		buf_string    = clean_path;
		buf_string[0] = '\0'; /* abandon any previous string */
	}

	/* Check if the given path is an empty string */
	if (path[0] == '\0') {
		strcpy(buf_string, ".");
		return buf_string;
	}

	is_rooted = (path[0] == '/');
	if (len > 0)
		n = len;
	else
		n = strlen(path);

	r      = 0;
	w      = 0;
	dotdot = 0;

	if (is_rooted) {
		buf_string[w++] = '/';
		r               = 1;
		dotdot          = 1;
	}

	while (r < n) {
		switch (path[r]) {
		case '/':
			/* empty path element */
			r++;
			break;
		case '.':
			if (r + 1 == n || path[r + 1] == '/') {
				/* . element */
				r++;
				break;
			}
			if (path[r + 1] == '.' &&
			    (r + 2 == n || path[r + 2] == '/')) {
				/* .. element: remove to last */
				r += 2;
				if (w > dotdot) {
					/* can backtrack */
					w--;
					while (w > dotdot &&
					       buf_string[w] != '/')
						w--;
					break;
				}
				if (!is_rooted) {
					/* cannot backtrack, but not rooted, so
					 * append .. element */
					if (w > 0) {
						buf_string[w++] = '/';
					}
					buf_string[w++] = '.';
					buf_string[w++] = '.';
					dotdot          = w;
					break;
				}
			}
		default:
			/*
			 * real path element.
			 * add slash if needed
			*/
			if ((is_rooted && w != 1) || (!is_rooted && w != 0)) {
				buf_string[w++] = '/';
			}
			/* copy element */
			for (; r < n && path[r] != '/'; r++) {
				buf_string[w++] = path[r];
			}
		}
	}

	/* turn empty string into "." */
	if (w == 0) {
		strcpy(buf_string, ".");
		return buf_string;
	}

	/* end the string */
	buf_string[w] = '\0';

	return buf_string;
}

char *
spl_path_join(char *path, ...)
{
	va_list paths;
	char   *buf_string, *cur_path, *clean_path;
	int     n;

	buf_string = malloc(4096 * sizeof(char));

	va_start(paths, path);
	cur_path = path;
	n        = 0;

	strcpy(buf_string, "");
	do {
		/* ignore empty paths */
		if (cur_path[0] != '\0') {
			strcat(buf_string, cur_path);
			strcat(buf_string, "/");
			n++;
		}
	} while ((cur_path = va_arg(paths, char *)) != NULL);

	/* check if there are no valid argument passed */
	if (n == 0) {
		buf_string[0] = '\0';
		return buf_string;
	}

	clean_path = spl_path_clean(buf_string, 0, NULL);

	free(buf_string);
	return clean_path;
}

int
last_slash_index(char *str)
{
	int i;

	i = strlen(str) - 1;
	while (i >= 0 && str[i] != '/')
		i--;

	return i;
}

char *
spl_path_dir(char *path, char *dir)
{
	/* clean only the non-base part of the path (excluding the last slash) */
	dir = spl_path_clean(path, last_slash_index(path) + 1, dir);

	return dir;
}

char *
spl_path_base(char *path)
{
	char *buf_string, *ret_string;
	int   i;

	/* clean the path */
	buf_string = spl_path_clean(path, 0, NULL);

	/* extract the last element */
	i = last_slash_index(buf_string);
	if (i >= 0)
		ret_string = strdup(buf_string + i + 1);
	else
		ret_string = strdup(buf_string);

	/* if empty now, it had only slashes */
	if (ret_string[0] == '\0')
		strcpy(ret_string, "/");

	free(buf_string);
	return ret_string;
}

void
spl_path_change_base(char *path, char *new_base)
{
	int n;

	n = last_slash_index(path);

	/* in case of empty or base-only string */
	if (n == -1) {
		strcpy(path, new_base);
		return;
	}

	if (n > 0) {
		path[++n] = '\0';
		strcat(path, new_base);
	}
}

#endif /* SPL_PATH_IMPLEMENTATION */

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
