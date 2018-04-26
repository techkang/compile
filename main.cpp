//
// Created by ksc on 18-4-22.
//
#include <cstdio>
#include <cstdlib>
#include <cstring>

char map[25][10] = {{"$"},
                    {"_"},
                    {"^"},
                    {"{"},
                    {"}"},
                    {"∫"},
                    {"Σ"},
                    {"\\blank"},
                    {"id"},
                    {"num"},
                    {"("},
                    {")"},
                    {"_^"},
                    {"ε"},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {"S"},
                    {"B"},
                    {"D"},
                    {"C"}};

char *check(char *s, int num[100][5], char *split) {
    if (*(s) != '$') {
        return s;
    }
    *split++ = *s;
    *num[0] = 0;
    num++;
    /* $:0  _:1     ^:2     {:3
     * }:4  \int:5   \sum:6    \blank:7
     * id:8 num:9   (:10    ):11    _^:12
     * */
    while (*++s != '$') {
        *split++ = '\t';
        switch (*s) {
            case '_':
                if (*(s + 1) == '^') {
                    *num[0] = 12;
                    num++;
                    *split++ = *s++;
                    *split++ = *s;
                    break;
                } else {
                    *num[0] = 1;
                    num++;
                    *split++ = *s;
                }
                break;
            case '^':
                *num[0] = 2;
                num++;
                *split++ = *s;
                break;
            case '{':
                *num[0] = 3;
                num++;
                *split++ = *s;
                break;
            case '}':
                *num[0] = 4;
                num++;
                *split++ = *s;
                break;
            case '\\':
                if (*(s + 1) == 'i' && *(s + 2) == 'n' && *(s + 3) == 't') {
                    *num[0] = 5;
                    num++;
                    for (int i = 0; i < 4; i++) {
                        *split++ = *s++;
                    }
                    s--;
                    break;
                }
                if (*(s + 1) == 's' && *(s + 2) == 'u' && *(s + 3) == 'm') {
                    *num[0] = 6;
                    num++;
                    for (int i = 0; i < 4; i++) {
                        *split++ = *s++;
                    }
                    s--;
                    break;
                }
                if (*(s + 1) == 'b' && *(s + 2) == 'l' && *(s + 3) == 'a' && *(s + 4) == 'n' && *(s + 5) == 'k') {
                    *num[0] = 7;
                    num++;
                    for (int i = 0; i < 6; i++) {
                        *split++ = *s++;
                    }
                    s--;
                    break;
                }
                printf("Unknown control sequence!\n");
                return s;
            case '(':
                *num[0] = 10;
                num++;
                *split++ = *s;
                break;
            case ')':
                *num[0] = 11;
                num++;
                *split++ = *s;
                break;
            case ' ':
            case '\n':
                split--;
                break;
            default:
                if (*s >= 48 && *s < 58) {
                    while (*s >= 48 && *s < 58) {
                        *split++ = *s++;
                    }
                    s--;
                    *num[0] = 9;
                    num++;
                    break;
                }
                if (*s <= 'Z' && *s >= 'A' || *s <= 'z' && *s >= 'a') {
                    while (*s <= 'Z' && *s >= 'A' || *s <= 'z' && *s >= 'a' || *s >= 48 && *s < 58) {
                        *split++ = *s++;
                    }
                    s--;
                    *num[0] = 8;
                    num++;
                    break;
                }
                printf("Unknown token!\n");
                return s;
        }
    }
    *split++ = '\t';
    *split++ = *s;
    *split = 0;
    *num[0] = 0;
    return nullptr;
}

bool print_info(int stack[100][5], int num[100][5], const bool flag) {
    printf("%s ", map[*stack[0]]);
    stack++;
    while (*stack[0]) {
        printf("%s ", map[*stack[0]]);
        stack++;
    }
    if (flag) {
        printf("%s \t\t%s ", map[*stack[0]], map[*num[0]]);
        num++;
    } else {
        printf("\t\t");
    }
    printf("%s ", map[*num[0]]);
    while (*num[0]) {
        num++;
        printf("%s ", map[*num[0]]);
    };
    printf("\n");
}

bool inherit(const int father[5], int child[5]) {
    int i = 1;
    for (; i < 3; i++) {
        child[i] = father[i];
    }
    return true;
}

bool superscript(const int father[5], int child[5]) {
    child[2] = int(father[2] * 0.6);
    child[1] = int(-child[2] * 0.5 + father[1]);
    return true;
}

bool subscript(const int father[5], int child[5]) {
    child[2] = int(father[2] * 0.6);
    child[1] = int(father[1] + child[2]);
    return true;
}


int max(int a,int b){
    return a>b?a:b;
}

int next(int (*p)[3],int a){
    while(**p){
        if (**p==a){
            return (*p)[2];
        }
        p++;
    }
    return -1;
}


bool match(int num[100][5], const char *split) {
    /* $:0  _:1     ^:2     {:3
     * }:4  \int:5   \sum:6    \blank:7
     * id:8 num:9   (:10    ):11    _^:12   epsilon:13
     *
     * S:0  B:1     C:3     D:2
     *
     * S -> $ B $
     * B ->  \int{B}{B}{B}D | \sum{B}{B}{B}D | id D | num D | \blank D | (B)D
     * D -> _^{B}D | ^{B}D | _{B}D | BD | epsilon
     * ====================
     * first(B) = { \int, \sum, id, num, \blank, ( }
     * first(D) = { \int, \sum, id, num, \blank, (, epsilon, _, ^, _^ }
     * first(S) = { $ }
     *
     * follow(D)  = { epsilon, $, }, ), id, num, \blank, \int, \sum, ( }
     * */
    const int S = 20, B = 21, D = 22;

    int table[4][14][14] = {{{3, 0, B, 0, 1}},
                            {{0},     {0},                   {0},                   {0}, {0},     {11, 5, 3, B, 4, 3, B, 4, 3, B, 4, D, 2}, {11, 6, 3, B, 4, 3, B, 4, 3, B, 4, D, 3}, {2, 7, D, 4},  {2, 8, D, 5},  {2, 9, D, 6},  {4, 10, B, 11, D, 7}, {0},     {0},                              {0}},
                            {{1, 13}, {5, 1, 3, B, 4, D, 8}, {5, 2, 3, B, 4, D, 9}, {0}, {1, 13}, {2,  B, D, 10},                           {2,  B, D, 10},                           {2, B, D, 10}, {2, B, D, 10}, {2, B, D, 10}, {2, B,  D, 10},       {1, 13}, {8, 12, 3, B, 4, 3, B, 4, D, 11}, {1, 13}}};
    int stack[100][5] = {0},ally[100][3], (*p)[3]=ally,(*q)[3],father[5],(*num_head)[5]=num;
    int height = -1, i = 0, temp, temp2,length,  index,left=0, count = 0;
    bool flag = true;
    stack[++height][0] = S;
    while (height >= 0) {
        if ((stack[height][0]) >= S) {
            if (table[stack[height][0] - S][*num[0]][0] == 0) {
                printf("error!\n");
                return false;
            } else {
                inherit(stack[height], father);
                temp = stack[height][0] - S;
                printf("%s → ", map[temp + S]);
                stack[height--][0] = 0;
                length = table[temp][*num[0]][0];

                (*p)[0]=temp+S;
                (*p)[1]=table[temp][*num[0]][length+1];
                p++;

                for (i = length; i > 0; i--) {
                    stack[++height][0] = table[temp][*num[0]][i];
//                    inherit(table[temp][*num[0]], stack[height - 1]);
                }
                for (i = 0; i < table[temp][*num[0]][0]; i++) {
                    printf("%s ", map[stack[height - i][0]]);
                }
                printf("\n");
                switch (table[temp][*num[0]][length + 1]) {
                    case 1:
                        stack[height - 1][1] = 175;
                        stack[height - 1][2] = 50;
                        stack[height - 1][3] = 0;
                        break;
                    case 2:
                    case 3:
                        inherit(father, stack[height]);
                        subscript(father, stack[height - 2]);
                        superscript(father, stack[height - 5]);
                        inherit(father, stack[height - 8]);
                        inherit(father, stack[height - 10]);
                        break;
                    case 4:
                    case 5:
                    case 6:
                        inherit(father, stack[height]);
                        inherit(father, stack[height - 1]);
                        break;
                    case 7:
                        inherit(father, stack[height]);
                        inherit(father, stack[height - 1]);
                        inherit(father, stack[height - 2]);
                        inherit(father, stack[height - 3]);
                        break;
                    case 8:
                        subscript(father, stack[height - 2]);
                        inherit(father, stack[height - 4]);
                        break;
                    case 9:
                        superscript(father, stack[height - 2]);
                        inherit(father, stack[height - 4]);
                        break;
                    case 10:
                        inherit(father, stack[height]);
                        inherit(father, stack[height - 1]);
                        break;
                    case 11:
                        subscript(father, stack[height - 2]);
                        superscript(father, stack[height - 5]);
                        inherit(father, stack[height - 7]);
                    default:
                        break;
                }
            }
        } else {
            if (stack[height][0] == *num[0]) {
                index = 0;
                while (*split++ != '\t' && (*num[0] || flag)) {
                    count++;
                    index++;
                }
                inherit(stack[height], *num);
                (*num)[4] = count++ - index;
                if((*num)[0]==5||(*num)[0]==6){
                    (*num)[3] = ((*num)[2]);
                } else{
                    (*num)[3] = index * ((*num)[2]);
                }
                print_info(stack, num, flag);
                flag = false;
                stack[height--][0] = 0;

                if (**num>=5&&**num<=11||**num==13){
                    (*p)[0]=**num;
                    (*p)[2]=(*num)[3];
                    p++;
                }

                num++;
            } else {
                if (stack[height][0] == 13) {
                    print_info(stack, num, flag);
                    flag = false;
                    stack[height--][0] = 0;
                } else {
                    printf("Match error!\n");
                    return false;
                }
            }

        }
    }
    if( height != -1){
        return false;
    }
/*
    while (--p!=ally){
        q=p;
        switch (**p){
            case B:
                switch ((*p)[1]){
                    case 2:
                        temp=next(q,B);
                        while (**q!=B){
                            q++;
                        }
                        q++;
                        (*p)[2]=max(temp,next(q,B))+next(q,B)+next(q,D)+next(q,5);
                        break;
                    case 3:
                        temp=next(q,B);
                        while (**q!=B){
                            q++;
                        }
                        q++;
                        temp2=next(q,B);
                        while (**q!=B){
                            q++;
                        }
                        q++;
                        (*p)[2]=max(temp,temp2)+next(q,B)+next(q,D)+next(p,6);
                        break;
                    case 4:(*p)[2]=next(q,7)+next(q,D);
                        break;
                    case 5:(*p)[2]=next(q,8)+next(q,D);
                        break;
                    case 6:(*p)[2]=next(q,9)+next(q,D);
                        break;
                    case 7:(*p)[2]=next(q,10)+next(q,B)+next(q,11)+next(q,D);
                        break;
                    default:
                        break;
                }
            case D:
                switch ((*p)[1]){
                    case 8:
                    case 9:
                    case 10:
                        (*p)[2]=next(q,B)+next(q,D);
                        break;
                    case 11:
                        temp=next(q,B);
                        while (**q!=B){
                            q++;
                        }
                        q++;
                        (*p[2])=max(temp,next(q,B))+next(q,D);
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
    num=num_head;

    stack[++height][0] = S;
    while (height >= 0) {
        q=p;
        if ((stack[height][0]) >= S) {
            if (table[stack[height][0] - S][*num[0]][0] == 0) {
                printf("error!\n");
                return false;
            } else {
                inherit(stack[height], father);
                temp = stack[height][0] - S;
//                printf("%s → ", map[temp + S]);
                stack[height--][0] = 0;
                length = table[temp][*num[0]][0];

//                (*p)[0]=temp+S;
//                (*p)[1]=table[temp][*num[0]][length+1];
//                p++;

                for (i = length; i > 0; i--) {
                    stack[++height][0] = table[temp][*num[0]][i];
//                    inherit(table[temp][*num[0]], stack[height - 1]);
                }
//                for (i = 0; i < table[temp][*num[0]][0]; i++) {
//                    printf("%s ", map[stack[height - i][0]]);
//                }
//                printf("\n");
                switch (table[temp][*num[0]][length + 1]) {
                    case 1:
                        stack[height - 1][1] = 175;
                        stack[height - 1][2] = 50;
                        stack[height - 1][3] = 0;
                        break;
                    case 2:
                    case 3:
                        inherit(father, stack[height]);
                        p++;
                        stack[height][3]=(*p)[2];
                        subscript(father, stack[height - 2]);
                        temp=next(q,B);
                        superscript(father, stack[height - 5]);
                        while (**q!=B){
                            q++;
                        }
                        q++;
                        temp2=next(q,B);
                        stack[height-5][2]=stack[height-2][2]=max(temp,temp2);
                        inherit(father, stack[height - 8]);
                        while (**q!=B){
                            q++;
                        }
                        q++;
                        stack[height-8][3]=next(q,B);
                        inherit(father, stack[height - 10]);
                        stack[height-10][3]=next(q,D);
                        break;
                    case 4:
                    case 5:
                    case 6:
                        inherit(father, stack[height]);
                        stack[height][2]=next(q,(**q)+3);
                        inherit(father, stack[height - 1]);
                        stack[height-1][2]=next(q,D);
                        break;
                    case 7:
                        inherit(father, stack[height]);
                        stack[height][2]=next(q,10);
                        inherit(father, stack[height - 1]);
                        stack[height-1][2]=next(q,B);
                        inherit(father, stack[height - 2]);
                        stack[height-2][2]=next(q,11);
                        inherit(father, stack[height - 3]);
                        stack[height-3][2]=next(q,D);
                        break;
                    case 8:
                        subscript(father, stack[height - 2]);
                        inherit(father, stack[height - 4]);
                        stack[height-2][2]=next(q,B);
                        stack[height-4][2]=next(q,D);
                        break;
                    case 9:
                        superscript(father, stack[height - 2]);
                        inherit(father, stack[height - 4]);
                        stack[height-2][2]=next(q,B);
                        stack[height-4][2]=next(q,D);
                        break;
                    case 10:
                        inherit(father, stack[height]);
                        inherit(father, stack[height - 1]);
                        stack[height][2]=next(q,B);
                        stack[height-1][2]=next(q,D);
                        break;
                    case 11:
                        subscript(father, stack[height - 2]);
                        superscript(father, stack[height - 5]);
                        inherit(father, stack[height - 7]);
                    default:
                        break;
                }
            }
        } else {
            if (stack[height][0] == *num[0]) {
                (*num)[3]=stack[height][3];
//                index = 0;
//                while (*split++ != '\t' && (*num[0] || flag)) {
//                    count++;
//                    index++;
//                }
//                inherit(stack[height], *num);
//                (*num)[4] = count++ - index;
//                if((*num)[0]==5||(*num)[0]==6){
//                    (*num)[3] = ((*num)[2]);
//                } else{
//                    (*num)[3] = index * ((*num)[2]);
//                }
//                (*num)[3] = index * ((*num)[2]);
//                print_info(stack, num, flag);
//                flag = false;
//                stack[height--][0] = 0;
                height--;
//                if (**num>=5&&**num<=11||**num==13){
//                    (*p)[0]=**num;
//                    (*p)[2]=(*num)[3];
//                    p++;
//                }

                num++;
            } else {
                if (stack[height][0] == 13) {
//                    print_info(stack, num, flag);
                    flag = false;
                    stack[height--][0] = 0;
                } else {
                    printf("Match error!\n");
                    return false;
                }
            }

        }
    }
    */
    return true;

}

bool zhidao(const char *split, int num[100][5], FILE *fp) {
    char start[] = "<html>\n"
                   "<head>\n"
                   "<META content=\"text/html; charset=gb2312\">\n"
                   "</head>\n"
                   "<body>";
    char end[] = "</body>\n"
                 "</html>";
    int left = 500, index;
    fprintf(fp, "%s", start);
    //  Σ: &sum;  ∫: &int;
    num++;
    while (*num[0]) {
        switch (*num[0]) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 12:
                break;
            case 5:
            case 6:
            case 10:
            case 11:
                fprintf(fp,
                        "<div style=\"position: absolute; top:%dpx; left:%dpx;\"><span style=\"font-size:%dpx; font-style:oblique; line-\n"
                        "height:100%c;\">%s</span></div>\n", (*num)[1], left, (*num)[2], '%', map[(*num)[0]]);
                left += (*num)[3] / 2;
                break;
            case 7:
                fprintf(fp,
                        "<div style=\"position: absolute; top:%dpx; left:%dpx;\"><span style=\"font-size:%dpx; font-style:oblique; line-\n"
                        "height:100%c;\"> </span></div>\n", (*num)[1], left, (*num)[2], '%');
                left += (*num)[3] / 2;
                break;
            case 8:
            case 9:
                fprintf(fp,
                        "<div style=\"position: absolute; top:%dpx; left:%dpx;\"><span style=\"font-size:%dpx; font-style:oblique; line-\n"
                        "height:100%c;\">", (*num)[1], left, (*num)[2], '%');
                for (index = ((*num)[4]); split[index] != '\t'; index++) {
                    fprintf(fp, "%c", split[index]);
                }
                fprintf(fp, "%s", "</span></div>\n");
                left += (*num)[3] / 2;
                break;
            default:
                break;
        }
        num++;
    }
    fprintf(fp, "%s", end);
    return true;
}

bool cifa_wenfa(char *s, char *split, int num[][100][5]) {
    FILE *fp;
    char testfile[] = "../information/test/test00.txt";
    char resultfile[] = "../information/result/test00.html";
    for (int i = 5; i < 100; i++) {
        testfile[24] = char(i / 10 + '0');
        resultfile[26] = char(i / 10 + '0');
        testfile[25] = char(i % 10 + '0');
        resultfile[27] = char(i % 10 + '0');
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
        char *result = check(s, num[i], split);
        if (result == nullptr) {
            printf("$\t");
            int j = 0;
            do {
                j++;
                printf("%s\t", map[num[i][j][0]]);
            } while (num[i][j][0] != 0);
            printf("\n%s\n\n", split);
            if (match(num[i], split)) {
                printf("Congratulations! Match succeed!\n\n");
                fp = fopen(resultfile, "w");
                zhidao(split, num[i], fp);
                fclose(fp);
            } else {
                printf("\n\n");
            }
        } else {
            printf("last matched: ");
            char *p = s;
            while (p != result) {
                printf("%c", *p);
                p++;
            }
            num[i][0][0] = 0;
            printf("\n\n");
        }


    }
    return true;
}

int main() {
    char s[100];
    char split[200] = "";
    int num[100][100][5];
    cifa_wenfa(s, split, num);
    return 0;
}
