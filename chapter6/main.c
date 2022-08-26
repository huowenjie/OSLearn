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

    printf("创建成功！\n");

end:
    if (parser) {
        hack_asm_destroy(parser);
    }

    printf("按回车键结束...\n");
    getchar();
    return 0;
}
