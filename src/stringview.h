
/*
  -- STRINGVIEW_DEF --
  Storage class specifier
  extern when including header, and none when including implementation.
*/
#ifdef STRINGVIEW_IMPL
// Implementation include
#define STRINGVIEW_DEF 
#else
// Header include
#define STRINGVIEW_DEF extern
#endif //!STRINGVIEW_IMPL

#ifndef STRINGVIEW_H
#define STRINGVIEW_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
  -- StringView --
  C++ like string views for looking into raw c strings.
*/

/* 
  -- StringView Struct/Type --
  @param data: Pointer to raw c string
  @param size: Length of the string view  
*/
typedef struct StringView StringView;

struct StringView
{
  const char *data;
  size_t size; 
};

/*
  -- Helper macro for formatting string views into printf --
*/
#define STRINGVIEW_FMT "%.*s"
/*
  -- Helper macro for string views arguments into printf --
*/
#define STRINGVIEW_ARG(sv) (int)((sv).size), (sv).data

/*
  -- Build a string view from a c string --
  @param cstr: Pointer to a raw c string.
  @return `StringView`: Wich length is
  calculated until a null terminator.
*/
STRINGVIEW_DEF StringView stringview_from_cstr(const char *cstr);

/*
  -- Build a string view from a c string region --
  @param cstr: Pointer to a raw c string.
  @param start: Offset of the cstr where 
  the view starts.
  @param end: Offset from the start where
  the view ends.
  @return `StringView`: Wich length is `end`
  (or clamped at null terminator - start)
*/
STRINGVIEW_DEF StringView stringview_from_cstr_sub(const char *cstr, size_t start, size_t end);

/*
  -- Build a string view from another string view region --
  @param sv: Pointer to a string view.
  @param start: Offset of the string view where 
  the view starts.
  @param end: Offset from the start where
  the view ends.
  @return `StringView`: Wich length is `end`
  (or clamped at `sv` size - start)
*/
STRINGVIEW_DEF StringView stringview_from_sub(const StringView *sv, size_t start, size_t end);

/*
  -- Splits a string view at the specified offset --
  @param sv: Pointer to base string view.
  @param offset: Offset where the string view 
  gets splitted.
  @return `StringView`: that contains the right part
  that was chopped, sv will contain left side. 
*/
STRINGVIEW_DEF StringView stringview_split_offset(StringView *sv, size_t offset);

/*
  -- Splits a string view at first ocurrence of the specified delim --
  @param sv: Pointer to base string view.
  @param delim: Delimitier to split at.
  @return `StringView`: that contains the right part
  that was chopped, sv will contain left side. consumes delimitier
*/
STRINGVIEW_DEF StringView stringview_split_delim(StringView *sv, char delim);

/*
  -- Advances input string view by one from the left side --
  @param sv: Pointer to base string view.
*/
STRINGVIEW_DEF void stringview_next(StringView *sv);

/*
  -- Get the current character from the left side --
  @param sv: Pointer to base string view.
  @return 'char': The character from the left side
*/
STRINGVIEW_DEF char stringview_getc(const StringView *sv);

/*
  -- Convert string view contents to a signed 64 bit number --
  @param sv: Pointer to base string view.
  @return 'int64_t': The number contained on the string
  @note: Stops at the first non digit character, 
  supports the following signs: + - before the digits.
*/
STRINGVIEW_DEF int64_t stringview_to_i64(const StringView *sv);

/*
  -- Convert string view contents to an unsigned 64 bit number --
  @param sv: Pointer to base string view.
  @return 'uint64_t': The number contained on the string
  @note: Stops at the first non digit character,
  and only supports + as a valid sign.
*/
STRINGVIEW_DEF uint64_t stringview_to_u64(const StringView *sv);

/*
  -- Convert string view to a C string --
  @param sv: Pointer to base string view.
  @return 'char *': Heap allocated C string
  @note: User must manually free once it's done.
*/
STRINGVIEW_DEF char *stringview_to_cstr(const StringView *sv);

/*
  -- Copy string view into a buffer and append null terminator --
  @param sv: Pointer to input string view
  @param buff: Output buffer
  @param size: Maximum size of the output buffer
  @return 'true' on when data is copied, 
  'false' when no data is copied
*/
STRINGVIEW_DEF bool stringview_into_buff(const StringView *sv, char buff[], size_t size);

#endif //!STRINGVIEW_H

#ifdef STRINGVIEW_IMPL

/*
  -- STRINGVIEW_MALLOC --
  Macro that allows overrinding the default malloc function (malloc).
  The signature is STRINGVIEW_MALLOC(size)
*/
#ifndef STRINGVIEW_MALLOC
#include <stdlib.h>
#define STRINGVIEW_MALLOC(size) malloc(size)
#endif //!STRINGVIEW_MALLOC

/*
  -- STRINGVIEW_MEMCPY --
  Macro that allows overrinding the default memcpy function (memcpy).
  The signature is STRINGVIEW_MEMCPY(dest, source, size)
*/
#ifndef STRINGVIEW_MEMCPY
#include <string.h>
#define STRINGVIEW_MEMCPY(dest, source, size) memcpy(dest, source, size)
#endif //!STRINGVIEW_MEMCPY

#include <string.h>

STRINGVIEW_DEF StringView stringview_from_cstr(const char *cstr)
{
  StringView sv = {0, 0};
  sv.data = cstr;
  sv.size = strlen(cstr);
  return sv;
}

STRINGVIEW_DEF StringView stringview_from_cstr_sub(const char *cstr, size_t start, size_t end)
{
  StringView sv = {0, 0};
  size_t len = strlen(cstr);

  if (start >= len) {
    sv.data = cstr + len;
    sv.size = 0;
    return sv;
  }

  sv.data = cstr + start;
  sv.size = (end > len - start) ? len - start : end;

  return sv;
}

STRINGVIEW_DEF StringView stringview_from_sub(const StringView *sv, size_t start, size_t end)
{
  StringView sub_sv = {0, 0};

  if (start >= sv->size) {
    sub_sv.data = sv->data + sv->size;
    sub_sv.size = 0;
    return sub_sv;
  }

  sub_sv.data = sv->data + start;
  sub_sv.size = (end > sv->size - start) ? sv->size - start : end;

  return sub_sv;
}

STRINGVIEW_DEF StringView stringview_split_offset(StringView *sv, size_t offset)
{
  StringView right = {0, 0};

  if (offset > sv->size) {
    right.data = sv->data + sv->size;
    right.size = 0;
    return right;
  }

  right.data = sv->data + offset;
  right.size = sv->size - offset;

  sv->size = offset;

  return right;
}

STRINGVIEW_DEF StringView stringview_split_delim(StringView *sv, char delim)
{
  StringView right = {0, 0};

  size_t offset = 0;
  while (offset < sv->size && sv->data[offset] != delim)
    offset += 1;

  if (offset > sv->size) {
    right.data = sv->data + sv->size;
    right.size = 0;
    return right;
  }

  right.data = sv->data + offset + 1;
  right.size = sv->size - (offset + 1);

  sv->size = offset;

  return right;
}

STRINGVIEW_DEF void stringview_next(StringView *sv)
{
  if ((sv->size - 1) <= 0) return;

  sv->size -= 1;
  sv->data += 1;
}

STRINGVIEW_DEF char stringview_getc(const StringView *sv)
{
  if (sv->size <= 0) return '\0';
  return *sv->data;
}

STRINGVIEW_DEF int64_t stringview_to_i64(const StringView *sv)
{
  if (sv->size <= 0) return 0;

  StringView copy = *sv;
  int64_t result = 0;
  int64_t sign = 1;
  
  // Handle first character as sign
  char sign_char = *copy.data;
  if (sign_char == '+' || sign_char == '-') {
    if (sign_char == '-') sign = -1;
    // no check for '+' since default sign 
    // is positive, just consume the sign.
    copy.data += 1;
    copy.size -= 1;
  }
  
  while (copy.size > 0 && *copy.data >= '0' && *copy.data <= '9') 
  {
    result = (result * 10) + (*(copy.data) - '0');
    copy.data += 1;
    copy.size -= 1;
  }

  return result * sign;
}

STRINGVIEW_DEF uint64_t stringview_to_u64(const StringView *sv)
{
  if (sv->size <= 0) return 0;

  StringView copy = *sv;
  int64_t result = 0;
  
  // Handle first character as sign (consume only)
  if (*copy.data == '+') {
    // no check for '-' since isn't allowed
    // when parsing unsigned numbers.
    copy.data += 1;
    copy.size -= 1;
  }
  
  while (copy.size > 0 && *copy.data >= '0' && *copy.data <= '9') 
  {
    result = (result * 10) + (*(copy.data) - '0');
    copy.data += 1;
    copy.size -= 1;
  }

  return result;
}

STRINGVIEW_DEF char *stringview_to_cstr(const StringView *sv)
{
  if (sv->size <= 0) return NULL;

  char *cstr = STRINGVIEW_MALLOC(sv->size + 1);
  if (cstr == NULL) return NULL;

  STRINGVIEW_MEMCPY(cstr, sv->data, sv->size);
  cstr[sv->size] = '\0';

  return cstr;
}

STRINGVIEW_DEF bool stringview_into_buff(const StringView *sv, char buff[], size_t size)
{
  if (sv->size <= 0 || sv->size >= size) return false;
  STRINGVIEW_MEMCPY(buff, sv->data, sv->size);
  buff[sv->size] = '\0';
  
  return true;
}

#endif //!STRINGVIEW_IMPL