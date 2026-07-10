#define STF_IMPL_MOD
#include "../src/stf.h"

#define STRINGVIEW_IMPL
#include "../src/stringview.h"

#include <inttypes.h>
#include <stdlib.h>

const char *test_string = "Hello StringViews";

bool build_stringview_from_cstr(void)
{
  StringView sv = stringview_from_cstr((char *)test_string);

  printf("StringView = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(sv));

  return ((sv.data) && (sv.size > 0));
}

bool build_stringview_from_cstr_sub(void)
{
  // Build a string wich size clamps to the end of c string
  StringView sv = stringview_from_cstr_sub((char*)test_string, 6, 100);

  // Build a string wich size is 5
  StringView hello = stringview_from_cstr_sub((char *)test_string, 0, 5);

  // Build a string wich points at the end
  StringView nothing = stringview_from_cstr_sub((char *)test_string, 100, 100);

  printf("StringViews = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(sv));
  printf("Hello = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(hello));
  printf("Nothing = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(nothing));
  
  return 
    ((sv.data) && (sv.size > 0)) &&
    ((hello.data) && (hello.size == 5)) &&
    ((*nothing.data == 0) && (nothing.size == 0));
}

bool build_stringview_from_sub(void)
{
  StringView sv = stringview_from_cstr_sub((char *)test_string, 6, 100); // "StringView"
  StringView s = stringview_from_sub(&sv, 0, 6);

  printf("StringViews = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(sv));
  printf("String = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(s));

  return
    ((*(sv.data + sv.size - 1) == 's') && (sv.size == 11)) &&
    ((*s.data == 'S') && (s.size == 6));
}

bool split_stringview_by_offset(void)
{
  StringView base = stringview_from_cstr_sub((char *)test_string, 6, 11); // "StringViews"
  StringView left = base;
  StringView right = stringview_split_offset(&left, 6); // left: "String", right: "Views"
  
  printf("base = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(base));
  printf("left = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(left));
  printf("right = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(right));

  return
    ((*base.data == 'S') && (*(base.data + base.size - 1) == 's') && (base.size == 11)) &&
    ((*left.data == 'S') && (*(left.data + left.size - 1) == 'g') && (left.size == 6)) &&
    ((*right.data == 'V') && (*(right.data + right.size - 1) == 's') && (right.size == 5));
}

bool split_stringview_by_delim(void)
{
  StringView base = stringview_from_cstr((char *)test_string); // "Hello StringViews"
  StringView left = base;
  StringView right = stringview_split_delim(&left, ' '); // left: "Hello", right: "StringViews"
  
  printf("base = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(base));
  printf("left = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(left));
  printf("right = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(right));

  return
    ((*base.data == 'H') && (*(base.data + base.size - 1) == 's') && (base.size == 17)) &&
    ((*left.data == 'H') && (*(left.data + left.size - 1) == 'o') && (left.size == 5)) &&
    ((*right.data == 'S') && (*(right.data + right.size - 1) == 's') && (right.size == 11));
}

bool advance_stringview_from_left(void)
{
  StringView string = stringview_from_cstr("foo");

  printf("base = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(string));
  stringview_next(&string);

  return (*string.data == 'o') && (string.size == 2);
}

bool getchar_stringview_from_left(void)
{
  StringView string = stringview_from_cstr("foo");
  char expected_c = 'f';

  printf("base = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(string));
  printf("expected = %c\n", expected_c);

  char c = stringview_getc(&string);

  return 
    ((*string.data == expected_c) && (string.size == 3)) && 
    (c == 'f');
}

bool convert_stringview_to_number(void)
{
  StringView i64_string = stringview_from_cstr("-34");
  StringView u64_string = stringview_from_cstr("+35");
  StringView invalid_string = stringview_from_cstr("aaa");
  StringView invalid_sign_string = stringview_from_cstr("-1");

  int64_t i64 = stringview_to_i64(&i64_string);
  uint64_t u64 = stringview_to_u64(&u64_string);
  int64_t invalid_i64 = stringview_to_i64(&invalid_string);
  uint64_t invalid_u64 = stringview_to_u64(&invalid_string);
  uint64_t invalid_sign_u64 = stringview_to_u64(&invalid_sign_string);

  int64_t expected_i64 = -34;
  int64_t expected_invalid = 0;
  uint64_t expected_u64 = 35;

  printf("input:\n");
  printf("i64 = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(i64_string));
  printf("u64 = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(u64_string));
  printf("invalid (non digit) = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(invalid_string));
  printf("invalid (negative u64) = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(invalid_sign_string));
  printf("output:\n");
  printf("expected i64 = %"PRIi64"\n", expected_i64);
  printf("expected u64 = %"PRIu64"\n", expected_u64);
  printf("expected invalid (any) = %"PRIi64"\n", expected_invalid);

  return
    ((i64 == expected_i64) && (u64 == expected_u64)) &&
    ((invalid_i64 == expected_invalid) && (invalid_u64 == expected_invalid)) &&
    (invalid_sign_u64 == expected_invalid);
}

bool convert_stringview_to_cstr(void)
{
  const char *source = "Hello World";
  size_t size = strlen(source);
  
  StringView sv = stringview_from_cstr((char *)source);
  char *to = stringview_to_cstr(&sv);
  size_t to_size = strlen(to);

  int memcmp_res = memcmp(source, to, size);
  free(to);
  
  return (memcmp_res == 0) && (size == to_size);
}

bool copy_stringview_into_buffer(void)
{
  StringView sv = stringview_from_cstr("Hello World!");
  char buff[16] = {0};
  bool copy_invalid_size = stringview_into_buff(&sv, buff, 8);
  bool copy_valid_size = stringview_into_buff(&sv, buff, 16);
  int memcmp_res = memcmp(sv.data, buff, sv.size);

  printf("base = "STRINGVIEW_FMT"\n", STRINGVIEW_ARG(sv));
  printf("copy = %s\n", buff);

  return 
    (copy_invalid_size == false) &&
    (copy_valid_size == true) &&
    (memcmp_res == 0);
}

stf_Test stringview_module_tests[] =
{
  {"from_cstr", "Build from C string", build_stringview_from_cstr},
  {"from_cstr_sub", "Build from C string slice", build_stringview_from_cstr_sub},
  {"from_sub", "Build from stringview slice", build_stringview_from_sub},
  {"split_offset", "Split at offset", split_stringview_by_offset},
  {"split_delim", "Split by delimiter", split_stringview_by_delim},
  {"advance_left", "Advance from left", advance_stringview_from_left},
  {"getchar_left", "Read a character from the left", getchar_stringview_from_left},
  {"to_number", "Convert to number", convert_stringview_to_number},
  {"to_cstr", "Convert to C string", convert_stringview_to_cstr},
  {"copy_to_buffer", "Copy into buffer", copy_stringview_into_buffer},
};

STF_MODULE_EXPORTS(stringview_module_tests);