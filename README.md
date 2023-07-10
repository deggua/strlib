# strlib
Header only string library for C11

## Features
* Length tracked strings
* Strings are immuteable, each function on strings may only allocate once for the return value
* Most common string operations are provided
* Simple, no-allocation conversion from a length tracked string to a null-terminated string
* Easy to use interface

## Macros
`str(x)` produces a no-allocation `String` 'literal' from a C-string literal

Example:
```c
String_Equal(str("foo"), str("bar")) == false
```

`String(...)` acts as an overload for `String_FromCString`, `String_FromCharArray`, `String_New`, and `String_Copy`

Example:
```c
String x = String("c-string literal");       // String_FromCString
String y = String(str("String literal"));    // String_Copy
String z = String((char){'a', 'b', 'c'}, 3); // String_FromCharArray
String w = String(10);                       // String_New
```
Note that to create a string with `\0` in it, it should be initialized like this:
```c
String str = String(str("String with \0 inside"));
// str = "String with \0 inside"
```
Doing the following won't work because the macro can't distinguish between a string literal and a `const char*`
```c
String str = String("String with \0 inside");
// str = "String with "
```

`STRING_FMT` and `STRING_ARG(str)` can be used with `printf` to format `String` prints. Note that this doesn't handle the case where there are `\0` chars inside the string.

Example:
```c
printf("My string = " STRING_FMT "\n", STRING_ARG(&my_string));
```

## Functions
|Function|Description|
|--------|-----------|
|`String String_New(size_t len)`| Allocates an uninitialized `String` of length `len`|
|`String String_FromCString(const char* str)` | Creates a `String` from a null-terminated C-string `str`|
|`String String_FromCharArray(const char* str, size_t len)` | Creates a `String` from a C-array of length `len` |
| `String String_Copy(const String* str)` | Creates a copy of `str` |
| `String String_Join(const String* left, const String* right)` | Concatenates `left` and `right` in order
| `ssize_t String_FirstOccurrenceOf(const String* str, const String* substr)` | Returns the index of the first occurrence of `substr` in `str`, negative if `substr` was not found |
| `ssize_t String_LastOccurrenceOf(const String* str, const String* substr)` | Returns the index of the last occurrence of `substr` in `str`, negative if `substr` was not found |
| `bool String_StartsWith(const String* str, const String* prefix)` | Returns `true` if `str` begins with `prefix`, otherwise `false` |
| `bool String_EndsWith(const String* str, const String* suffix)` | Returns `true` if `str` ends with `suffix`, otherwise `false` |
| `bool String_Contains(const String* str, const String* substr)` | Returns `true` if `substr` exists in `str`, otherwise `false` |
| `ssize_t String_Compare(const String* str_a, const String* str_b)` | Returns `0` if the strings are identical, `<0` if `str_a` comes before `str_b` if sorted in lexicographic order, `>0` if `str_b` comes before `str_a` if sorted in lexicographic order |
| `bool String_Equal(const String* str_a, const String* str_b)` | Returns `true` if `str_a` is identical to `str_b`, otherwise `false` |
| `String String_Trim(const String* str)` | Removes whitespace from the beginning and end of `str` |
| `size_t String_DistinctInstancesOf(const String* str, const String* substr)` | Returns the number of distinct (non-overlapping) instances of `substr` in `str` |
| `size_t String_InstancesOf(const String* str, const String* substr)` | Returns the number of instances of `substr` in `str` |
| `String String_Replace(const String* str, const String* old, const String* new)` | Replaces (from the left) every distinct instance of `old` in `str` with `new` |
| `StringList String_Split(const String* str, const String* delim)` | Returns a `StringList` containing an array of `String` substrings which were separated by `delim` in `str` |
| `String String_Slice(const String* str, size_t start, size_t end)` | Returns a `String` slice from `str` that starts from index `start` up to `end` |
| `String String_Write(const String* str, FILE* fd)` | Write `str` to a `FILE*` `fd` |
| `String String_Print(const String* str)` | Print `str` to `stdout`, handles printing strings with `\0` in them |
| `const char* String_CStr(String* str)` | Returns a null-terminated C-string from a `String` |
| `void String_Delete(String* str)` | Frees a `String` |
| `void StringList_Delete(String* str_list)` | Frees a `StringList` |

## Tests
`test.c` has some (currently 164) tests that verify functional correctness, I recommend you compile with `clang test.c -fsanitize=address` to verify memory correctness as well.

## Performance
Most functions are O(n), worst case for some functions is O(n<sup>2</sup>). All funcitons perform, at most, a single memory allocation (if they return a `String`). `String_CStr` does not allocate memory, it just places a null-terminator in the `String` argument's buffer, therefore its lifetime is tied to the associated `String`.

## TODO
* Configureable length type (e.g. using `uint32_t` instead of `size_t` for lower overhead)
* SSO (small-string optimization)
* `StringBuilder` for more efficient string construction
* C99 support
