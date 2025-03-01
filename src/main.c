#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <brainfuck.h>
#include <getopt.h>

void print_usage(char *name) {
  fprintf(stdout, "Usage: %s -[evh] [file...]\n", name);
  fprintf(stdout, "\t-e --eval\t\trun code directly\n");
  fprintf(stdout, "\t-v --version\t\tshow the version information\n");
  fprintf(stdout, "\t-h --help\t\tshow a help message\n");
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
  brainfuck_execute(state->root, context);
  brainfuck_destroy_context(context);
  brainfuck_destroy_state(state);
  fclose(file);
  return EXIT_SUCCESS;
}

// int run_string(char *code) {
//   BrainfuckState *state = brainfuck_state();
//   BrainfuckExecutionContext *context =
//   brainfuck_context(BRAINFUCK_TAPE_SIZE); BrainfuckInstruction *instruction =
//   brainfuck_parse_string(code); brainfuck_add(state, instruction);
//   brainfuck_execute(state->root, context);
//   brainfuck_destroy_context(context);
//   brainfuck_destroy_state(state);
//   return EXIT_SUCCESS;
// }

// static struct option long_option[] = {{"help", no_argument, 0, 'h'},
//                                       {"eval", required_argument, 0, 'e'},
//                                       {0, 0, 0, 0}};

int main(int argc, char **argv) {
  int i = 1;

  // if (run_file(fopen("./examples/helloworld.bf", "r")) == EXIT_FAILURE) {
  //   fprintf(stderr, "error: failed to read file.");
  //   return EXIT_FAILURE;
  // }
  if (argc > 1) {
    while (i < argc) {
      if (run_file(fopen(argv[i++], "r")) == EXIT_FAILURE) {
        fprintf(stderr, "error: failed to read file %s\n", argv[i - 1]);
      }
    }
  }
  return 0;
}
