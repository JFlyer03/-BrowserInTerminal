#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

//<h1 em justify="center">out</h1>
typedef struct element {
    char tag[5];
    char Att[20][200];
    int numAtt;
    char AttValue[20][2000];
    int numChild;
    struct element *child[100];
    int h;
    int w;
} element;

element elements[110];
int top = -1;
char c;
int pos = 0;
int print = 0;
// 定义四种状态
// （1）在读取到<后开始读取tag  若读取到空格 停止 进入状态2 若读取到> 进入状态4
// （2）读取Att 读到=停止 进入状态3读取AttValue 若读到空格 进入状态2再次读取Att 若读到> 进入状态4
// （3）读取AttValue 从“ 读取到 ” 之后读到>进入状态4 读到空格 进入状态2
// （4）读到/时出栈 记得将</tag>读取完并top--
// （5）打印中间值

//处理div

char MidTag[5];
char MidAtt[20];
char MidValue[2000];
char output[10000];
int indexTag;
int indexAtt;
int indexValue;
int indexPut;
const char *type[5] = {"h", "p", "img", "div"};
//h表现为全大写 p不做大写处理
const char *hp[5] = {"color", "em", "i", "u"};
const char *image[3] = {"src", "width"};
const char *divv[6] = {"w", "h", "direction", "align-items", "justify-content"};

const char *color[4] = {"red", "blue", "green"};
const char *set[5] = {"start", "center", "end", "space-evenly"};
//目标处理div
int main() {
    while ((c = getchar())!= EOF) {
        if (c == '\n' || c == '\t') {
            continue;
        }

        if (pos == 0) {
            if (c == '<') {
                pos = 1;
                indexTag = 0;
            }
            continue;
        }

        if (pos == 1) {     // 开始标签
            if (c == '/') {
                pos = 4;
                indexTag = 0;
            } else if (isalpha(c)) {
                MidTag[indexTag] = c;
                indexTag++;
            } else if (c == ' ' || c == '>') {  // div
                MidTag[indexTag] = '\0';
                top++;
                strcpy(elements[top].tag, MidTag);
                elements[top].numAtt = 0;
                elements[top].numChild=0;
                elements[top].h=0;
                elements[top].w=0;
                memset(MidTag,'\0',sizeof(MidTag));

                if (c == '>') {
                    pos = 5;
                } else {
                    pos = 2;
                }
            }
            continue;
        }
        if (pos == 2) {     // 读属性
            if (isalpha(c)) {
                MidAtt[indexAtt] = c;
                indexAtt++;
            } else if (c == ' ') {
                if (indexAtt > 0) {
                    MidAtt[indexAtt] = '\0';
                    strcpy(elements[top].Att[elements[top].numAtt], MidAtt);
                    elements[top].numAtt++;
                    memset(MidAtt, '\0', sizeof(MidAtt));
                    indexAtt = 0;
                }
            } else if (c == '>' || c == '=') {
                MidAtt[indexAtt] = '\0';
                strcpy(elements[top].Att[elements[top].numAtt], MidAtt);
                elements[top].numAtt++;
                memset(MidAtt, '\0', sizeof(MidAtt));
                indexAtt = 0;
                if (c == '>') {
                    pos = 5;
                } else if (c == '=') {
                    pos = 3;
                }
            }
            continue;
        }
        if (pos == 3) {     // 读属性值
            if (c == '"') {
                if (indexValue!= 0) {
                    if (elements[top].numAtt > 0) {
                        strcpy(elements[top].AttValue[elements[top].numAtt - 1], MidValue);
                    }
                    memset(MidValue, '\0',sizeof(MidValue));
                    pos = 2;
                    indexValue=0;
                } else {
                    indexValue = 0;
                }
            } else {
                MidValue[indexValue] = c;
                indexValue++;
            }
            continue;
        }
        if (pos == 4) {     // 处理结束标签
            if (c == '>') {
                element *cur= &elements[top];  // 处理 div 父元素         
                if (top >= 0 && strcmp(cur->tag, "div") == 0 &&cur->numChild>0) {
                    int qq = 0;    // 子元素高度之和
                    for (int i = 0; i < cur->numChild; i++) {
                        qq += (cur->child[i])->h;
                    }
                    int divHeight = cur->h;     // 原始父元素高度
                    if(cur->h == 0) {
                        cur->h=qq;
                    }
                    if (divHeight > qq) {   // 父元素
                        for (int i = 0; i < divHeight - qq; i++) {
                            for (int j = 0; j < 50; j++) {
                                printf(" ");
                            }
                            printf("\n");
                        }
                        print += divHeight - qq;
                    } else {
                        print = qq;
                    }
                }
                top--;
                if (top >= 0) {     // 将子元素加入父元素
                    elements[top].child[elements[top].numChild] = malloc(sizeof(elements));
                    memcpy(elements[top].child[elements[top].numChild], cur, sizeof(elements));
                    // elements[top].child[elements[top].numChild] = cur;
                    elements[top].numChild++;
                }
                pos = 0;
                if (top >= 0 && strcmp(cur->tag, "div") == 0 && cur->numChild == 0) {
                    for (int i = 0; i < cur->h; i++) {
                        for (int j = 0; j < 50; j++) {
                            printf(" ");
                        }
                        printf("\n");
                    }
                    print += cur->h;
                }
            }
            continue;
        }

        if (pos == 5) {    
            if (c == '<') {     // 输出内容   </div> 、 <div>
                if (indexPut == 0) {
                    // 输出img内容
                    if (strcmp(elements[top].tag, "img") == 0) {
                        int dot = 0;
                        int len = 0;
                        char s[1000] = "";
                        for (int i = 0; i < elements[top].numAtt; i++) {
                            if (strcmp(elements[top].Att[i], "src") == 0) {
                                len = strlen(elements[top].AttValue[i]);
                                strcpy(s, elements[top].AttValue[i]);
                            }
                            if (strcmp(elements[top].Att[i], "width") == 0) {
                                dot = atoi(elements[top].AttValue[i]);
                            }
                        }

                        for (int i = 0; i < len; i++) {
                            printf("%c", s[i]);
                            if ((i + 1) % dot == 0) {
                                print++;
                                for (int j = 0; j < 50 - dot; j++) {
                                    printf(" ");
                                }
                                printf("\n");
                            }
                        }
                        elements[top].h=len/dot;
                        elements[top].w=dot;

                    }
                    // 输出 div 内容
                    if(strcmp(elements[top].tag,"div")==0) {
                        for(int i=0;i<elements[top].numAtt;i++) {
                            if (strcmp(elements[top].Att[i], "w") == 0) {
                                elements[top].w= atoi(elements[top].AttValue[i]);
                            }
                            if (strcmp(elements[top].Att[i], "h") == 0) {
                                elements[top].h= atoi(elements[top].AttValue[i]);
                            }
                        }
                    }
                    pos = 1;
                    indexTag = 0;
                } else {
                    // 输出 h 和 p 的内容
                    if (strcmp(elements[top].tag, "h") == 0||strcmp(elements[top].tag, "p") == 0) {
                        print++;
                        int flag = 0;
                        for (int i = 0; i < elements[top].numAtt; i++) {
                            if (strcmp(elements[top].Att[i], "color") == 0) {
                                flag = 1;
                                if (strcmp(elements[top].AttValue[i], "red") == 0) {
                                    printf("\033[31m");
                                }
                                if (strcmp(elements[top].AttValue[i], "blue") == 0) {
                                    printf("\033[34m");
                                }
                                if (strcmp(elements[top].AttValue[i], "green") == 0) {
                                    printf("\033[32m");
                                }
                                break;
                            }
                        }
                        for (int i = 0; i < elements[top].numAtt; i++) {
                            if (strcmp(elements[top].Att[i], "em") == 0) {
                                flag = 1;
                                printf("\033[1m");
                                break;
                            }
                        }
                        for (int i = 0; i < elements[top].numAtt; i++) {
                            if (strcmp(elements[top].Att[i], "i") == 0) {
                                flag = 1;
                                printf("\033[3m");
                                break;
                            }
                        }
                        for (int i = 0; i < elements[top].numAtt; i++) {
                            if (strcmp(elements[top].Att[i], "u") == 0) {
                                flag = 1;
                                printf("\033[4m");
                                break;
                            }
                        }
                        for (int i = 0; i < indexPut; i++) {
                           if(strcmp(elements[top].tag, "h") == 0) printf("%c", toupper(output[i]));
                            if(strcmp(elements[top].tag, "p") == 0) printf("%c",output[i]);
                        }

                        if (flag == 1) printf("\033[0m");
                        for (int i = 0; i < 50 - indexPut; i++) {
                            printf(" ");
                        }
                        elements[top].w=indexPut;
                        elements[top].h=1;
                        printf("\n");
                    }
                    pos = 1;
                    indexTag = 0;
                    indexPut = 0;
                }
                memset(output,'\0',sizeof(output));
            } else {    // 内容
                if (strcmp(elements[top].tag, "h") == 0||strcmp(elements[top].tag, "p") == 0){    
                    output[indexPut] = c;
                    indexPut++;}
            }
        }
    }
    for (int i = 0; i < 10 - print; i++) {
        for (int j = 0; j < 50; j++) {
            printf(" ");
        }
        if (i!= 9 - print) printf("\n");
    }
    return 0;
}