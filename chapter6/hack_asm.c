#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    /* 模拟指令内存表，有效指令将会被格式化保存在这个表中 */
    struct hack_rom rom;
};

/**
 * 实现思路：
 * 1.读取汇编语言文件；
 * 2.获取每一行的内容；
 * 3.格式化内容（去掉空格、注释）；
 * 4.解析指令是否有效；
 * 5.如果指令有效，则对指令进行预处理符号替换；
 * 6.预处理的符号在创建解析工具时全部保存在符号表中；
 * 7.将替换后的指令保存在指令内存表；
 * 8.将格式化好的指令转换为机器码输出；
 * 9.逐行保存机器码并对比官方的汇编编译结果。
 */

/*===========================================================================*/

/* 检查文件名的后缀，以 . 分隔。如果 name 的后缀和 suffix 一致，则返回 1，否则返回 0 */
int check_name_suffix(const char *name, const char *suffix);

/*===========================================================================*/

HACK_ASM_PARSER *hack_asm_create(const char *file)
{
    HACK_ASM_PARSER *parser = NULL;
    char *name = NULL;
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


err:

    return NULL;
}

void hack_asm_destroy(HACK_ASM_PARSER *cpl)
{
}

int hack_asm_first(HACK_ASM_PARSER *cpl)
{
    return 0;
}

int hack_asm_next(HACK_ASM_PARSER *cpl)
{
    return 0;
}

int hack_asm_line(HACK_ASM_PARSER *cpl)
{
    return 0;
}

int hack_asm_rom_addr(HACK_ASM_PARSER *cpl)
{
    return 0;
}

int hack_asm_is_valid(HACK_ASM_PARSER *cpl)
{
    return 0;
}

int hack_asm_format(HACK_ASM_PARSER *cpl)
{
    return 0;
}

int hack_asm_is_label(HACK_ASM_PARSER *cpl)
{
    return 0;
}

int hack_asm_prep(HACK_ASM_PARSER *cpl)
{
    return 0;
}

const char *hack_asm_get_instruct(HACK_ASM_PARSER *cpl)
{
    return 0;
}

int hack_asm_output(HACK_ASM_PARSER *cpl)
{
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

/*===========================================================================*/
