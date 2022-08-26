#include <stdio.h>
#include "hack_asm.h"

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
            /* 检查语法错误并输出 */
            const char *error = hack_asm_check_grammar(parser);

            if (error) {
                printf("%s\n", error);
                goto end;
            }

            hack_asm_format(parser);
            hack_asm_update(parser);
            printf("%s\n", hack_asm_get_instruct(parser));
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
