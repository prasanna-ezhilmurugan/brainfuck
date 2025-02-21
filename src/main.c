#include <stdio.h>
#include <stdlib.h>

#include <brainfuck.h>

void print_usage(char *name) {
  fprintf(stderr, "Usage: %s -[evh] [file...]\n", name);
  fprintf(stderr, "\t-e --eval\t\trun code directly\n");
  fprintf(stderr, "\t-v --version\t\tshow the version information\n");
  fprintf(stderr, "\t-h --help\t\tshow a help message\n");
}

int run_file(FILE *file) {
  BrainfuckState *state = brainfuck_state();
  BrainfuckExecutionContext *context = brainfuck_context(BRAINFUCK_TAPE_SIZE);
  if (file == NULL) {
    brainfuck_destroy_context(context);
    brainfuck_destroy_state(state);
    return EXIT_FAILURE;
  }
  brainfuck_add(state, brainfuck_parse_stream(file));
  return EXIT_SUCCESS;
}

int main() {
  // printf("brainfuck");
  print_usage("brianfuck");
  return 0;
}
