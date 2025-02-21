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

BrainfuckInstruction *brainfuck_parse_stream(FILE *stream) {
  return brainfuck_parse_stream_until(stream, EOF);
}

BrainfuckInstruction *brainfuck_parse_stream_until(FILE *stream,
                                                   const int until) {
  BrainfuckInstruction *instruction =
      (BrainfuckInstruction *)malloc(sizeof(BrainfuckInstruction));
  instruction->next = 0;
  instruction->loop = 0;
  BrainfuckInstruction *root = instruction;
  char ch;
  char temp;
  while ((ch = fgetc(stream)) != until) {
    if (ch == EOF || feof(stream))
      break;
    instruction->type = ch;
    instruction->difference = 1;
    switch (ch) {
    case BRAINFUCK_TOKEN_PLUS:
    case BRAINFUCK_TOKEN_MINUS:
      while ((temp = fgetc(stream)) != until &&
             (temp == BRAINFUCK_TOKEN_PLUS || temp == BRAINFUCK_TOKEN_MINUS)) {
        if (temp == ch) {
          instruction->difference++;
        } else {
          instruction->difference--;
        }
      }
      ungetc(temp, stream);
      break;
    case BRAINFUCK_TOKEN_NEXT:
    case BRAINFUCK_TOKEN_PREVIOUS:
      while (
          (temp = fgetc(stream)) != until &&
          (temp == BRAINFUCK_TOKEN_NEXT || temp == BRAINFUCK_TOKEN_PREVIOUS)) {
        if (ch == temp) {
          instruction->difference++;
        } else {
          instruction->difference--;
        }
      }
      ungetc(temp, stream);
      break;
    case BRAINFUCK_TOKEN_OUTPUT:
    case BRAINFUCK_TOKEN_INPUT:
      while ((temp = fgetc(stream)) != until && (temp == ch)) {
        instruction->difference++;
      }
      ungetc(temp, stream);
      break;
    case BRAINFUCK_TOKEN_LOOP_START:
      instruction->loop = brainfuck_parse_stream_until(stream, until);
      break;
    case BRAINFUCK_TOKEN_LOOP_END:
      return root;
    default:
      continue;
    }
    instruction->next =
        (BrainfuckInstruction *)malloc(sizeof(BrainfuckInstruction));
    instruction->next->next = 0;
    instruction->next->loop = 0;
    instruction = instruction->next;
  }
  instruction->type = BRAINFUCK_TOKEN_LOOP_END;
  return root;
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
