/*
 ===============================================================================
 |                                 spl_color.h                                 |
 |                     https://github.com/mrsafalpiya/spl                      |
 |                                                                             |
 |                       Color related helper functions                        |
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
 *         #define SPLCOL_IMPL
 *
 * before you include this file in *one* C or C++ file to create the
 * implementation.
 */

/*
 ===============================================================================
 |                              HEADER-FILE MODE                               |
 ===============================================================================
 */

#ifndef SPLCOL_H
#define SPLCOL_H

/*
 ===============================================================================
 |                                   Options                                   |
 ===============================================================================
 */

/* = SPLCOL = */
#ifndef SPLCOL_DEF
#define SPLCOL_DEF /* You may want `static` or `static inline` here */
#endif

/*
 ===============================================================================
 |                                   Macros                                    |
 ===============================================================================
 */

/* = HEX CONVERSION = */

/* Convert hex color representation into 3 comma-separated uint8 values */
#define SPLCOL_HEX(hex)                                       \
	((hex) >> (2 * 8)) & 0xFF, ((hex) >> (1 * 8)) & 0xFF, \
		((hex) >> (0 * 8)) & 0xFF

/* Convert hex color (including alpha) representation into 4 comma-separated
 * uint8 values */
#define SPLCOL_HEXA(hex)                                      \
	((hex) >> (3 * 8)) & 0xFF, ((hex) >> (2 * 8)) & 0xFF, \
		((hex) >> (1 * 8)) & 0xFF, ((hex) >> (0 * 8)) & 0xFF

/* Convert hex color (including alpha) representation into 4 comma-separated
 * normalized float values (mainly for opengl) */
#define SPLCOL_HEXGL(hex)                                   \
	(float)(((hex) >> (3 * 8)) & 0xFF) / 255.0,         \
		(float)(((hex) >> (2 * 8)) & 0xFF) / 255.0, \
		(float)(((hex) >> (1 * 8)) & 0xFF) / 255.0, \
		(float)(((hex) >> (0 * 8)) & 0xFF) / 255.0

/*
 ===============================================================================
 |                            Function Declarations                            |
 ===============================================================================
 */

#endif /* SPLCOL_H */

/*
 ===============================================================================
 |                             IMPLEMENTATION MODE                             |
 ===============================================================================
 */

#ifdef SPLCOL_IMPL

/*
 ===============================================================================
 |                          Function Implementations                           |
 ===============================================================================
 */

#endif /* SPLCOL_IMPL */

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
