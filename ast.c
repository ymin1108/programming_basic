#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_c.c"

int main(void)
{   
    int number_of_funcdef = 0; // 함수의 개수
    
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
    int ext_size = json_len(ext);


    // ext의 value 값들
    for (int i = 0; i < ext_size; i++)
    {
        // extdml value 값
        json_value ext_index = json_get(ext, i);

        /*
        _nodetype이 FuncDef 또는 Decldls 일 때만 실행
        _nodetype = FuncDef
        _nodetype = Decl > type._nodetype = FuncDecl
         위 두 경우가 함수
        */
        char *nodetype_str = json_get_string(ext_index, "_nodetype");
        if (!strcmp(nodetype_str, "FuncDef"))
        {
            number_of_funcdef++;
            
            json_value funcdef_decl = json_get(ext_index,"decl");


            char *funcdef_decl_str = json_get_string(funcdef_decl, "name");
            printf("%s\n", funcdef_decl_str);
        }
        
        if (!strcmp(nodetype_str, "Decl"))
        {
            json_value decl_type = json_get(ext_index, "type");
            char *decl_type_nodetype = json_get_string(decl_type, "_nodetype");

            if (!strcmp(decl_type_nodetype, "FuncDecl"))
            {   
                number_of_funcdef++;
                char *decl_name = json_get_string(ext_index, "name");
                printf("%s\n", decl_name);
            }
        }
    }
    
    
    printf("\nNumber of function: %d", number_of_funcdef);


    // 메모리 해제
    free(buffer);
    return 0;
}
