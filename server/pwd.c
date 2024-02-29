#include "pwd.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>




int pwd(MYSQL *conn, dirStackType *dirStk, char *str) {
    if (isEmpty(dirStk)) {
        sprintf(str, "/%s", dirStk->userName);  // 如果目录栈为空，只返回根路径
        return 0;
    }

    // 为了从栈底到栈顶访问，我们可能需要先反转栈或使用临时数组/栈存放元素
    // 这里简化处理，假设用一个足够大的数组模拟这个过程
    // 注意，这种方法可能存在栈非常大时的内存问题
    int tempIds[128]; // 假设栈的深度不会超过128
    int top = 0;
    
   
    // 临时保存原始栈信息
    while (!isEmpty(dirStk)) {
        stkPop(dirStk, &tempIds[top++]);
    }
   
    // 重建原始栈的同时构建路径
    strcpy(str, ""); // 清空字符串
    strcat(str, "/");
    strcat(str, dirStk->userName);

    for(int i = top - 1; i >= 0; i--)
    {
        printf("check loop %d, file id = %d\n", tempIds[i]);
    }
    for (int i = top - 1; i >= 0; i--) {
        
        File file;
        bzero(&file, sizeof(File));
        printf("loop %d, file id = %d\n", i, tempIds[i]);
        int ret = getFileDataById(conn, tempIds[i], &file);
        printf("loop %d, after\n", i);
        if(ret != -2 && ret != -1) {
            strcat(str, "/");
            strcat(str, file.filename);
            stkPush(dirStk, tempIds[i]); // 还原栈结构
        } else {
            return -1;
        }
    }

    return 0;
}
