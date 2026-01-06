#include <string.h>

#include "test.h"
#include "yokai/arena.h"
#include "yokai/strbuf.h"

TEST(strbuf_through_check) {
  Arena arena = arena_create(128);

  /* create a new string buffer */
  StrBuf str_buf;
  str_buf.arena = &arena;
  sb__init(&str_buf, 5);

  /* append a new c-string */
  sb__append(&str_buf, "let ", 4);

  /* append a new StrView */
  sb__append_strview(&str_buf, (StrView){"x", 1});

  /* append a new char */
  sb__putc(&str_buf, ' ');
  sb__putc(&str_buf, '=');

  /* append a final string */
  sb__append_cstr(&str_buf, " 5;");

  const char *output = sb__cstr(&str_buf);
  ASSERT(memcmp(output, "let x = 5;", 9) == 0, "output didn't match");
}
