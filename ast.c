#include <stdio.h>
#include <stdlib.h>
#include "json_c.c"

int main(void)
{   
    FILE *fp;
    fp = fopen("ast.json", "r");

    if (fp == NULL)
    {
        printf("File Open Faild");
        return 1;
    }

    // 파일의 크기를 확인하는 부분
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    // json 파일에서 내용을 읽고 문자열을 저장할 변수
    char *buffer = (char *)malloc(file_size + 1);

    fread(buffer, 1, file_size, fp);

    json_value json = json_create(buffer);
    json_print(json);

    fclose(fp);
    free(buffer);
	return 0;
}