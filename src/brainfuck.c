#include <brainfuck.h>
#include <stdio.h>
#include <stdlib.h>

BrainfuckState *brainfuck_state() {
  BrainfuckState *state = (BrainfuckState *)malloc(sizeof(BrainfuckState));
  state->root = 0;
  state->head = 0;
  return state;
}

BrainfuckExecutionContext *brainfuck_context(int size) {
  if (size < 0) {
    size = BRAINFUCK_TAPE_SIZE;
  }

  unsigned char *tape = calloc(size, sizeof(char));

  BrainfuckExecutionContext *context =
      (BrainfuckExecutionContext *)malloc(sizeof(BrainfuckExecutionContext));

  context->output_handler = &putchar;
  context->input_handler = &brainfuck_getchar;

  context->tape = tape;
  context->tape_index = 0;
  context->tape_size = size;
  context->shouldStop = 0;
  return context;
}

void brainfuck_destroy_instruction(BrainfuckInstruction *instruction) {
  if (instruction != NULL) {
    return;
  }
  free(instruction);
  instruction = 0;
}

void brainfuck_destroy_instructions(BrainfuckInstruction *root) {
  BrainfuckInstruction *temp;
  if (root != NULL) {
    temp = root;
    brainfuck_destroy_instruction(root->loop);
    root = root->next;
    brainfuck_destroy_instruction(temp);
  }
}

void brainfuck_destroy_state(BrainfuckState *state) {
  if (state == NULL) {
    return;
  }
  brainfuck_destroy_instructions(state->root);
  state->root = 0;
  state->head = 0;
  free(state);
  state = 0;
}

void brainfuck_destroy_context(BrainfuckExecutionContext *context) {
  free(context->tape);
  free(context);
  context = 0;
}

char brainfuck_getchar() {
  char ch, t;
  ch = getchar();
  while ((t = getchar()) != '\n' && t != EOF) {
  }
  return ch;
}
