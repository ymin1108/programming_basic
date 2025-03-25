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
    int ext_size = json_len(ext);   // 55나옴


    // ext의 value의 name을 반목문을 통해 출력
    for (int i = 0; i < ext_size; i++)
    {
        json_value ext_index = json_get(ext, i);


        char *nodetype_str = json_get_string(ext_index, "_nodetype");
        
        if (!strcmp(nodetype_str, "FuncDef") || (!strcmp(nodetype_str, "Decl")))
        {
            if(!strcmp(nodetype_str, "FuncDef"))
            {
                json_value funcdef_decl = json_get(ext_index,"decl");
                char * funcdef_decl_str = json_get_string(funcdef_decl, "name");
                if(funcdef_decl_str)
                {
                    printf("%s\n", funcdef_decl_str);
                    number_of_funcdef++;
                }
            }
            
            if(!strcmp(nodetype_str, "Decl"))
            {
                json_value decl_type = json_get(ext_index, "type");
                char *decl_type_nodetype = json_get_string(decl_type, "_nodetype");
                char *decl_type_name = json_get_string(decl_type, "declname");

                if (!strcmp(decl_type_nodetype, "FuncDecl"))
                {
                    // printf("\t%s\n", decl_type_nodetype);

                    if (decl_type_name)
                    {
                        printf("%s\n", decl_type_name);
                        number_of_funcdef++;
                    }
                }
            }
        }
    }
    
    
    
    
    
    printf("%d", number_of_funcdef); // 함수 개수 출력


    // 메모리 해제
    free(buffer);
    return 0;
}
