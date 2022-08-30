#ifndef HACK_ASM_H
#define HACK_ASM_H

/*===========================================================================*/
/* HACK 计算机汇编编译器 */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 代码解析器 */
typedef struct hack_asm_parser HACK_ASM_PARSER;

/* 为汇编程序创建解析器对象 */
extern HACK_ASM_PARSER *hack_asm_create(const char *file);

/* 销毁解析器对象 */
extern void hack_asm_destroy(HACK_ASM_PARSER *parser);

/* 定位在指令首部 */
extern int hack_asm_first(HACK_ASM_PARSER *parser);

/* 定位下一句指令 */
extern int hack_asm_next(HACK_ASM_PARSER *parser);

/* 获取当前指令的行数 */
extern int hack_asm_line(HACK_ASM_PARSER *parser);

/* 获取当前指令的指令内存地址 */
extern int hack_asm_rom_addr(HACK_ASM_PARSER *parser);

/* 当前指令是否有效，如果是，返回 1；否则返回 0 */
extern int hack_asm_is_valid(HACK_ASM_PARSER *parser);

/* 语法检测，返回错误字符串 */
extern const char *hack_asm_check_grammar(HACK_ASM_PARSER *parser);

/* 清除当前指令的空格、注释等 */
extern int hack_asm_format(HACK_ASM_PARSER *parser);

/* 将当前的指令更新到指令内存 */
extern int hack_asm_update(HACK_ASM_PARSER *parser);

/* 当前指令是否是跳转符号 */
extern int hack_asm_is_label(HACK_ASM_PARSER *parser);

/* 将当前指令的预定义符号替换为实际的内存地址 */
extern int hack_asm_prep(HACK_ASM_PARSER *parser);

/* 跳转符号预处理 */
extern int hack_asm_jmp_prep(HACK_ASM_PARSER *parser);

/* 指令 + 1 */
extern void hack_asm_instruct_add(HACK_ASM_PARSER *parser);

/* 获取指令字符串 */
extern const char *hack_asm_get_instruct(HACK_ASM_PARSER *parser);

/* 将汇编语言转换为机器语言指令 */
extern int hack_asm_to_bin(HACK_ASM_PARSER *parser);

/* 输出编译好的机器语言 */
extern int hack_asm_output(HACK_ASM_PARSER *parser);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*===========================================================================*/

#endif /* HACK_ASM_H */
