#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_c.c"

// 노드에서 if 조건 개수를 재귀적으로 카운트하는 함수
// 노드에서 if 조건 개수를 재귀적으로 카운트하는 함수
int count_if_conditions(json_value node)
{
    int count = 0;
    
    if (node.type != JSON_OBJECT) {
        return count;
    }

    char *nodetype_str = json_get_string(node, "_nodetype");
    if (nodetype_str == NULL) {
        return count;
    }
    
    // 1. If 노드인 경우
    if (!strcmp(nodetype_str, "If"))
    {
        // 자신이 If 노드인 경우 카운트 증가
        count = 1;
        
        // If 노드의 자식들 검사
        json_value iftrue = json_get(node, "iftrue");
        if (iftrue.type != JSON_NULL)
        {
            count += count_if_conditions(iftrue);
        }
        
        json_value iffalse = json_get(node, "iffalse");
        if (iffalse.type != JSON_NULL)
        {
            count += count_if_conditions(iffalse);
        }
    }
    // 2. 함수 정의인 경우
    else if (!strcmp(nodetype_str, "FuncDef"))
    {
        // 함수 본문 검사
        json_value body = json_get(node, "body");
        if (body.type != JSON_NULL)
        {
            count += count_if_conditions(body);
        }
    }
    // 3. 복합 구문(블록)인 경우
    else if (!strcmp(nodetype_str, "Compound"))
    {
        // 블록 아이템 검사
        json_value block_items = json_get(node, "block_items");
        if (block_items.type == JSON_ARRAY)
        {
            int items_size = json_len(block_items);
            for (int i = 0; i < items_size; i++)
            {
                json_value item = json_get(block_items, i);
                count += count_if_conditions(item);
            }
        }
    }
    // 4. 반복문(For, While, DoWhile)인 경우
    else if (!strcmp(nodetype_str, "For") || 
             !strcmp(nodetype_str, "While") || 
             !strcmp(nodetype_str, "DoWhile"))
    {
        // stmt 필드 검사
        json_value stmt = json_get(node, "stmt");
        if (stmt.type != JSON_NULL)
        {
            count += count_if_conditions(stmt);
        }
    }
    // 5. Switch 문인 경우
    else if (!strcmp(nodetype_str, "Switch"))
    {
        // stmt 필드 검사
        json_value stmt = json_get(node, "stmt");
        if (stmt.type != JSON_NULL)
        {
            count += count_if_conditions(stmt);
        }
    }
    // 6. Case, Default 문인 경우
    else if (!strcmp(nodetype_str, "Case") || 
             !strcmp(nodetype_str, "Default"))
    {
        // stmts 배열 검사
        json_value stmts = json_get(node, "stmts");
        if (stmts.type == JSON_ARRAY)
        {
            int stmts_size = json_len(stmts);
            for (int i = 0; i < stmts_size; i++)
            {
                json_value stmt = json_get(stmts, i);
                count += count_if_conditions(stmt);
            }
        }
    }
    
    return count;
}

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
    json_value ext = json_get(json, "ext");
    int ext_size = json_len(ext);

    // ext의 value 값들
    for (int i = 0; i < ext_size; i++)
    {
        json_value ext_index = json_get(ext, i);
        char *nodetype_str = json_get_string(ext_index, "_nodetype");
        if (!strcmp(nodetype_str, "FuncDef"))
        {
            json_value funcdef_decl = json_get(ext_index, "decl");
            char *funcdef_decl_str = json_get_string(funcdef_decl, "name");
            json_value funcdef_type = json_get(funcdef_decl, "type");

            // 반환 타입이 포인터인지 확인
            char *type_nodetype = json_get_string(funcdef_type, "type", "_nodetype");
            if (!strcmp(type_nodetype, "PtrDecl"))
            {
                // 포인터 타입인 경우
                json_value ptr_type = json_get(funcdef_type, "type");
                json_value type_decl = json_get(ptr_type, "type");
                json_value type_value = json_get(type_decl, "type");
                json_value names_array = json_get(type_value, "names");
                json_value first_name = json_get(names_array, 0);
                printf("%s\n\tReturn Type: ", funcdef_decl_str);
                json_print(first_name);
                printf("*\n");
            }
            else
            {
                // 일반 타입인 경우
                json_value funcdef_type_type = json_get(funcdef_type, "type");
                json_value names_array = json_get(funcdef_type_type, "type");
                names_array = json_get(names_array, "names");
                json_value first_name = json_get(names_array, 0);
                printf("%s\n\tReturn Type: ", funcdef_decl_str);
                json_print(first_name);
                printf("\n");
            }

            // 파라미터 정보 추출
            json_value args = json_get(funcdef_type, "args");
            if (args.type != JSON_NULL)
            {
                json_value params = json_get(args, "params");
                int params_size = json_len(params);
                printf("\tParameters:\n");
                for (int j = 0; j < params_size; j++)
                {
                    json_value param = json_get(params, j);
                    char *param_name = json_get_string(param, "name");
                    json_value param_type = json_get(param, "type");
                    
                    // 파라미터 타입 추출
                    char *param_type_str = "";
                    json_value type_node = param_type;
                    while (type_node.type != JSON_NULL)
                    {
                        char *nodetype = json_get_string(type_node, "_nodetype");
                        if (!strcmp(nodetype, "TypeDecl"))
                        {
                            json_value ident_type = json_get(type_node, "type");
                            json_value names = json_get(ident_type, "names");
                            param_type_str = json_get_string(names, 0);
                            break;
                        }
                        else if (!strcmp(nodetype, "PtrDecl"))
                        {
                            param_type_str = "pointer to ";
                            type_node = json_get(type_node, "type");
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    printf("\t\t%s %s\n", param_type_str, param_name);
                }
            }

            // if 조건 개수 카운트
            json_value body = json_get(ext_index, "body");
            int if_count = count_if_conditions(body);
            printf("\tIf Conditions Count: %d\n", if_count);

            number_of_funcdef++;
        }
        if (!strcmp(nodetype_str, "Decl"))
        {
            json_value decl_type = json_get(ext_index, "type");
            char *decl_type_nodetype = json_get_string(decl_type, "_nodetype");
            if (!strcmp(decl_type_nodetype, "FuncDecl"))
            {
                char *decl_name = json_get_string(ext_index, "name");
                // 반환 타입이 포인터인지 확인
                char *type_nodetype = json_get_string(decl_type, "type", "_nodetype");
                if (!strcmp(type_nodetype, "PtrDecl"))
                {
                    // 포인터 타입인 경우
                    json_value ptr_type = json_get(decl_type, "type");
                    json_value type_decl = json_get(ptr_type, "type");
                    json_value type_value = json_get(type_decl, "type");
                    json_value names_array = json_get(type_value, "names");
                    json_value first_name = json_get(names_array, 0);
                    printf("%s\n\tReturn Type: ", decl_name);
                    json_print(first_name);
                    printf("*\n");
                }
                else
                {
                    // 일반 타입인 경우
                    json_value decl_type_type = json_get(decl_type, "type");
                    json_value names_array = json_get(decl_type_type, "type");
                    names_array = json_get(names_array, "names");
                    json_value first_name = json_get(names_array, 0);
                    printf("%s\n\tReturn Type: ", decl_name);
                    json_print(first_name);
                    printf("\n");
                }

                // 파라미터 정보 추출
                json_value args = json_get(decl_type, "args");
                if (args.type != JSON_NULL)
                {
                    json_value params = json_get(args, "params");
                    int params_size = json_len(params);
                    printf("\tParameters:\n");
                    for (int j = 0; j < params_size; j++)
                    {
                        json_value param = json_get(params, j);
                        char *param_name = json_get_string(param, "name");
                        json_value param_type = json_get(param, "type");
                        
                        // 파라미터 타입 추출
                        char *param_type_str = "";
                        json_value type_node = param_type;
                        while (type_node.type != JSON_NULL)
                        {
                            char *nodetype = json_get_string(type_node, "_nodetype");
                            if (!strcmp(nodetype, "TypeDecl"))
                            {
                                json_value ident_type = json_get(type_node, "type");
                                json_value names = json_get(ident_type, "names");
                                param_type_str = json_get_string(names, 0);
                                break;
                            }
                            else if (!strcmp(nodetype, "PtrDecl"))
                            {
                                param_type_str = "pointer to ";
                                type_node = json_get(type_node, "type");
                            }
                            else
                            {
                                break;
                            }
                        }
                        
                        printf("\t\t%s %s\n", param_type_str, param_name);
                    }
                }
                
                number_of_funcdef++;
            }
        }
        }

    printf("\nNumber of function: %d", number_of_funcdef);
    // 메모리 해제
    free(buffer);
    return 0;
}
