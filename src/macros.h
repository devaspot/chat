//////////////////////////////////////////////////
// Generic [macros.h]
//     A collection of common macros that can be
//     used in any program.
//
//     WARNING: You should be very careful with
//     macros, especially when forming compound
//     statements.
//////////////////////////////////////////////////

#ifndef MACROS_H
#define MACROS_H

// Deallocates a pointer and cleanly sets it to NULL.
#define DELETE(x)           { delete   x; x = NULL; }

// Deallocates an array pointer and cleanly sets it to NULL.
#define DELETE_ARRAY(x)     { delete[] x; x = NULL; }

// Recommended for (char *) strings only. A common idiom.
// Returns true if the string *appears* allocated and isn't empty.
#define NON_EMPTY_STRING(x) (x && *x)

#endif