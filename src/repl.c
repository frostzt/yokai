#include <stdio.h>
#include <string.h>

#include "yokai/lexer.h"
#include "yokai/repl.h"
#include "yokai/str.h"

static const char *PROMPT = ">> ";
static const char *WELCOME = "\n"
                             "   ██    ██  ██████  ██   ██   ████   ██████\n"
                             "    ██  ██   ██  ██  ██  ██   ██  ██    ██\n"
                             "     ████    ██  ██  █████    ██████    ██\n"
                             "      ██     ██  ██  ██  ██   ██  ██    ██\n"
                             "      ██     ██████  ██   ██  ██  ██  ██████\n"
                             "\n"
                             "                【 妖 怪 】\n"
                             "\n"
                             "         ~ Welcome to Yōkai REPL! ~\n"
                             "           Type your code below\n"
                             "\n";
void repl_start(FILE *in, FILE *out) {
  fputs(WELCOME, out);
  fputs(PROMPT, out);
  fflush(out);

  char line[4096];
  while (fgets(line, 4096, in) != NULL) {
    size_t len = strlen(line);

    // remove the trailing chars
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
      line[--len] = '\0';
    }

    // handle empty lines
    if (len == 0) {
      fputs(PROMPT, out);
      fflush(out);
      continue;
    }

    // construct a strview
    StrView input = {.data = line, .len = len};
    Lexer lexer = {.input = input, .position = 0, .read_position = 0, .ch = 0};
    read_char(&lexer);

    Token token = next_token(&lexer);

    while (token.type != TOK_EOF) {
      fprintf(out, "Type:%s Literal:%.*s\n", token_type_name(token.type), (int)token.literal.len,
              token.literal.data);
      token = next_token(&lexer);
    }

    fputs(PROMPT, out);
    fflush(out);
  }

  fprintf(out, "\n\nBye bye...\n");
  fflush(out);
}
