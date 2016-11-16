//
// Created by Saurabh Tiwari on 15/11/16.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "record_mgr.h"

int split (const char *str, char c, char ***arr)
{
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**) malloc(sizeof(char*) * count);
    if (*arr == NULL)
        exit(1);

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
        {
            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0')
    {
        if (*p != c && *p != '\0')
        {
            *t = *p;
            t++;
        }
        else
        {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}
int main()
{
    Schema * schema = (Schema *) malloc(sizeof(Schema));
    char *sample_string = "Schema with <3> attributes (a: INT, b: STRING[4], c: INT) with keys: (a)\\n";
    char **tokens;
    int c = 0, i;
    char **arr = NULL;
    char **attributeArray = NULL;

    c = split(sample_string, '<', &arr);
    c = split(arr[1], '>', &attributeArray);

    int attrNum = atoi(attributeArray[0]);
    schema->numAttr = attrNum;
    schema->attrNames = (char **) malloc(sizeof(char*)*attrNum);
    schema->dataTypes = (DataType *) malloc(sizeof(DataType)*attrNum);
    schema->typeLength = (int *) malloc(sizeof(int)*attrNum);



//    for (i = 0; i < c; i++)
//        printf("string #%d: %s\n", attrNum, attributeArray[i]);
    char *attributeStringPointer = strstr(attributeArray[1], "(")+1;
    char * substring = strstr(attributeStringPointer, ")");
    int length = substring - attributeStringPointer;
    char *attributeString = malloc(length);
    memcpy(attributeString, attributeStringPointer, length);

    printf("string #%d: %s\n", length, attributeString);
    char **datatypeArray = NULL;
    c = split(attributeString, ',', &datatypeArray);
    char * splitAttribute = (char *) malloc(sizeof(char));
    for (i = 0; i < c; i++)
    {
        splitAttribute = datatypeArray[i];

        splitAttribute = strtok(splitAttribute, ": ");
        schema->attrNames[i] = (char)malloc(sizeof(char));
        schema->attrNames[i] = splitAttribute;
        splitAttribute = strtok(NULL, ": ");
        if(strcmp(splitAttribute,"INT")==0)
        {
            schema->dataTypes[i] = DT_INT;
            schema->typeLength[i] = 0;
        }
        else if(strcmp(splitAttribute,"FLOAT")==0)
        {
            schema->dataTypes[i] = DT_FLOAT;
            schema->typeLength[i] = 0;
        }
        else if(strcmp(splitAttribute,"BOOL")==0)
        {
            schema->dataTypes[i] = DT_BOOL;
            schema->typeLength[i] = 0;
        }
        else
        {
            splitAttribute = strtok(splitAttribute,"[");
            splitAttribute = strtok(NULL,"[");
            splitAttribute = strtok(splitAttribute,"]");
            int strLength = atoi(splitAttribute);
            schema->dataTypes[i]= DT_STRING;
            schema->typeLength[i] = strLength;

        }
    }
    char * keyAttribute = (char *) malloc(sizeof(char));
    keyAttribute = strtok(substring,":");
    keyAttribute =strtok(NULL," ");
    keyAttribute =strtok(keyAttribute,"(");
    keyAttribute =strtok(keyAttribute,")");

   int a = 0;



    return 0;


}
