#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#include <stddef.h>
#include <stdio.h>
#define BRAINFUCK_TAPE_SIZE 30000

#define BRAINFUCK_EOF_BEHAVIOUR 1

#define BRAINFUCK_TOKEN_PLUS '+'
#define BRAINFUCK_TOKEN_MINUS '-'
#define BRAINFUCK_TOKEN_PREVIOUS '<'
#define BRAINFUCK_TOKEN_NEXT '>'
#define BRAINFUCK_TOKEN_OUTPUT '.'
#define BRAINFUCK_TOKEN_INPUT ','
#define BRAINFUCK_TOKEN_LOOP_START '['
#define BRAINFUCK_TOKEN_LOOP_END ']'

typedef struct BrainfuckInstruction {
  int difference;
  char type;
  struct BrainfuckInstruction *next;
  struct BrainfuckInstruction *previous;
  struct BrainfuckInstruction *loop;
} BrainfuckInstruction;

typedef struct BrainfuckState {
  struct BrainfuckInstruction *root;
  struct BrainfuckInstruction *head;
} BrainfuckState;

typedef int (*BrainfuckOutputHandler)(int chr);
typedef char (*BrainfuckInputHandler)(void);

typedef struct BrainfuckExecutionContext {
  BrainfuckOutputHandler output_handler;
  BrainfuckInputHandler input_handler;
  unsigned char *tape;
  int tape_index;
  size_t tape_size;
  int shouldStop;
} BrainfuckExecutionContext;

/*
 * functions for print debugging purpose.
 */
void brainfuck_print_instruction(BrainfuckInstruction *instructions);
void brainfuck_print_instructions(BrainfuckInstruction *root);
void brainfuck_print_state(BrainfuckState *state);

BrainfuckInstruction *brainfuck_instruction();

BrainfuckState *brainfuck_state();

BrainfuckExecutionContext *brainfuck_context(int);

BrainfuckInstruction *brainfuck_remove(BrainfuckState *state,
                                       BrainfuckInstruction *instruction);

BrainfuckInstruction *brainfuck_add(BrainfuckState *state,
                                    BrainfuckInstruction *instruction);

BrainfuckInstruction *brainfuck_add_first(BrainfuckState *state,
                                          BrainfuckInstruction *instruction);
BrainfuckInstruction *
brainfuck_insert_before(BrainfuckState *state,
                        BrainfuckInstruction *instruction);

BrainfuckInstruction *brainfuck_parse_stream(FILE *stream);
BrainfuckInstruction *brainfuck_parse_stream_until(FILE *stream,
                                                   const int until);

BrainfuckInstruction *brainfuck_parse_string(char *str);
BrainfuckInstruction *brainfuck_parse_substring(char *str, int begin, int end);
BrainfuckInstruction *brainfuck_parse_substring_incremental(char *str, int *ptr,
                                                            int end);

void brainfuck_destroy_instruction(BrainfuckInstruction *instruction);
void brainfuck_destroy_instructions(BrainfuckInstruction *root);
void brainfuck_destroy_state(BrainfuckState *state);
void brainfuck_destroy_context(BrainfuckExecutionContext *context);

void brainfuck_execute(BrainfuckInstruction *root,
                       BrainfuckExecutionContext *context);

char brainfuck_getchar();
#endif