/**
 * zlisp - a stack based, RPN lisp-like language
 *
 * Copyright 2015 zach wick <zach@zachwick.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>

// zlval (zlisp value) struct
typedef struct {
	int type;
	long num;
	int err;
} zlval;

// Enum of possible zlval types
enum {
	ZLVAL_NUM,
	ZLVAL_ERR
};

// Enum of possible error types
enum {
	ZLERR_DIV_ZERO,
	ZLERR_BAD_OP,
	ZLERR_BAD_NUM
};

// Create a new number type zlval
zlval
zlval_num (long x) {
	zlval v;
	v.type = ZLVAL_NUM;
	v.num = x;
	return v;
}

// Create an error type zlval
zlval
zlval_err (int x) {
	zlval v;
	v.type = ZLVAL_ERR;
	v.err = x;
	return v;
}

// Print a zlval
void
zlval_print (zlval v) {
	switch (v.type) {
	case ZLVAL_NUM:
		// If the zlval is a number type, print it then break
		printf ("%li", v.num);
		break;
	case ZLVAL_ERR:
		// If the zlval is an error, print a meaningful error message
		if (v.err == ZLERR_DIV_ZERO) {
			printf ("Error: Division by Zero");
		}
		if (v.err == ZLERR_BAD_OP) {
			printf ("Error: Bad Operator");
		}
		if (v.err == ZLERR_BAD_NUM) {
			printf ("Error: Bad Number");
		}
		break;
	}
}

// Print a zlval followed by a newline
void
zlval_println (zlval v) {
	zlval_print (v);
	putchar ('\n');
}

int
main (int argc, char** argv) {
	puts ("zlisp version 0.01");
	puts ("Use Ctrl-C to Exit\n");

	// Go into an infinite loop
	for (;;) {
		// Output the prompt and get the input
		char* input = readline ("zlisp> ");

		// Add the input to the history
		add_history (input);

		// Parse the input and print the result
		//zlval result = eval(input);
		puts (input);
		
		// Free the recieved input
		free (input);
	}
	return 0;
}
