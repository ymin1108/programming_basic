#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_c.c"

int main(void)
{   
    // 파일 열기
    FILE *fp = fopen("ast.json", "r");
    if (fp == NULL)
    {
        printf("File Open Failed");
        return 1;
    }

    // 파일 크기 확인
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    // 파일 내용 읽기
    char *buffer = (char *)malloc(file_size + 1);
    
    fread(buffer, 1, file_size, fp);
    buffer[file_size] = '\0';
    fclose(fp);

    // json 객체 생성
    json_value json = json_create(buffer);
    // json_print(json);

    json_value ext = json_get(json, "ext");
    int ext_size = json_len(ext);   // 55나옴


    // est의 value의 name을 반목문을 통해 출력
    for (int i = 0; i < ext_size; i++)
    {
        json_value ext_fuction = json_get(ext, i);
        char *name = json_get_string(ext_fuction, "name");
        printf("%s\n", name);
    }


    // 메모리 해제
    free(buffer);
    return 0;
}
