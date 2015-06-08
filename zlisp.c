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
#include <string.h>
#include <stdbool.h>
#include <editline/readline.h>
#include <editline/history.h>

// zlval (zlisp value) struct
typedef struct {
	int type;
	double num;
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

// All operators and operands are each an Op
typedef struct {
	char* name;
	int type;
	int argc;
} Op;

// Enum of possible Op types
enum {
	ZLOP_OPR,
	ZLOP_OPD
};

// Operator and Operand stack
typedef struct {
	Op * ops;
	size_t length;
} Stack;
	
// Create an initial Stack
void
init_stack (Stack* stack) {
	memset (stack, 0, sizeof (Stack));
}

// Add one op to a stack
bool
add_op (Stack* stack, Op op) {
	const size_t chunk_size = 1024;
	if (stack->length % chunk_size == 0) {
		// Try to extend the array
		Op * new_ops = (Op *) realloc (stack->ops,
		                               (stack->length + chunk_size) * sizeof(Op));
		if (!new_ops) {
			return false;
		}
		stack->ops = new_ops;
	}
	stack->ops[stack->length++] = op;
	return true;
}

// Create a new Op
Op
op_create (char * opchar) {
	Op op;
	op.name = opchar;   

	if (strcmp(op.name, "+")==0 || strcmp(op.name, "-")==0 || strcmp(op.name, "*")==0 || strcmp(op.name, "/")==0) {
		op.type = ZLOP_OPR;
		op.argc = 2;
	}
	else if (strcmp(op.name, "!")==0) {
		op.type = ZLOP_OPR;
		op.argc = 1;
	}
	else {
		op.type = ZLOP_OPD;
		op.argc = 1;
	}

	return op;
}

// Create a new number type zlval
zlval
zlval_num (double x) {
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

// Parse a given input string to a Stack of operators and operands
Stack
parse (char* input) {
	Stack stack;
	Stack* s = &stack;

	char *pch;
	Op new_op;
	bool success = false;

	// Intialize s
	init_stack (s);

	pch = strtok (input, " ");
	while (pch != NULL) {
		// Create a new Op out of the token
		new_op = op_create(pch);
		success = add_op (s, new_op);
		if (success) {
			pch = strtok (NULL, " ");
		} else {
			puts ("PARSE ERROR");
		}
	}
	return stack;
}

// Evaluate a input string
zlval
eval (char* input) {
	Stack parsed_input = parse(input);
	zlval result;
	
	// DEGUG printing
	/* for (int i = 0; i<parsed_input.length; i++) { */
	/* 	puts (parsed_input.ops[i].name); */
	/* } */
	// DEBUG end printing

	// Start with the last token, and process right -> left
	for (int i = parsed_input.length; i>=0; i--) {
		if (parsed_input.ops[i].type == ZLOP_OPR) {
			// Look at ops[i].argc and take that many more ops off ot the stack
			if (parsed_input.ops[i].argc == 2) {
				if (strcmp(parsed_input.ops[i].name, "+") == 0) {
					// Do addition
					double r = atof(parsed_input.ops[i - 2].name) + atof(parsed_input.ops[i - 1].name);
					result = zlval_num (r);
					i = i - 2;
					continue;
				}
				else if (strcmp(parsed_input.ops[i].name, "-") == 0) {
					// Do subtraction
					double r = atof(parsed_input.ops[i - 2].name) - atof(parsed_input.ops[i - 1].name);
					result = zlval_num (r);
					i = i - 2;
					continue;
				}
				else if (strcmp(parsed_input.ops[i].name, "*") == 0) {
					// Do multiplication
					double r = atof(parsed_input.ops[i - 2].name) * atof(parsed_input.ops[i - 1].name);
					result = zlval_num (r);
					i = i - 2;
					continue;
				}
				else if (strcmp(parsed_input.ops[i].name, "/") == 0) {
					// Do division
					double r = atof(parsed_input.ops[i - 2].name) / atof(parsed_input.ops[i - 1].name);
					result = zlval_num (r);
					i = i - 2;
					continue;
				}
				else {
					result = zlval_err (ZLERR_BAD_OP);
				}
			}
		}
	}
	return result;
}

// Print a zlval
void
zlval_print (zlval v) {
	switch (v.type) {
	case ZLVAL_NUM:
		// If the zlval is a number type, print it then break
		printf ("%f", v.num);
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
		//eval (input);
		zlval result = eval(input);
		zlval_println (result);
		
		// Free the recieved input
		free (input);
	}
	return 0;
}
