# spl - Safal Piya's Library

My collection of single header file public domain libraries for all of my (and
probably yours) programming needs in C.

## Library collection

1. `color` -- Color related functions.
1. `flags` -- GNU-style argument parsing inspired by golang's flag.
1. `path` -- Path-string manipulation functions.
1. `str` -- C string manipulation functions.
1. `utils` -- Few utilities useful for almost all programs.

## Creating a library

Make a copy of `spl_template.h` and rename the file along with the contents
inside the file accordingly -- `<++>` are supposed to be replaced with the name
of the library. The file provides a good starting point for the library.

## Tips for scanning the library

### General

- Take a look at the "HEADER-FILE MODE" section for the documentation of all
  the constants, variables and functions declared in a header file.

- To quickly jump to the functions available in a header file, search for
  `Function Declarations`.

  Same goes for `Global Variables`, `Macros`, `Data`, `Function
  Implementations` etc.

### Functions

- To quickly jump to the function documentation and implementation, search for
  the line having the beginning word as the function name following a open
  braces.

  The first instance will be the documentation and the next instance will
  follow the implementation.

  For example: To quickly take a look at the documentation and implementation
  of `foo` function, search for `^foo(`.

### Comments

- Inside a comment including documentation of a function, components inside
  backticks ``` refer to an argument given to the current function and single
  quotes `'` refers to any token within the program including other function
  calls and variables.
