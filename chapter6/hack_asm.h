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
extern void hack_asm_destroy(HACK_ASM_PARSER *cpl);

/* 定位在指令首部 */
extern int hack_asm_first(HACK_ASM_PARSER *cpl);

/* 定位下一句指令 */
extern int hack_asm_next(HACK_ASM_PARSER *cpl);

/* 获取当前指令的行数 */
extern int hack_asm_line(HACK_ASM_PARSER *cpl);

/* 获取当前指令的指令内存地址 */
extern int hack_asm_rom_addr(HACK_ASM_PARSER *cpl);

/* 当前指令是否有效，如果是，返回 1；否则返回 0 */
extern int hack_asm_is_valid(HACK_ASM_PARSER *cpl);

/* 清除当前指令的空格、注释等 */
extern int hack_asm_format(HACK_ASM_PARSER *cpl);

/* 当前指令是否是符号 */
extern int hack_asm_is_label(HACK_ASM_PARSER *cpl);

/* 将当前指令的预定义符号替换为实际的内存地址 */
extern int hack_asm_prep(HACK_ASM_PARSER *cpl);

/* 获取指令字符串 */
extern const char *hack_asm_get_instruct(HACK_ASM_PARSER *cpl);

/* 将机器语言更新到输出文件 */
extern int hack_asm_output(HACK_ASM_PARSER *cpl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*===========================================================================*/

#endif /* HACK_ASM_H */
