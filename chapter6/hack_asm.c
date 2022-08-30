#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "hack_link.h"
#include "hack_hash.h"
#include "hack_asm.h"

/*===========================================================================*/

#define ASM_INSTRUCT_A 0 /* A 指令 */
#define ASM_INSTRUCT_C 1 /* C 指令 */
#define ASM_INSTRUCT_LABEL 2 /* 符号 */

/* 指令内存 */
struct hack_rom {
    struct hack_rom_node *head;
    struct hack_rom_node *tail;

    int count;

    /* 当前指令 */
    struct hack_rom_node *cur;
};

/* 指令内存节点 */
struct hack_rom_node {
    struct hack_rom_node *prev;
    struct hack_rom_node *next;

    /* 指令 */
    char *instruct;
};

struct hack_asm_parser {
    /* 汇编语言文件指针 */
    FILE *asm_fp;

    /* 机器语言文件指针 */
    FILE *hack_fp;

    /* 符号表 */
    HACK_HASH_MAP *label_map;

    /* 跳转符号表 */
    HACK_HASH_MAP *jmp_map;

    /* 模拟指令内存表，有效指令将会被格式化保存在这个表中 */
    struct hack_rom rom;

    /* 解析缓冲区和长度 */
    char *buff;
    int buflen;

    /* 当前实际的行数计数器 */
    int line_counter;

    /* 当前有效指令的计数器 */
    int instruct_counter;

    /* 预处理符号 */
    int prep_label;
};

/*===========================================================================*/

/* 检查文件名的后缀，以 . 分隔。如果 name 的后缀和 suffix 一致，则返回 1，否则返回 0 */
static int check_name_suffix(const char *name, const char *suffix);

/* 检查 A 指令语法 */
static int check_a_grammar(const char *code, char *buff, int line);

/* 检查 C 指令语法 */
static int check_c_grammar(const char *code, char *buff, int line);

/* 检查跳转符号语法 */
static int check_label_grammar(const char *code, char *buff, int line);

/* 将十六进制的字符转换为二进制 */
static const char *hex_to_bin(char hex);

/* 获取 A 域串 */
static int hack_asm_cbin_get_a(const char *comp, char *buf);

/* 获取 C 域 */
static int hack_asm_cbin_get_c(const char *comp, char *buf);

/* 获取 d 域 */
static int hack_asm_cbin_get_d(const char *dest, char *buf);

/* 获取 j 域 */
static int hack_asm_cbin_get_j(const char *jump, char *buf);

/* 转换 C 指令 */
static int hack_asm_get_cbin(const char *instruct, char *buf);

/*===========================================================================*/

HACK_ASM_PARSER *hack_asm_create(const char *file)
{
    HACK_ASM_PARSER *parser = NULL;
    HACK_HASH_MAP *labels = NULL;
    HACK_HASH_MAP *jmp_map = NULL;
    char *name = NULL;
    char *p = NULL;
    char *buf = NULL;
    size_t len = 0;

    FILE *asm_fp = NULL;
    FILE *hack_fp = NULL;

    if (!file || !*file) {
        printf("文件名为空\n");
        return NULL;
    }

    if (!check_name_suffix(file, "asm")) {
        printf("文件不是 asm 格式！\n");
        return NULL;
    }

    len = strlen(file) + 2;
    name = malloc(len);
    if (!name) {
        printf("创建缓冲区失败\n");
        return NULL;
    }
    memset(name, 0 , len);
    strcpy(name, file);

    p = strrchr(name, '.');
    p++;
    strcpy(p, "hack");

    /* 打开文件 */
    asm_fp = fopen(file, "r");
    hack_fp = fopen(name, "w");

    if (!asm_fp || !hack_fp) {
        printf("打开文件失败！\n");
        goto err;
    }

    /* 创建符号表 */
    labels = hack_hash_map_new(0);
    if (!labels) {
        printf("创建符号表失败！\n");
        goto err;
    }

    /* 创建跳转符号表 */
    jmp_map = hack_hash_map_new(0);
    if (!jmp_map) {
        printf("创建符号表失败！\n");
        goto err;
    }

    /* 添加预定义的符号 */
    hack_hash_map_insert_int(labels, "SP", 0);
    hack_hash_map_insert_int(labels, "LCL", 1);
    hack_hash_map_insert_int(labels, "ARG", 2);
    hack_hash_map_insert_int(labels, "THIS", 3);
    hack_hash_map_insert_int(labels, "THAT", 4);
    hack_hash_map_insert_int(labels, "R0", 0);
    hack_hash_map_insert_int(labels, "R1", 1);
    hack_hash_map_insert_int(labels, "R2", 2);
    hack_hash_map_insert_int(labels, "R3", 3);
    hack_hash_map_insert_int(labels, "R4", 4);
    hack_hash_map_insert_int(labels, "R5", 5);
    hack_hash_map_insert_int(labels, "R6", 6);
    hack_hash_map_insert_int(labels, "R7", 7);
    hack_hash_map_insert_int(labels, "R8", 8);
    hack_hash_map_insert_int(labels, "R9", 9);
    hack_hash_map_insert_int(labels, "R10", 10);
    hack_hash_map_insert_int(labels, "R11", 11);
    hack_hash_map_insert_int(labels, "R12", 12);
    hack_hash_map_insert_int(labels, "R13", 13);
    hack_hash_map_insert_int(labels, "R14", 14);
    hack_hash_map_insert_int(labels, "R15", 15);
    hack_hash_map_insert_int(labels, "SCREEN", 16384);
    hack_hash_map_insert_int(labels, "KBD", 24576);

    len = 1024;
    buf = malloc(len);
    memset(buf, 0, len);

    parser = malloc(sizeof(HACK_ASM_PARSER));
    memset(parser, 0 , sizeof(HACK_ASM_PARSER));

    parser->asm_fp = asm_fp;
    parser->hack_fp = hack_fp;
    parser->label_map = labels;
    parser->jmp_map = jmp_map;
    parser->rom.cur = NULL;
    parser->buff = buf;
    parser->buflen = len;
    parser->line_counter = 0;
    parser->instruct_counter = 0;
    parser->prep_label = 16;

    hack_link_reset((struct hack_link *)&parser->rom);

    free(name);
    return parser;

err:
    if (jmp_map) {
        hack_hash_map_del(jmp_map);
    }

    if (labels) {
        hack_hash_map_clear(labels);
        hack_hash_map_del(labels);
    }

    if (asm_fp) {
        fclose(asm_fp);
    }

    if (hack_fp) {
        fclose(hack_fp);
    }

    if (name) {
        free(name);
    }
    return NULL;
}

void hack_asm_destroy(HACK_ASM_PARSER *parser)
{
    if (!parser) {
        return;
    }

    /* 释放指令列表 */
    while (parser->rom.count) {
        struct hack_rom_node *node = 
            (struct hack_rom_node *)hack_link_pop((struct hack_link *)&parser->rom);
        if (node) {
            if (node->instruct) {
                free(node->instruct);
            }
            free(node);
        }
    }    

    if (parser->buff) {
        free(parser->buff);
        parser->buff = NULL;
        parser->buflen = 0;
    }

    if (parser->jmp_map) {
        hack_hash_map_clear(parser->jmp_map);
        hack_hash_map_del(parser->jmp_map);
        parser->jmp_map = NULL;
    }

    if (parser->label_map) {
        hack_hash_map_clear(parser->label_map);
        hack_hash_map_del(parser->label_map);
        parser->label_map = NULL;
    }

    if (parser->asm_fp) {
        fclose(parser->asm_fp);
        parser->asm_fp = NULL;
    }

    if (parser->hack_fp) {
        fclose(parser->hack_fp);
        parser->hack_fp = NULL;
    }

    free(parser);
}

int hack_asm_first(HACK_ASM_PARSER *parser)
{
    FILE *asm_fp = NULL;
    char *buf = NULL;
    int buflen = 0;

    if (!parser) {
        printf("空指针！\n");
        return -1;
    }

    asm_fp = parser->asm_fp;
    if (!asm_fp) {
        printf("尚未打开文件！\n");
        return -1;
    }

    rewind(asm_fp);

    buf = parser->buff;
    buflen = parser->buflen;

    if (!fgets(buf, buflen, asm_fp)) {
        return -1;
    }

    parser->line_counter = 1;
    parser->instruct_counter = 0;
    return 0;
}

int hack_asm_next(HACK_ASM_PARSER *parser)
{
    FILE *asm_fp = NULL;
    char *buf = NULL;
    int buflen = 0;

    if (!parser) {
        printf("空指针！\n");
        return -1;
    }

    asm_fp = parser->asm_fp;
    if (!asm_fp) {
        printf("尚未打开文件！\n");
        return -1;
    }

    buf = parser->buff;
    buflen = parser->buflen;

    if (!fgets(buf, buflen, asm_fp)) {
        return -1;
    }

    parser->line_counter++;
    return 0;
}

int hack_asm_line(HACK_ASM_PARSER *parser)
{
    if (parser) {
        return parser->line_counter;
    }
    return 0;
}

int hack_asm_rom_addr(HACK_ASM_PARSER *parser)
{
    /* TODO */
    return 0;
}

int hack_asm_is_valid(HACK_ASM_PARSER *parser)
{
    char *buf = NULL;

    if (!parser) {
        return 0;
    }

    buf = parser->buff;

    while (*buf) {
        if (isgraph(*buf)) {
            /* 第一个可显示字符 */
            if (*buf == '/') {
                char next = *(buf + 1);
                if (next == '/') {
                    return 0;
                }
            }

            /* 其余情况有效，这里不检查语法错误 */
            return 1;
        }

        buf++;
    }

    return 0;
}

const char *hack_asm_check_grammar(HACK_ASM_PARSER *parser)
{
    char *buf = NULL;
    static char buffer[256] = { 0 };

    if (!parser) {
        strcpy(buffer, "解析器异常！\n");
        return buffer;
    }

    buf = parser->buff;

    if (buf[0] == '@') {
        if (!check_a_grammar(buf + 1, buffer, parser->line_counter)) {
            return NULL;
        }
        return buffer;
    }

    if (buf[0] == '(') {
        if (!check_label_grammar(buf, buffer, parser->line_counter)) {
            return NULL;
        }
        return buffer;
    }

    if (!check_c_grammar(buf, buffer, parser->line_counter)) {
        return NULL;
    }
    return buffer;
}

int hack_asm_format(HACK_ASM_PARSER *parser)
{
    char *buf = NULL;
    int i = 0;
    int j = 0;
    int len = 0;

    if (!parser) {
        return -1;
    }

    buf = parser->buff;
    len = parser->buflen;

    for (; i < len - 1 && buf[i]; i++) {
        if (!isgraph(buf[i]) || isspace(buf[i])) {            
            for (j = i; buf[j] != '\0' && j < len - 1; j++) {
                /* 去除所有不可见字符和空白字符 */
                buf[j] = buf[j + 1];
            }
        }
    }

    return 0;
}

int hack_asm_update(HACK_ASM_PARSER *parser)
{
    struct hack_rom_node *node = NULL;
    char *inst = NULL;
    int len = 0;
    
    if (!parser) {
        return -1;
    }

    len = sizeof(struct hack_rom_node);
    node = malloc(len);
    memset(node, 0, len);

    len = strlen(parser->buff) + 1;
    inst = malloc(len);
    memset(inst, 0, len);
    strcpy(inst, parser->buff);

    node->instruct = inst;
    hack_link_push((struct hack_link *)&parser->rom, (struct hack_link_node *)node);

    parser->rom.cur = node;
    return 0;
}

int hack_asm_is_label(HACK_ASM_PARSER *parser)
{
    const char *buf = NULL;

    if (!parser) {
        return 0;
    }

    buf = parser->buff;
    return buf[0] == '(';
}

int hack_asm_prep(HACK_ASM_PARSER *parser)
{
    char *buf = NULL;
    char *p = NULL;

    if (!parser) {
        return -1;
    }

    buf = parser->buff;    
    p = buf + 1;

    /* 跳转符号预处理 */
    if (buf[0] == '(') {
        /* 发现跳转符号定义直接跳过 */
        return 0;
    }

    if (buf[0] != '@') {
        return 0;
    }

    if (!isdigit(*p)) {
        int addr = 0;

        /* 首先查找跳转符号表，查找失败则查找符号表 */
        if (hack_hash_map_find_int(parser->jmp_map, p, &addr) != 0) {
            if (hack_hash_map_find_int(parser->label_map, p, &addr) != 0) {
                addr = parser->prep_label;

                /* 插入新的符号，并指定其数字 */
                hack_hash_map_insert_int(parser->label_map, p, addr);
                parser->prep_label++;
            }
        }

        /* 直接将符号替换为数字 */
        sprintf(p, "%d", addr);
    }

    return 0;
}

int hack_asm_jmp_prep(HACK_ASM_PARSER *parser)
{
    char *buf = NULL;
    char *p = NULL;

    if (!parser) {
        return -1;
    }

    buf = parser->buff;    
    p = buf + 1;

    /* 跳转符号预处理 */
    if (buf[0] == '(') {
        char *rp = strchr(p, ')');

        if (rp) {
            *rp = '\0';
        }

        /* 查找符号表，不论标签是否存在，位置以(label)格式指令为准，标签记录的是指令的索引 */        
        hack_hash_map_insert_int(parser->jmp_map, p, parser->instruct_counter);
    }
    return 0;
}

void hack_asm_instruct_add(HACK_ASM_PARSER *parser)
{
    if (parser) {
        parser->instruct_counter++;
    }
}

const char *hack_asm_get_instruct(HACK_ASM_PARSER *parser)
{
    if (parser && parser->rom.cur) {
        return parser->rom.cur->instruct;
    }
    return NULL;
}

int hack_asm_to_bin(HACK_ASM_PARSER *parser)
{
    char *buf = NULL;
    char *instruct = NULL;

    if (!parser || !parser->rom.cur) {
        return -1;
    }

    buf = parser->buff;
    instruct = parser->rom.cur->instruct;

    /* A 指令转换 */
    if (buf[0] == '@') {
        int addr = 0;
        int i = 4;
        char *save = NULL;
        char *p = NULL;

        addr = atoi(buf + 1);

        /* 获取 A 指令地址数字 */
        sprintf(buf, "%08X", addr);
        save = buf + 17;
        p = save;

        while (i < 8) {
            const char *bin = hex_to_bin(buf[i]);
            strcpy(p, bin);

            p += 4;
            i++;
        }

        strcpy(buf, save);
        return 0;
    }

    /* C 指令转换 */
    return hack_asm_get_cbin(instruct, buf);
}

int hack_asm_output(HACK_ASM_PARSER *parser)
{
    FILE *hack_fp = NULL;

    if (!parser) {
        return -1;
    }

    hack_fp = parser->hack_fp;

    if (hack_fp) {
        //strcat(parser->buff, "\n");
        fputs(parser->buff, hack_fp);
        fputs("\n", hack_fp);
        fflush(hack_fp);
    }

    return 0;
}

/*===========================================================================*/

int check_name_suffix(const char *name, const char *suffix)
{
    const char *p = NULL;

    if (!name || !*name || !suffix || !*suffix) {
        printf("字符串为空！\n");
        return 0;
    }

    p = strrchr(name, '.');
    if (!p) {
        return 0;
    }

    p++;
    if (strcmp(p, suffix)) {
        return 0;
    }

    return 1;
}

int check_a_grammar(const char *code, char *buff, int line)
{
    const char *p = NULL;

    if (!code) {
        sprintf(buff, "指令异常！line:%d\n", line);
        return -1;
    }

    p = code;

    if (isdigit(*p)) {
        while (*(++p)) {
            if (!isdigit(*p)) {
                sprintf(buff, "A 指令的 value 项必须是一个非负十进制数或者一个符号! line:%d\n", line);
                return -1;
            }
        }
        return 0;
    } else if (isalnum(*p)) {
        while (*(++p)) {
            if (!isgraph(*p)) {
                sprintf(buff, "A 指令符号命名错误, 必须以英文字母或者数字来命名! line:%d\n", line);
                return -1;
            }
        }
        return 0;
    }

    sprintf(buff, "符号命名错误, 必须以英文字母或者数字来命名! line:%d\n", line);
    return -1;
}

/* 检查 C 指令语法 */
int check_c_grammar(const char *code, char *buff, int line)
{
    char *p = NULL;
    char *tp = NULL;
    const char *dest = NULL;
    /*const char *comp = NULL;*/
    const char *jump = NULL;

    char *tmp = NULL;
    int size = 0;

    if (!code) {
        sprintf(buff, "指令异常! line:%d\n", line);
        return -1;
    }

    size = strlen(code) + 1;
    tmp = malloc(size);
    memset(tmp, 0, size);
    strcpy(tmp, code);

    /*
     * 检查 C 指令 dest=comp;jump
     * dest 和 jump 可以为空
     * dest 为空，= 被省略
     * jump 为空，; 被省略
     */

    /* 首先检查包含 = 的情况 */
    if ((p = strchr(tmp, '='))) {
        dest = tmp;
        if (dest == p) {
            free(tmp);
            sprintf(buff, "语法格式错误, = 存在时, dest 不能省略! line:%d\n", line);
            return -1;
        }

        tp = strrchr(tmp, '=');
        if (tp && tp != p) {
            free(tmp);
            sprintf(buff, "语法格式错误, = 仅允许出现一次! line:%d\n", line);
            return -1;
        }

        *p++ = '\0';
        /*comp = p;*/

        /* 检查包含 ; 的情况 */
        if ((p = strchr(tmp, ';'))) {
            tp = strrchr(tmp, ';');
            if (tp && tp != p) {
                free(tmp);
                sprintf(buff, "语法格式错误, ; 仅允许出现一次! line:%d\n", line);
                return -1;
            }

            *p = '\0';
            jump = p + 1;

            if (!*jump) {
                free(tmp);
                sprintf(buff, "语法格式错误, ; 存在时, jump 不能省略! line:%d\n", line);
                return -1;
            }
        }
    } else if ((p = strchr(tmp, ';'))) {
        /* 检查包含 ; 的情况 */
        tp = strrchr(tmp, ';');
        if (tp && tp != p) {
            free(tmp);
            sprintf(buff, "语法格式错误, ; 仅允许出现一次! line:%d\n", line);
            return -1;
        }

        /*comp = tmp;*/
        *p = '\0';
        jump = p + 1;

        if (!*jump) {
            free(tmp);
            sprintf(buff, "语法格式错误, ; 存在时, jump 不能省略! line:%d\n", line);
            return -1;
        }
    } else {
        free(tmp);
        sprintf(buff, "语法格式错误, ; 和 = 不能同时省略! line:%d\n", line);
        return -1;
    }

    free(tmp);
    return 0;
}

int check_label_grammar(const char *code, char *buff, int line)
{
    const char *p = NULL;
    const char *rp = NULL;

    if (!code) {
        sprintf(buff, "指令异常！line:%d\n", line);
        return -1;
    }

    p = strchr(code, '(');
    rp = strchr(code, ')');

    if (!p || !rp || !(rp + 1) != '\0') {
        sprintf(buff, "符号命名错误, 必须将符号名称放于括号(label)! line:%d\n", line);
        return -1;
    }

    return 0;
}

const char *hex_to_bin(char hex)
{
    static char bin[5] = { 0 };

    if (!isalnum(hex)) {
        return NULL;
    }

    switch (hex) {
    case '0':
        strcpy(bin, "0000");
        return bin;
    case '1':
        strcpy(bin, "0001");
        return bin;
    case '2':
        strcpy(bin, "0010");
        return bin;
    case '3':
        strcpy(bin, "0011");
        return bin;
    case '4':
        strcpy(bin, "0100");
        return bin;
    case '5':
        strcpy(bin, "0101");
        return bin;
    case '6':
        strcpy(bin, "0110");
        return bin;
    case '7':
        strcpy(bin, "0111");
        return bin;
    case '8':
        strcpy(bin, "1000");
        return bin;
    case '9':
        strcpy(bin, "1001");
        return bin;
    case 'A':
        strcpy(bin, "1010");
        return bin;
    case 'a':
        strcpy(bin, "1010");
        return bin;
    case 'B':
        strcpy(bin, "1011");
        return bin;
    case 'b':
        strcpy(bin, "1011");
        return bin;
    case 'C':
        strcpy(bin, "1100");
        return bin;
    case 'c':
        strcpy(bin, "1100");
        return bin;
    case 'D':
        strcpy(bin, "1101");
        return bin;
    case 'd':
        strcpy(bin, "1101");
        return bin;
    case 'E':
        strcpy(bin, "1110");
        return bin;
    case 'e':
        strcpy(bin, "1110");
        return bin;
    case 'F':
        strcpy(bin, "1111");
        return bin;
    case 'f':
        strcpy(bin, "1111");
        return bin;
    }

    return NULL;
}

int hack_asm_cbin_get_a(const char *comp, char *buf)
{
    if (!comp || !*comp || !buf) {
        return -1;
    }

    if (strchr(comp, 'M')) {
        buf[0] = '1';
    } else {
        buf[0] = '0';
    }

    buf[1] = '\0';
    return 0;
}

int hack_asm_cbin_get_c(const char *comp, char *buf)
{
    if (!comp || !*comp || !buf) {
        return -1;
    }

    if (!strcmp(comp, "0")) {
        strcpy(buf, "101010");
    } else if (!strcmp(comp, "1")) {
        strcpy(buf, "111111");
    } else if (!strcmp(comp, "-1")) {
        strcpy(buf, "111010");
    } else if (!strcmp(comp, "D")) {
        strcpy(buf, "001100");
    } else if (!strcmp(comp, "A") || !strcmp(comp, "M")) {
        strcpy(buf, "110000");
    } else if (!strcmp(comp, "!D")) {
        strcpy(buf, "001101");
    } else if (!strcmp(comp, "!A") || !strcmp(comp, "!M")) {
        strcpy(buf, "110001");
    } else if (!strcmp(comp, "-D")) {
        strcpy(buf, "001111");
    } else if (!strcmp(comp, "-A") || !strcmp(comp, "-M")) {
        strcpy(buf, "110011");
    } else if (!strcmp(comp, "D+1") || !strcmp(comp, "1+D")) {
        strcpy(buf, "011111");
    } else if (!strcmp(comp, "A+1") || !strcmp(comp, "1+A")) {
        strcpy(buf, "110111");
    } else if (!strcmp(comp, "M+1") || !strcmp(comp, "1+M")) {
        strcpy(buf, "110111");
    } else if (!strcmp(comp, "D-1")) {
        strcpy(buf, "001110");
    } else if (!strcmp(comp, "A-1") || !strcmp(comp, "M-1")) {
        strcpy(buf, "110010");
    } else if (!strcmp(comp, "D+A") || !strcmp(comp, "A+D")) {
        strcpy(buf, "000010");
    } else if (!strcmp(comp, "D+M") || !strcmp(comp, "M+D")) {
        strcpy(buf, "000010");
    } else if (!strcmp(comp, "D-A") || !strcmp(comp, "D-M")) {
        strcpy(buf, "010011");
    } else if (!strcmp(comp, "A-D") || !strcmp(comp, "M-D")) {
        strcpy(buf, "000111");
    } else if (!strcmp(comp, "D&A") || !strcmp(comp, "A&D")) {
        strcpy(buf, "000000");
    } else if (!strcmp(comp, "D&M") || !strcmp(comp, "M&D")) {
        strcpy(buf, "000000");
    } else if (!strcmp(comp, "D|A") || !strcmp(comp, "A|D")) {
        strcpy(buf, "010101");
    } else if (!strcmp(comp, "D|M") || !strcmp(comp, "M|D")) {
        strcpy(buf, "010101");
    } else {
        return -1;
    }

    return 0;
}

int hack_asm_cbin_get_d(const char *dest, char *buf)
{
    if (!buf) {
        return -1;
    }

    if (!dest) {
        strcpy(buf, "000");
    } else if (!strcmp(dest, "M")) {
        strcpy(buf, "001");
    } else if (!strcmp(dest, "D")) {
        strcpy(buf, "010");
    } else if (!strcmp(dest, "MD") || !strcmp(dest, "DM")) {
        strcpy(buf, "011");
    } else if (!strcmp(dest, "A")) {
        strcpy(buf, "100");
    } else if (!strcmp(dest, "AM") || !strcmp(dest, "MA")) {
        strcpy(buf, "101");
    } else if (!strcmp(dest, "AD") || !strcmp(dest, "DA")) {
        strcpy(buf, "110");
    } else if ((strchr(dest, 'A') && strchr(dest, 'M') && strchr(dest, 'D')) && (strlen(dest) == (size_t)3)) {
        strcpy(buf, "111");
    } else {
        return -1;
    }

    return 0;
}

int hack_asm_cbin_get_j(const char *jump, char *buf)
{
    if (!buf) {
        return -1;
    }

    if (!jump) {
        strcpy(buf, "000");
    } else if (!strcmp(jump, "JGT")) {
        strcpy(buf, "001");
    } else if (!strcmp(jump, "JEQ")) {
        strcpy(buf, "010");
    } else if (!strcmp(jump, "JGE")) {
        strcpy(buf, "011");
    } else if (!strcmp(jump, "JLT")) {
        strcpy(buf, "100");
    } else if (!strcmp(jump, "JNE")) {
        strcpy(buf, "101");
    } else if (!strcmp(jump, "JLE")) {
        strcpy(buf, "110");
    } else if (!strcmp(jump, "JMP")) {
        strcpy(buf, "111");
    } else {
        return -1;
    }

    return 0;
}

int hack_asm_get_cbin(const char *instruct, char *buf)
{
    char *p = NULL;
    char *tp = NULL;
    const char *dest = NULL;
    const char *comp = NULL;
    const char *jump = NULL;

    char *tmp = NULL;
    int size = 0;

    if (!instruct || !*instruct || !buf) {
        return -1;
    }

    size = strlen(instruct) + 1;
    tmp = malloc(size);
    memset(tmp, 0, size);
    strcpy(tmp, instruct);

    /*
     * 检查 C 指令 dest=comp;jump
     * dest 和 jump 可以为空
     * dest 为空，= 被省略
     * jump 为空，; 被省略
     */

    /* 首先检查包含 = 的情况 */
    if ((p = strchr(tmp, '='))) {
        dest = tmp;
        if (dest == p) {
            goto err;
        }

        tp = strrchr(tmp, '=');
        if (tp && tp != p) {
            goto err;
        }

        *p++ = '\0';
        comp = p;

        /* 检查包含 ; 的情况 */
        if ((p = strchr(tmp, ';'))) {
            tp = strrchr(tmp, ';');
            if (tp && tp != p) {
                goto err;
            }

            *p = '\0';
            jump = p + 1;

            if (!*jump) {
                goto err;
            }
        }
    } else if ((p = strchr(tmp, ';'))) {
        /* 检查包含 ; 的情况 */
        tp = strrchr(tmp, ';');
        if (tp && tp != p) {
            goto err;
        }

        comp = tmp;
        *p = '\0';
        jump = p + 1;

        if (!*jump) {
            goto err;
        }
    } else {
        goto err;
    }

    p = buf;
    strcpy(p, "111");

    p += 3;
    hack_asm_cbin_get_a(comp, p);
    p++;
    hack_asm_cbin_get_c(comp, p);
    p += 6;
    hack_asm_cbin_get_d(dest, p);
    p += 3;
    hack_asm_cbin_get_j(jump, p);

    return 0;

err:
    free(tmp);
    return -1;
}

/*===========================================================================*/
