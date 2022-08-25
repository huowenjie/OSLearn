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
