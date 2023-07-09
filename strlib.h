#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
    Simple header only string library for C
    NOTE: all functions returning a String allocate memory and requires a corresponding call to String_Delete
*/

// TODO: StringBuilder
// TODO: SSO

// Doesn't need to be free'd
// NOTE: Calling String_CStr on it is invalid
#define str(x) (&((const String){.len = sizeof((x)) - 1, .buf = (x)}))

#define STRING_OVERLOAD(arg0, arg1, arg2, ...) arg2

/* clang-format off */
#define String_1(x) \
    _Generic((x), \
        const String* : String_Copy, \
        String* : String_Copy, \
        const char* : String_FromCString, \
        char* : String_FromCString, \
        default: String_New \
    )(x)
/* clang-format on */

// Generic constructor for String type
// String(String* x)           -> Copy of x
// String(char* x)             -> String from C string (null-terminated)
// String(char* x, size_t len) -> String from C array of length = len
// String(size_t len)          -> Uninitialized String of length = len
#define String(...) STRING_OVERLOAD(__VA_ARGS__, String_FromCharArray, String_1)(__VA_ARGS__)

typedef struct {
    size_t len;
    char*  buf;
} String;

typedef struct {
    size_t  len;
    String* str;
} StringList;

// Frees a String
static inline void String_Delete(String* str)
{
    free(str->buf);
}

// Frees a StringList
static inline void StringList_Delete(StringList* str_list)
{
    free(str_list->str);
}

// Allocates a String of some length `len`
static inline String String_New(size_t len)
{
    char* buf = calloc(1, len + 1);
    assert(buf);

    return (String){.len = len, .buf = buf};
}

// Creates a String from a C-string (null terminated char array)
static inline String String_FromCString(const char* str)
{
    size_t len = strlen(str);
    String ret = String_New(len);
    memcpy(ret.buf, str, len);

    return ret;
}

// Creates a String from an array of characters of some length
static inline String String_FromCharArray(const char* arr, size_t len)
{
    String ret = String_New(len);
    memcpy(ret.buf, arr, len);

    return ret;
}

// Makes a copy of `str`
static inline String String_Copy(const String* str)
{
    String ret = String_New(str->len);
    memcpy(ret.buf, str->buf, str->len);

    return ret;
}

// Concatenates `left` and `right` in order
static inline String String_Join(const String* left, const String* right)
{
    String cat = String_New(left->len + right->len);
    memcpy(cat.buf, left->buf, left->len);
    memcpy(cat.buf + left->len, right->buf, right->len);

    return cat;
}

// Finds the index of the first occurrence of `substr` in `str`
// returns a negative value if no occurrence exists
static inline ssize_t String_FirstOccurrenceOf(const String* str, const String* substr)
{
    if (substr->len > str->len) {
        return -1;
    }

    for (size_t ii = 0; ii < str->len - substr->len + 1; ii++) {
        bool found = !memcmp(&str->buf[ii], substr->buf, substr->len);
        if (found) {
            return (ssize_t)ii;
        }
    }

    return -1;
}

// Finds the index of the last occurrence of `substr` in `str`
// returns a negative value if no occurrence exists
static inline ssize_t String_LastOccurrenceOf(const String* str, const String* substr)
{
    if (substr->len > str->len) {
        return -1;
    }

    for (ssize_t ii = str->len - substr->len; ii >= 0; ii--) {
        bool found = !memcmp(&str->buf[ii], substr->buf, substr->len);
        if (found) {
            return (ssize_t)ii;
        }
    }

    return -1;
}

// Determines if `str` begins with the String `prefix`
static inline bool String_StartsWith(const String* str, const String* prefix)
{
    if (str->len < prefix->len) {
        return false;
    }

    return !memcmp(str->buf, prefix->buf, prefix->len);
}

// Determines if `str` begins with the String `suffix`
static inline bool String_EndsWith(const String* str, const String* suffix)
{
    if (str->len < suffix->len) {
        return false;
    }

    return !memcmp(&str->buf[str->len - suffix->len], suffix->buf, suffix->len);
}

// Determines if `str` contains `substr`
static inline bool String_Contains(const String* str, const String* substr)
{
    return String_FirstOccurrenceOf(str, substr) >= 0;
}

// returns the lexicographic order of str_a and str_b
// if negative, str_a would come before str_b
// if positive, str_b would come before str_a
// if zero the strings are identical
static inline ssize_t String_Compare(const String* str_a, const String* str_b)
{
    size_t min_len = str_a->len < str_b->len ? str_a->len : str_b->len;

    int cmp = memcmp(str_a->buf, str_b->buf, min_len);
    if (cmp == 0) {
        return (ssize_t)str_a->len - (ssize_t)str_b->len;
    } else {
        return (ssize_t)cmp;
    }
}

// Determines if `str_a` is identical to `str_b`
static inline ssize_t String_Equal(const String* str_a, const String* str_b)
{
    if (str_a->len != str_b->len) {
        return false;
    }

    return !memcmp(str_a->buf, str_b->buf, str_a->len);
}

// Determines if a char is whitespace
static inline bool String_IsWhitespaceChar(char c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\v';
}

// Removes whitespace from the beginning and end of `str`
static inline String String_Trim(const String* str)
{
    size_t front = 0;
    size_t back  = 0;

    for (ssize_t ii = 0; ii < (ssize_t)str->len && String_IsWhitespaceChar(str->buf[ii]); ii++) {
        front += 1;
    }

    for (ssize_t ii = (ssize_t)str->len - 1; ii >= (ssize_t)front && String_IsWhitespaceChar(str->buf[ii]); ii--) {
        back += 1;
    }

    size_t trim_amt = front + back;
    String ret      = String_New(str->len - trim_amt);
    memcpy(ret.buf, &str->buf[front], str->len - front - back);

    return ret;
}

// Returns the number of distinct (non-overlapping) instances of `substr` in `str`
// e.g. String_DistinctInstancesOf("aaaa", "aaa") == 1
static inline size_t String_DistinctInstancesOf(const String* str, const String* substr)
{
    if (str->len < substr->len) {
        return 0;
    }

    size_t instances = 0;
    for (size_t ii = 0; ii < str->len - substr->len + 1;) {
        bool found = !memcmp(&str->buf[ii], substr->buf, substr->len);
        if (found) {
            instances += 1;
            ii += substr->len ? substr->len : 1;
        } else {
            ii += 1;
        }
    }

    return instances;
}

// Returns the number of instances of `substr` in `str`
// e.g. String_InstancesOf("aaaa", "aaa") == 2
static inline size_t String_InstancesOf(const String* str, const String* substr)
{
    if (str->len < substr->len) {
        return 0;
    }

    size_t instances = 0;
    for (size_t ii = 0; ii < str->len - substr->len + 1; ii++) {
        bool found = !memcmp(&str->buf[ii], substr->buf, substr->len);
        if (found) {
            instances += 1;
        }
    }

    return instances;
}

// Replaces all instances of `old` with `new` in `str`
// NOTE: Calling String_Replace with old == "" produces the original string
static inline String String_Replace(const String* str, const String* old, const String* new)
{
    if (str->len < old->len || old->len == 0) {
        return String_Copy(str);
    }

    size_t  old_count = String_DistinctInstancesOf(str, old);
    ssize_t ret_len   = (ssize_t)str->len + (ssize_t)old_count * ((ssize_t) new->len - (ssize_t)old->len);
    String  ret       = String_New(ret_len);

    size_t ret_pos = 0;
    size_t str_pos = 0;
    for (str_pos = 0; str_pos < str->len - old->len + 1;) {
        bool found = !memcmp(&str->buf[str_pos], old->buf, old->len);
        if (found) {
            memcpy(&ret.buf[ret_pos], new->buf, new->len);
            ret_pos += new->len;
            str_pos += old->len;
        } else {
            ret.buf[ret_pos] = str->buf[str_pos];
            ret_pos += 1;
            str_pos += 1;
        }
    }

    if (str_pos != str->len) {
        memcpy(&ret.buf[ret_pos], &str->buf[str_pos], str->len - str_pos);
    }

    return ret;
}

// Returns a StringList which contains all the substrings formed by splitting `str` wherever `delim` occurs
// NOTE: Calling String_Split with delim == "" is invalid
static inline StringList String_Split(const String* str, const String* delim)
{
    assert(delim->len > 0);

    size_t delim_count     = String_DistinctInstancesOf(str, delim);
    size_t substr_count    = delim_count + 1;
    size_t char_count      = str->len - delim_count * delim->len;
    size_t char_buf_size   = char_count + substr_count; // need 1 extra byte per substring for null terminator insertion
    size_t str_header_size = sizeof(String) * substr_count;

    char*   mem  = calloc(1, char_buf_size + str_header_size);
    String* strs = (String*)mem;
    char*   data = mem + str_header_size;

    *strs = (String){.len = 0, .buf = data};
    size_t str_pos;
    for (str_pos = 0; str_pos < str->len - delim->len + 1;) {
        bool found = !memcmp(&str->buf[str_pos], delim->buf, delim->len);
        if (found) {
            strs += 1;
            data += 1;
            *strs = (String){.len = 0, .buf = data};
            str_pos += delim->len;
        } else {
            *data = str->buf[str_pos];
            data += 1;
            strs->len += 1;
            str_pos += 1;
        }
    }

    if (str_pos != str->len) {
        memcpy(data, &str->buf[str_pos], str->len - str_pos);
        strs->len += str->len - str_pos;
    }

    return (StringList){
        .len = substr_count,
        .str = (String*)mem,
    };
}

// Returns a C-style string given a String
// NOTE: Does not allocate memory, lifetime of the return value is tied to that of the supplied String
// intended to be used to interface with C-style APIs efficiently
static inline const char* String_CStr(String* str)
{
    str->buf[str->len] = '\0';
    return str->buf;
}
