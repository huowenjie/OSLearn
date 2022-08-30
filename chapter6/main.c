#include <stdio.h>
#include "hack_asm.h"
    
/**
 * 由于语法检查相对复杂，所以我仅仅简单检查语法，并没有深入实现。
 * 所有的功能都堆在 hack_asm.c 中，有些混乱。
 * 本程序没有做任何优化和性能上的考虑，一切以实现功能为准。
 * 
 * HackAsm 的工作流：
 * 1.读取汇编语言文件；
 * 2.获取每一行的内容；
 * 3.判断指令是否有效（检查是否是注释）；
 * 4.格式化内容（去掉空格、注释）；
 * 5.简单检查语法错误；
 * 6.出现跳转符号定义时，记录跳转符号；
 * 7.重新读取文件，并检查语法格式；
 * 8.预处理符号,将从符号表中检索已经定义的符号并替换为地址；
 * 9.将格式化好的指令转换为机器码；
 * 10.逐行保存机器码。
 */
int main(int argc, char *argv[])
{
    HACK_ASM_PARSER *parser = NULL;

    if (argc < 2) {
        printf("请输入需要编译的 asm 文件名！\n");
        goto end;
    }

    parser = hack_asm_create(argv[1]);
    if (!parser) {
        printf("创建语法解析器失败！\n");
        goto end;
    }

    if (hack_asm_first(parser)) {
        printf("定位置首行失败！");
        goto end;
    }

    do {
        if (hack_asm_is_valid(parser)) {
            /* 去除所有的空格和换行 */
            hack_asm_format(parser);

            /* 检查语法错误并输出 */
            const char *error = hack_asm_check_grammar(parser);

            if (error) {
                printf("%s\n", error);
                goto end;
            }

            /* 跳转符号预处理 */
            hack_asm_jmp_prep(parser);
            if (!hack_asm_is_label(parser)) {
                hack_asm_instruct_add(parser);
            }
        }
    } while (!hack_asm_next(parser));

    if (hack_asm_first(parser)) {
        printf("定位置首行失败！");
        goto end;
    }

    do {
        if (hack_asm_is_valid(parser)) {
            /* 去除所有的空格和换行 */
            hack_asm_format(parser);

            /* 检查语法错误并输出 */
            const char *error = hack_asm_check_grammar(parser);

            if (error) {
                printf("%s\n", error);
                goto end;
            }

            /* 符号预处理 */
            hack_asm_prep(parser);

            /* 标签指令则记录指令地址，然后跳过 */
            if (!hack_asm_is_label(parser)) {
                /* 将正确的指令更新在指令列表 */
                hack_asm_update(parser);
                hack_asm_instruct_add(parser);
                printf("%s\n", hack_asm_get_instruct(parser));

                hack_asm_to_bin(parser);
                hack_asm_output(parser);
            }
        }
    } while (!hack_asm_next(parser));

end:
    if (parser) {
        hack_asm_destroy(parser);
    }

    printf("按回车键结束...\n");
    getchar();
    return 0;
}
