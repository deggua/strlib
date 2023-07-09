#include <stdio.h>
#include <string.h>

#include "strlib.h"

#define FAILURE(msg) printf("FAILURE (%s:%d) :: " msg "\n", __FILE__, __LINE__)
#if 0
#    define SUCCESS(msg) printf("SUCCESS (%s:%d) :: " msg "\n", __FILE__, __LINE__)
#else
#    define SUCCESS(msg) \
        do {             \
        } while (0)
#endif
#define ASSERT(expr)             \
    do {                         \
        bool _status = (expr);   \
        if (!_status) {          \
            FAILURE(#expr);      \
            result->failed += 1; \
        } else {                 \
            SUCCESS(#expr);      \
            result->passed += 1; \
        }                        \
        result->total += 1;      \
    } while (0)

typedef struct {
    size_t total;
    size_t passed;
    size_t failed;
} TestResult;

void test_creation(TestResult* result)
{
    {
        String str = String("Hello World\n");

        ASSERT(strcmp(String_CStr(&str), "Hello World\n") == 0);
        ASSERT(str.len == strlen("Hello World\n"));

        String_Delete(&str);
    }

    {
        char   arr[] = "Hello World\n";
        String str   = String_FromCharArray(arr, sizeof(arr) - 1);

        ASSERT(strcmp(String_CStr(&str), "Hello World\n") == 0);
        ASSERT(str.len == strlen("Hello World\n"));

        String_Delete(&str);
    }

    {
        char   arr[] = "Hello World\n";
        String str   = String_New(sizeof(arr) - 1);
        memcpy(str.buf, arr, sizeof(arr) - 1);

        ASSERT(strcmp(String_CStr(&str), "Hello World\n") == 0);
        ASSERT(str.len == strlen("Hello World\n"));

        String_Delete(&str);
    }

    {
        String str1 = String("Hello World\n");
        String str2 = String_Copy(&str1);

        ASSERT(strcmp(String_CStr(&str1), String_CStr(&str2)) == 0);
        ASSERT(str1.len == str2.len);

        String_Delete(&str1);
        String_Delete(&str2);
    }

    {
        const String* str = str("Hello World\n");
        ASSERT(strcmp(str->buf, "Hello World\n") == 0);
        ASSERT(str->len == strlen("Hello World\n"));
    }

    {
        String x = String("abc");
        String y = String("abc", 2);
        String z = String(&x);
        String w = String(5);

        ASSERT(x.len == 3);
        ASSERT(String_Equal(&x, str("abc")));

        ASSERT(y.len == 2);
        ASSERT(String_Equal(&y, str("ab")));

        ASSERT(z.len == 3);
        ASSERT(String_Equal(&z, str("abc")));
        ASSERT(z.buf != x.buf);

        ASSERT(w.len == 5);
        ASSERT(String_Equal(&w, str("\0\0\0\0\0")));

        String_Delete(&x);
        String_Delete(&y);
        String_Delete(&z);
        String_Delete(&w);
    }
}

void test_simple(TestResult* result)
{
    String str1 = String("Lamp Squirrel Lamp Fish");
    String str2 = String("");

    ASSERT(String_FirstOccurrenceOf(&str1, str("Lamp")) == 0);
    ASSERT(String_FirstOccurrenceOf(&str1, str("Squirrel")) == 5);
    ASSERT(String_FirstOccurrenceOf(&str1, str("Fish")) == 19);
    ASSERT(String_FirstOccurrenceOf(&str1, str("Nope")) < 0);
    ASSERT(String_FirstOccurrenceOf(&str1, str("Lamp Squirrel Lamp Fish")) == 0);
    ASSERT(String_FirstOccurrenceOf(&str1, str("")) == 0);

    ASSERT(String_LastOccurrenceOf(&str1, str("Lamp")) == 14);
    ASSERT(String_LastOccurrenceOf(&str1, str("Squirrel")) == 5);
    ASSERT(String_LastOccurrenceOf(&str1, str("Fish")) == 19);
    ASSERT(String_LastOccurrenceOf(&str1, str("Lamp Squirrel Lamp Fish")) == 0);
    ASSERT(String_LastOccurrenceOf(&str1, str("")) == (ssize_t)str1.len);

    ASSERT(String_FirstOccurrenceOf(&str2, str("")) == 0);
    ASSERT(String_LastOccurrenceOf(&str2, str("")) == 0);
    ASSERT(String_FirstOccurrenceOf(&str2, str("Nope")) < 0);
    ASSERT(String_LastOccurrenceOf(&str2, str("Nope")) < 0);

    ASSERT(String_StartsWith(&str1, str("Lamp")) == true);
    ASSERT(String_StartsWith(&str1, str("")) == true);
    ASSERT(String_StartsWith(&str1, str("Nope")) == false);
    ASSERT(String_StartsWith(&str1, str("Squirrel")) == false);

    ASSERT(String_EndsWith(&str1, str("Fish")) == true);
    ASSERT(String_EndsWith(&str1, str("")) == true);
    ASSERT(String_EndsWith(&str1, str("Nope")) == false);
    ASSERT(String_EndsWith(&str1, str("Squirrel")) == false);

    ASSERT(String_StartsWith(&str2, str("")) == true);
    ASSERT(String_EndsWith(&str2, str("")) == true);
    ASSERT(String_StartsWith(&str2, str("Fish")) == false);
    ASSERT(String_EndsWith(&str2, str("Fish")) == false);

    ASSERT(String_Contains(&str1, str("Lamp")) == true);
    ASSERT(String_Contains(&str1, str("Squirrel")) == true);
    ASSERT(String_Contains(&str1, str("Fish")) == true);
    ASSERT(String_Contains(&str1, str("Nope")) == false);
    ASSERT(String_Contains(&str1, str("")) == true);

    ASSERT(String_Contains(&str2, str("")) == true);
    ASSERT(String_Contains(&str2, str("Nope")) == false);

    String_Delete(&str1);
    String_Delete(&str2);
}

void test_comparison(TestResult* result)
{
    String str1   = String("abc");
    String str1_a = String("abc");
    String str2   = String("abcd");
    String str3   = String("abd");
    String str4   = String("");
    String str4_a = String("");
    String str5   = String("aba");

    ASSERT(String_Equal(&str1, &str1_a) == true);
    ASSERT(String_Equal(&str1, &str2) == false);
    ASSERT(String_Equal(&str4, &str4_a) == true);
    ASSERT(String_Equal(&str4, &str3) == false);
    ASSERT(String_Equal(&str2, &str4) == false);

    ASSERT(String_Compare(&str1, &str1_a) == 0);
    ASSERT(String_Compare(&str1, &str2) < 0);
    ASSERT(String_Compare(&str1, &str3) < 0);
    ASSERT(String_Compare(&str1, &str5) > 0);
    ASSERT(String_Compare(&str1, &str4) > 0);
    ASSERT(String_Compare(&str4, &str1) < 0);
    ASSERT(String_Compare(&str4, &str4_a) == 0);

    String_Delete(&str1);
    String_Delete(&str1_a);
    String_Delete(&str2);
    String_Delete(&str3);
    String_Delete(&str4);
    String_Delete(&str4_a);
    String_Delete(&str5);
}

void test_instances(TestResult* result)
{
    String str1 = String("12341234");
    String str2 = String("777777");

    ASSERT(String_InstancesOf(&str1, str("1")) == 2);
    ASSERT(String_InstancesOf(&str1, str("12")) == 2);
    ASSERT(String_InstancesOf(&str1, str("41")) == 1);
    ASSERT(String_InstancesOf(&str1, str("1234")) == 2);
    ASSERT(String_InstancesOf(&str1, str("12341234")) == 1);
    ASSERT(String_InstancesOf(&str1, str("7")) == 0);
    ASSERT(String_InstancesOf(&str1, str("")) == str1.len + 1);
    ASSERT(String_InstancesOf(&str2, str("7")) == 6);
    ASSERT(String_InstancesOf(&str2, str("77")) == 5);
    ASSERT(String_InstancesOf(&str2, str("777")) == 4);
    ASSERT(String_InstancesOf(&str2, str("7777")) == 3);

    ASSERT(String_DistinctInstancesOf(&str1, str("1")) == 2);
    ASSERT(String_DistinctInstancesOf(&str1, str("12")) == 2);
    ASSERT(String_DistinctInstancesOf(&str1, str("41")) == 1);
    ASSERT(String_DistinctInstancesOf(&str1, str("1234")) == 2);
    ASSERT(String_DistinctInstancesOf(&str1, str("12341234")) == 1);
    ASSERT(String_DistinctInstancesOf(&str1, str("7")) == 0);
    ASSERT(String_DistinctInstancesOf(&str1, str("")) == str1.len + 1);
    ASSERT(String_DistinctInstancesOf(&str2, str("7")) == 6);
    ASSERT(String_DistinctInstancesOf(&str2, str("77")) == 3);
    ASSERT(String_DistinctInstancesOf(&str2, str("777")) == 2);
    ASSERT(String_DistinctInstancesOf(&str2, str("7777")) == 1);

    String_Delete(&str1);
    String_Delete(&str2);
}

void test_replace(TestResult* result)
{
    String str1 = String("Bear Lion Sheep Lion Wolf Bear");
    String str2 = String("zzzzz");

    String str1_r1 = String_Replace(&str1, str("Bear"), str("Cricket"));
    String str1_r2 = String_Replace(&str1, str("Lion"), str("No"));
    String str1_r3 = String_Replace(&str1, str("Foo"), str("Bar"));
    String str1_r4 = String_Replace(&str1, str(""), str("Test"));
    String str1_r5 = String_Replace(&str1, str("Lion "), str(""));

    ASSERT(String_Equal(&str1_r1, str("Cricket Lion Sheep Lion Wolf Cricket")));
    ASSERT(String_Equal(&str1_r2, str("Bear No Sheep No Wolf Bear")));
    ASSERT(String_Equal(&str1_r3, str("Bear Lion Sheep Lion Wolf Bear")));
    ASSERT(String_Equal(&str1_r4, str("Bear Lion Sheep Lion Wolf Bear")));
    ASSERT(String_Equal(&str1_r5, str("Bear Sheep Wolf Bear")));

    String str2_r1 = String_Replace(&str2, str("z"), str(""));
    String str2_r2 = String_Replace(&str2, str("z"), str("a"));
    String str2_r3 = String_Replace(&str2, str("z"), str("bb"));
    String str2_r4 = String_Replace(&str2, str("zz"), str(""));
    String str2_r5 = String_Replace(&str2, str("zz"), str("a"));
    String str2_r6 = String_Replace(&str2, str("zz"), str("bb"));
    String str2_r7 = String_Replace(&str2, str("zz"), str("ccc"));
    String str2_r8 = String_Replace(&str2, str("zzz"), str(""));
    String str2_r9 = String_Replace(&str2, str("zzzzz"), str(""));

    ASSERT(String_Equal(&str2_r1, str("")));
    ASSERT(String_Equal(&str2_r2, str("aaaaa")));
    ASSERT(String_Equal(&str2_r3, str("bbbbbbbbbb")));
    ASSERT(String_Equal(&str2_r4, str("z")));
    ASSERT(String_Equal(&str2_r5, str("aaz")));
    ASSERT(String_Equal(&str2_r6, str("bbbbz")));
    ASSERT(String_Equal(&str2_r7, str("ccccccz")));
    ASSERT(String_Equal(&str2_r8, str("zz")));
    ASSERT(String_Equal(&str2_r9, str("")));

    String_Delete(&str1);
    String_Delete(&str2);
    String_Delete(&str1_r1);
    String_Delete(&str1_r2);
    String_Delete(&str1_r3);
    String_Delete(&str1_r4);
    String_Delete(&str1_r5);
    String_Delete(&str2_r1);
    String_Delete(&str2_r2);
    String_Delete(&str2_r3);
    String_Delete(&str2_r4);
    String_Delete(&str2_r5);
    String_Delete(&str2_r6);
    String_Delete(&str2_r7);
    String_Delete(&str2_r8);
    String_Delete(&str2_r9);
}

void test_trim(TestResult* result)
{
    String str1 = String("foo");
    String str2 = String("foo bar");
    String str3 = String(" foo bar ");
    String str4 = String(" \n\r\n\t\f\v foo bar\t\n");
    String str5 = String("");
    String str6 = String("\n\r\t\f\v \t\n\f\v\r");

    String str1_tr = String_Trim(&str1);
    String str2_tr = String_Trim(&str2);
    String str3_tr = String_Trim(&str3);
    String str4_tr = String_Trim(&str4);
    String str5_tr = String_Trim(&str5);
    String str6_tr = String_Trim(&str6);

    ASSERT(String_Equal(&str1_tr, str("foo")));
    ASSERT(String_Equal(&str2_tr, str("foo bar")));
    ASSERT(String_Equal(&str3_tr, str("foo bar")));
    ASSERT(String_Equal(&str4_tr, str("foo bar")));
    ASSERT(String_Equal(&str5_tr, str("")));
    ASSERT(String_Equal(&str6_tr, str("")));

    String_Delete(&str1);
    String_Delete(&str2);
    String_Delete(&str3);
    String_Delete(&str4);
    String_Delete(&str5);
    String_Delete(&str6);
    String_Delete(&str1_tr);
    String_Delete(&str2_tr);
    String_Delete(&str3_tr);
    String_Delete(&str4_tr);
    String_Delete(&str5_tr);
    String_Delete(&str6_tr);
}

void test_split(TestResult* result)
{
    String str1 = String("Test;String;Splitting");
    String str2 = String(";Test;String;Splitting;");
    String str3 = String(";;;;");
    String str4 = String("Test---String---Splitting");
    String str5 = String("---Test---String---Splitting---");
    String str6 = String("Hello World");
    String str7 = String("");

    StringList slist1   = String_Split(&str1, str(";"));
    StringList slist2   = String_Split(&str2, str(";"));
    StringList slist3   = String_Split(&str3, str(";"));
    StringList slist4_1 = String_Split(&str4, str("---"));
    StringList slist5_1 = String_Split(&str5, str("---"));
    StringList slist4_2 = String_Split(&str4, str("--"));
    StringList slist5_2 = String_Split(&str5, str("--"));
    StringList slist6_1 = String_Split(&str6, str(";"));
    StringList slist6_2 = String_Split(&str6, str("Hello World"));
    StringList slist7   = String_Split(&str7, str(";"));

    ASSERT(slist1.len == 3);
    ASSERT(String_Equal(&slist1.str[0], str("Test")));
    ASSERT(String_Equal(&slist1.str[1], str("String")));
    ASSERT(String_Equal(&slist1.str[2], str("Splitting")));

    ASSERT(slist2.len == 5);
    ASSERT(String_Equal(&slist2.str[0], str("")));
    ASSERT(String_Equal(&slist2.str[1], str("Test")));
    ASSERT(String_Equal(&slist2.str[2], str("String")));
    ASSERT(String_Equal(&slist2.str[3], str("Splitting")));
    ASSERT(String_Equal(&slist2.str[4], str("")));

    ASSERT(slist3.len == 5);
    ASSERT(String_Equal(&slist3.str[0], str("")));
    ASSERT(String_Equal(&slist3.str[1], str("")));
    ASSERT(String_Equal(&slist3.str[2], str("")));
    ASSERT(String_Equal(&slist3.str[3], str("")));
    ASSERT(String_Equal(&slist3.str[4], str("")));

    ASSERT(slist4_1.len == 3);
    ASSERT(String_Equal(&slist4_1.str[0], str("Test")));
    ASSERT(String_Equal(&slist4_1.str[1], str("String")));
    ASSERT(String_Equal(&slist4_1.str[2], str("Splitting")));

    ASSERT(slist5_1.len == 5);
    ASSERT(String_Equal(&slist5_1.str[0], str("")));
    ASSERT(String_Equal(&slist5_1.str[1], str("Test")));
    ASSERT(String_Equal(&slist5_1.str[2], str("String")));
    ASSERT(String_Equal(&slist5_1.str[3], str("Splitting")));
    ASSERT(String_Equal(&slist5_1.str[4], str("")));

    ASSERT(slist4_2.len == 3);
    ASSERT(String_Equal(&slist4_2.str[0], str("Test")));
    ASSERT(String_Equal(&slist4_2.str[1], str("-String")));
    ASSERT(String_Equal(&slist4_2.str[2], str("-Splitting")));

    ASSERT(slist5_2.len == 5);
    ASSERT(String_Equal(&slist5_2.str[0], str("")));
    ASSERT(String_Equal(&slist5_2.str[1], str("-Test")));
    ASSERT(String_Equal(&slist5_2.str[2], str("-String")));
    ASSERT(String_Equal(&slist5_2.str[3], str("-Splitting")));
    ASSERT(String_Equal(&slist5_2.str[4], str("-")));

    ASSERT(slist6_1.len == 1);
    ASSERT(String_Equal(&slist6_1.str[0], str("Hello World")));

    ASSERT(slist6_2.len == 2);
    ASSERT(String_Equal(&slist6_2.str[0], str("")));
    ASSERT(String_Equal(&slist6_2.str[1], str("")));

    ASSERT(slist7.len == 1);
    ASSERT(String_Equal(&slist7.str[0], str("")));

    String_Delete(&str1);
    String_Delete(&str2);
    String_Delete(&str3);
    String_Delete(&str4);
    String_Delete(&str5);
    String_Delete(&str6);
    String_Delete(&str7);

    StringList_Delete(&slist1);
    StringList_Delete(&slist2);
    StringList_Delete(&slist3);
    StringList_Delete(&slist4_1);
    StringList_Delete(&slist5_1);
    StringList_Delete(&slist4_2);
    StringList_Delete(&slist5_2);
    StringList_Delete(&slist6_1);
    StringList_Delete(&slist6_2);
    StringList_Delete(&slist7);
}

void test_join(TestResult* result)
{
    String str1 = String("foo ");
    String str2 = String("bar");
    String str3 = String("");

    String join1_2 = String_Join(&str1, &str2);
    String join2_1 = String_Join(&str2, &str1);
    String join2_3 = String_Join(&str2, &str3);
    String join3_3 = String_Join(&str3, &str3);

    ASSERT(String_Equal(&join1_2, str("foo bar")));
    ASSERT(String_Equal(&join2_1, str("barfoo ")));
    ASSERT(String_Equal(&join2_3, str("bar")));
    ASSERT(String_Equal(&join3_3, str("")));

    String_Delete(&str1);
    String_Delete(&str2);
    String_Delete(&str3);
    String_Delete(&join1_2);
    String_Delete(&join2_1);
    String_Delete(&join2_3);
    String_Delete(&join3_3);
}

int main(void)
{
    TestResult result = {0};

    test_creation(&result);
    test_simple(&result);
    test_comparison(&result);
    test_instances(&result);
    test_replace(&result);
    test_trim(&result);
    test_split(&result);
    test_join(&result);

    printf(
        "\n\n"
        "%zu tests passed\n"
        "%zu tests failed\n"
        "%zu total\n",
        result.passed,
        result.failed,
        result.total);

    return result.failed;
}
