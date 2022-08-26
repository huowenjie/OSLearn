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
    HACK_HASH_MAP *labels = NULL;
    char *name = NULL;
    char *p = NULL;
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

    parser = malloc(sizeof(HACK_ASM_PARSER));
    memset(parser, 0 , sizeof(HACK_ASM_PARSER));

    parser->asm_fp = asm_fp;
    parser->hack_fp = hack_fp;
    parser->label_map = labels;
    parser->rom.cur = NULL;

    hack_link_reset((struct hack_link *)&parser->rom);

    free(name);
    return parser;

err:
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

void hack_asm_destroy(HACK_ASM_PARSER *cpl)
{
    if (!cpl) {
        return;
    }

    /* TODO 需要首先释放指令内存节点 */

    if (cpl->label_map) {
        hack_hash_map_clear(cpl->label_map);
        hack_hash_map_del(cpl->label_map);
        cpl->label_map = NULL;
    }

    if (cpl->asm_fp) {
        fclose(cpl->asm_fp);
        cpl->asm_fp = NULL;
    }

    if (cpl->hack_fp) {
        fclose(cpl->hack_fp);
        cpl->hack_fp = NULL;
    }

    free(cpl);
}

int hack_asm_first(HACK_ASM_PARSER *cpl)
{
    if (!cpl) {
        printf("空指针！\n");
        return -1;
    }

    
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
