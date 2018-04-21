#include <cstdio>
#include <cstdlib>
#include <cstring>


char *check(char *s, int *num, char *split) {
    if (*(s) != '$') {
        return s;
    }
    *split++ = *s;
    *num++ = 0;
    /* $:0  _:1     ^:2     {:3
     * }:4  \int:5   \sum:6    \blank:7
     * id:8 num:9   (:10    ):11    _^:12
     * */
    while (*++s != '$') {
        *split++ = '\t';
        switch (*s) {
            case '_':
                if (*(s + 1) == '^') {
                    *num++ = 12;
                    *split++ = *s++;
                    *split++ = *s;
                    break;
                } else {
                    *num++ = 1;
                    *split++ = *s;
                }
                break;
            case '^':
                *num++ = 2;
                *split++ = *s;
                break;
            case '{':
                *num++ = 3;
                *split++ = *s;
                break;
            case '}':
                *num++ = 4;
                *split++ = *s;
                break;
            case '\\':
                if (*(s + 1) == 'i' && *(s + 2) == 'n' && *(s + 3) == 't') {
                    *num++ = 5;
                    for (int i = 0; i < 4; i++) {
                        *split++ = *s++;
                    }
                    s--;
                    break;
                }
                if (*(s + 1) == 's' && *(s + 2) == 'u' && *(s + 3) == 'm') {
                    *num++ = 6;
                    for (int i = 0; i < 4; i++) {
                        *split++ = *s++;
                    }
                    s--;
                    break;
                }
                if (*(s + 1) == 'b' && *(s + 2) == 'l' && *(s + 3) == 'a' && *(s + 4) == 'n' && *(s + 5) == 'k') {
                    *num++ = 7;
                    for (int i = 0; i < 6; i++) {
                        *split++ = *s++;
                    }
                    s--;
                    break;
                }
                printf("Unknown control sequence!\n");
                return s;
            case '(':
                *num++ = 10;
                *split++ = *s;
                break;
            case ')':
                *num++ = 11;
                *split++ = *s;
                break;
            case ' ':
                break;
            default:
                if (*s >= 48 && *s < 58) {
                    while (*s >= 48 && *s < 58) {
                        *split++ = *s++;
                    }
                    s--;
                    *num++ = 9;
                    break;
                }
                if (*s <= 'Z' && *s >= 'A' || *s <= 'z' && *s >= 'a') {
                    while (*s <= 'Z' && *s >= 'A' || *s <= 'z' && *s >= 'a' || *s >= 48 && *s < 58) {
                        *split++ = *s++;
                    }
                    s--;
                    *num++ = 8;
                    break;
                }
                printf("Unknown token!\n");
                return s;
        }
    }
    *split++ = '\t';
    *split++ = *s;
    *split = 0;
    *num = 0;
    return nullptr;
}


bool match(int *num) {
    /* $:0  _:1     ^:2     {:3
     * }:4  \int:5   \sum:6    \blank:7
     * id:8 num:9   (:10    ):11    _^:12   epsilon:13
     * */
    /* S:0  B:1     C:3     D:2
     * */

    int S = 20, B = 21, C = 23, D = 22;
    //int _$ = 0, __ = 1, _or = 2, left_brace = 3, right_brace = 4, _int = 5, _sum = 6, _blank = 7;
    //int _id = 8, _num = 9, left_parenthesis = 10, right_parenthesis = 11, or_and_ = 12, epsilon = 13;
    int table[4][14][14] = {{{2, 0, B}},
                            {{0},     {0},                {0},                {0},     {0},     {12, 5, 3, B, 4, 3, B, 4, 3, B, 4, D, C}, {12, 6, 3, B, 4, 3, B, 4, 3, B, 4, D, C}, {3, 7, D, C}, {3, 8, D, C}, {3, 9, D, C}, {5, 10, B, 11, D, C}, {0},     {8, 12, 3, B, 4, 3, B, 4, D}, {0}},
                            {{1, 13}, {5, 1, 3, B, 4, D}, {5, 2, 3, B, 4, D}, {0},     {1, 13}, {1,  B},                                  {1,  B},                                  {1, B},       {1, B},       {1, B},       {0},                  {1, 13}, {8, 12, 3, B, 4, 3, B, 4, D}, {1, 13}},
                            {{1, 13}, {0},                {0},                {1, 13}, {1, 13}, {1,  13},                                 {3,  B, D, C},                            {3, B, D, C}, {3, B, D, C}, {3, B, D, C}, {3, B,  D, C},        {1, 13}, {0},                          {1, 13}}};
    int stack[100], height = -1, i = 0, temp, length;
    stack[++height] = B;
    num++;
    while (*num != 0) {
        if ((stack[height]) > 19) {
            if (table[stack[height] - 20][*num][0] == 0) {
                printf("error!");
                return false;
            } else {
                temp = stack[height] - 20;
                printf("Reduction: on the top of stack:%d, in the head of the line: %d.\n", temp, *num);
                stack[height--] = 0;
                length = table[temp][*num][0];
                for (i = length; i > 0; i--) {
                    stack[++height] = table[temp][*num][i];
                }
            }
        } else {
            if (stack[height] == 0) {
                return true;
            } else {
                if (stack[height] == *num) {
                    printf("Move in: on the top of the stack:%d, in the head of the line: %d.\n", temp, *num);
                    stack[height--] = 0;
                    num++;
                } else {
                    if (stack[height] == 13) {
                        printf("Move in: on the top of the stack:%d (epsilon).\n", stack[height]);
                        stack[height--] = 0;
                    } else {
                        printf("Match error!\n\n");
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool cifa_wenfa(char *s, char *split, int num[][100]) {
    FILE *fp;
    char testfile[] = "../information/test/test00.txt";
    for (int i = 1; i < 100; i++) {
        testfile[24] = char(i / 10 + '0');
        testfile[25] = char(i % 10 + '0');
        printf("test%d:  ", i);
        if ((fp = fopen(testfile, "r")) == nullptr) {
            printf("Open file filed!\n\n\n");
            break;
        }
        fgets(s, 100, fp);
        printf("%s", s);
        fclose(fp);

        /* $:0  _:1     ^:2     {:3
         * }:4  \int:5   \sum:6    \blank:7
         * id:8 num:9   (:10    ):11    _^:12   epsilon:13
         * */
        char dict[] = "$:0  _:1     ^:2     {:3     }:4  \\int:5   \\sum:6"
                      "    \\blank:7 id:8 num:9   (:10    ):11";
        char *result = check(s, num[i], split);
        if (result == nullptr) {
            printf("%s\n0\t", dict);
            int j = 0;
            do {
                j++;
                printf("%d\t", num[i][j]);
            } while (num[i][j] != 0);
            printf("\n%s\n", split);
            if (match(num[i])) {
                printf("Congratulations! Match succeed!\n\n");
            } else {
                printf("Something error!\n\n");
            }
        } else {
            printf("last matched: ");
            char *p = s;
            while (p != result) {
                printf("%c", *p);
                p++;
            }
            num[i][0] = 0;
            printf("\n\n");
        }


    }
    return true;
}


int main() {
    char s[100];
    char split[200] = "";
    int num[100][100];
    cifa_wenfa(s, split, num);
    //wenfa(num);
    return 0;
}
