#include <brainfuck.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

void brainfuck_print_instructions(BrainfuckInstruction *root) {
  BrainfuckInstruction *iter = root;
  while (iter != NULL) {
    brainfuck_print_instruction(iter);
    iter = iter->next;
  }
}

void brainfuck_print_instruction(BrainfuckInstruction *instruction) {
  printf("%c , %d\n", instruction->type, instruction->difference);
}

void brainfuck_print_state(BrainfuckState *state) {
  brainfuck_print_instructions(state->root);
  printf("-------------------------head------------------");
  brainfuck_print_instructions(state->head);
}

BrainfuckInstruction *brainfuck_instruction() {
  BrainfuckInstruction *instruction =
      (BrainfuckInstruction *)malloc(sizeof(BrainfuckInstruction));
  instruction->difference = 0;
  instruction->next = NULL;
  instruction->previous = NULL;
  instruction->loop = NULL;
  return instruction;
}

BrainfuckState *brainfuck_state() {
  BrainfuckState *state = (BrainfuckState *)malloc(sizeof(BrainfuckState));
  state->root = NULL;
  state->head = NULL;
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

BrainfuckInstruction *brainfuck_remove(BrainfuckState *state,
                                       BrainfuckInstruction *instruction) {
  if (instruction == NULL || state == NULL) {
    return NULL;
  }
  if (state->head == instruction) {
    state->head->next = instruction->previous;
  }
  instruction->previous->next = instruction->next;
  instruction->next->previous = instruction->previous;
  instruction->previous = NULL;
  instruction->next = NULL;
  return instruction;
}

BrainfuckInstruction *brainfuck_add(BrainfuckState *state,
                                    BrainfuckInstruction *instruction) {
  if (state == NULL || instruction == NULL) {
    return NULL;
  }
  instruction->previous = state->head;
  if (state->head != NULL) {
    state->head->next = instruction;
  }
  BrainfuckInstruction *iter = instruction;
  while (iter != NULL) {
    if (iter->next != NULL) {
      state->head = iter;
      break;
    }
    iter = iter->next;
  }
  if (state->root == NULL) {
    state->root = instruction;
  }
  return state->head;
}

BrainfuckInstruction *brainfuck_add_first(BrainfuckState *state,
                                          BrainfuckInstruction *instruction) {
  if (state == NULL || instruction == NULL) {
    return NULL;
  }
  instruction->previous = NULL;
  BrainfuckInstruction *iter = instruction;
  while (iter != NULL) {
    if (iter->next == NULL) {
      state->head = iter;
      break;
    }
    iter = iter->next;
  }
  iter->next = state->root;
  state->root->previous = iter;
  state->root = instruction;
  return state->head;
}

BrainfuckInstruction *brainfuck_parse_stream(FILE *stream) {
  return brainfuck_parse_stream_until(stream, EOF);
}

BrainfuckInstruction *brainfuck_parse_stream_until(FILE *stream,
                                                   const int until) {
  BrainfuckInstruction *instruction = brainfuck_instruction();
  BrainfuckInstruction *root = instruction;
  char ch;
  char temp;
  while ((ch = fgetc(stream)) != until) {
    if (ch == EOF || feof(stream)) {
      break;
    }
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
    instruction->next = brainfuck_instruction();
    instruction = instruction->next;
  }
  instruction->type = BRAINFUCK_TOKEN_LOOP_END;
  instruction->difference = 1;
  return root;
}

void brainfuck_destroy_instruction(BrainfuckInstruction *instruction) {
  if (instruction == NULL) {
    return;
  }
  free(instruction);
  instruction = NULL;
}

void brainfuck_destroy_instructions(BrainfuckInstruction *root) {
  BrainfuckInstruction *temp;
  while (root != NULL) {
    temp = root;
    brainfuck_destroy_instructions(root->loop);
    root = root->next;
    brainfuck_destroy_instruction(temp);
  }
}

void brainfuck_destroy_state(BrainfuckState *state) {
  if (state == NULL) {
    return;
  }
  brainfuck_destroy_instructions(state->root);
  state->root = NULL;
  state->head = NULL;
  free(state);
  state = NULL;
}

void brainfuck_destroy_context(BrainfuckExecutionContext *context) {
  if (context->tape != NULL) {
    free(context->tape);
  }
  if (context != NULL) {
    free(context);
  }
  context = NULL;
}

void brainfuck_execute(BrainfuckInstruction *root,
                       BrainfuckExecutionContext *context) {
  if (root == NULL || context == NULL) {
    return;
  }
  BrainfuckInstruction *instruction = root;
  int index;
  while (instruction != NULL && instruction->type != BRAINFUCK_TOKEN_LOOP_END) {
    switch (instruction->type) {
    case BRAINFUCK_TOKEN_PLUS:
      context->tape[context->tape_index] += instruction->difference;
      break;
    case BRAINFUCK_TOKEN_MINUS:
      context->tape[context->tape_index] -= instruction->difference;
      break;
    case BRAINFUCK_TOKEN_NEXT:
      if (instruction->difference >= INT_MAX - (long)context->tape_size ||
          (long)context->tape_index + instruction->difference >=
              (long)context->tape_size) {
        fprintf(stderr, "error: tape memory out of bounds");
        exit(EXIT_FAILURE);
      }
      context->tape_index += instruction->difference;
      break;
    case BRAINFUCK_TOKEN_PREVIOUS:
      if (instruction->difference >= INT_MAX - (long)context->tape_size ||
          (long)context->tape_index - instruction->difference < 0) {
        fprintf(stderr, "error: tape memory out of bounds");
        exit(EXIT_FAILURE);
      }
      context->tape_index -= instruction->difference;
      break;
    case BRAINFUCK_TOKEN_OUTPUT:
      for (index = 0; index < instruction->difference; index++) {
        context->output_handler(context->tape[context->tape_index]);
      }
      break;
    case BRAINFUCK_TOKEN_INPUT:
      for (index = 0; index < instruction->difference; index++) {
        char input = context->input_handler();
        if (input == EOF) {
          if (BRAINFUCK_EOF_BEHAVIOUR != 1) {
            context->tape[context->tape_index] = BRAINFUCK_EOF_BEHAVIOUR;
          }
        } else {
          context->tape[context->tape_index] = input;
        }
      }
      break;
    case BRAINFUCK_TOKEN_LOOP_START:
      while (context->tape[context->tape_index]) {
        brainfuck_execute(instruction->loop, context);
      }
      break;
    default:
      return;
    }
    instruction = instruction->next;
    if (context->shouldStop == 1) {
      instruction = NULL;
      return;
    }
  }
}

char brainfuck_getchar() {
  char ch, t;
  ch = getchar();
  while ((t = getchar()) != '\n' && t != EOF) {
  }
  return ch;
}
