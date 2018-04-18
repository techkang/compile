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
     * id:8 num:9   (:10    ):11
     * */
    while (*++s != '$') {
        *split++ = '\t';
        switch (*s) {
            case '_':
                *num++ = 1;
                *split++ = *s;
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
                    *num++ = -1;
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
                    while (*s >= 48 && *s < 58 || *s == '.') {
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


int main() {
    char s[100];
    char split[200] = "";
    int num[100];
    FILE *fp;
    char testfile[] = "../information/test/test00.txt";
    for (int i = 1; i < 100; i++) {
        testfile[24] = char(i / 10 + '0');
        testfile[25] = char(i % 10 + '0');
        printf("test%d:  ",i);
        if ((fp = fopen(testfile, "r")) == nullptr) {
            printf("Open file filed!");
            break;
        }
        fgets(s, 100, fp);
        printf("%s", s);
        fclose(fp);

        /* $:0  _:1     ^:2     {:3
         * }:4  \int:5   \sum:6    \blank:7
         * id:8 num:9   (:10    ):11
         * */
        char dict[] = "$:0  _:1     ^:2     {:3     }:4  \\int:5   \\sum:6"
                         "    \\blank:7 id:8 num:9   (:10    ):11";
        char *result = check(s, num, split);
        if (result== nullptr){
            printf("%s\n0\t", dict);
            int i = 0;
            do {
                i++;
                printf("%d\t", num[i]);
            } while (num[i] != 0);
            printf("\n%s\n\n", split);
        }
        else {
            printf("last matched: ");
            char *p=s;
            while (p != result)
            {
                printf("%c",*p);
                p++;
            }
            printf("\n\n");
        }

    }

    return 0;
}
