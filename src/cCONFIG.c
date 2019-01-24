/*
    The MIT License

    Copyright (c) 2018 Coor Fang

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include "cCONFIG.h"

static cCONFIG *config_list = NULL;

/**
 * @brief: Create a new option entity and attach to the list
 *
 * @param[in] key: Key string
 * @param[in] value: Value string
 *
 * @returns: -1\Error or The number of current config options
 *
 */
 static int cCONFIG_Create_New_Node(char *key, char *value) {
    int count = 0;
    cCONFIG *c = config_list;
    cCONFIG *p = (cCONFIG *)malloc(NODE_LEN);
    strcpy(p->key, key);
    strcpy(p->value, value);
    p->next = NULL;
    if (config_list == NULL){
        config_list = p;
        return 1;
    }
    else{
        while( NULL != c->next ){
            count++;
            c = c->next;
        }
        c->next = p;
        return count+2;
    }
 }

/**
 * @brief: Create a new option entity and attach to the list
 *
 * @param[in] line: One line from config.ini
 * @param[out] key: Key string
 * @param[out] value: Value string
 *
 * @returns: 0\Not valid 1\valid
 *
 */
static int cCONFIG_Parse_Line(char *line,  char *key, char *value) {
    char *p, *q;
    if(*line == '#')
        return 0;
    p = strchr(line, '=');
    q = strchr(line, '\0');
    if (p == NULL || q == NULL)
        return 0;
    q = strchr(line, '\n');
    if (q)
        *q = '\0';
    strncpy(key, line, p - line);
    strcpy(value, p+1);
    return 1;
}

/**
 * @brief: Free the memory of the saved options
 */
void cCONFIG_Delete_List(void) {
    cCONFIG *p = config_list;
    cCONFIG *q = p->next;

    while ( p == config_list && q != NULL ) {
        while ( NULL != q->next ) {
            p = q;
            q = p->next;
        }
        free(q);
        p->next = NULL;
        p = config_list;
        q = p->next;
    }
    free(p);
}

/**
 * @brief: Load a config file
 *
 * @param[in] DIR: Config file
 *
 * @returns: -1\Error or The number of loaded config items
 */
int cCONFIG_Parse_Config(const char *DIR){
    int count = 0;
    FILE *fd;
    char buf[ KEY_BUF_LEN + VAL_BUF_LEN ]="";
    char key[KEY_BUF_LEN]="";
    char value[VAL_BUF_LEN]="";

    fd = fopen(DIR, "r");
    if (fd == NULL){
        perror("[Error] Can not open config file");
        return -1;
    }

    do {
        fgets(buf, KEY_BUF_LEN + VAL_BUF_LEN, fd);
        if (cCONFIG_Parse_Line(buf, key, value))
            count = cCONFIG_Create_New_Node(key, value);
        memset(key, 0, strlen(key));
        memset(value, 0, strlen(key));
        memset(buf, 0, strlen(key));
    } while(!feof(fd));
    fclose(fd);
    return count;
}

/**
 * @brief: Get a value by key (string)
 *
 * @param[in] key: Config key
 *
 * @returns: NULL\No correspond value or The pointer of target string
 */
const char *cCONFIG_Value_Raw(const char *key){
    cCONFIG *p = config_list;
    while(p){
        if ( !strcmp(p->key, key) )
            return p->value;
        p = p->next;
    }
    return NULL;
}

/**
 * @brief: Check if value is true by key
 *
 * @param[in] key: Config key
 *
 * @returns: -1\No correspond value or not true/false string \0 False \1 True
 */
int cCONFIG_Value_Is_True(const char *key){
    const char *value = NULL;
    value = cCONFIG_Value_Raw(key);
    if (!value)
        return -1;
    if (!strcmp(value, "true"))
        return 1;
    else if (!strcmp(value, "false"))
        return 0;
    else
        return -1;
}

/**
 * @brief: Print all the key-value options
 *
 */
void cCONFIG_Dump(void){
    int count = 0;
    cCONFIG *p = config_list;
    printf("--------------------------------------\n");
    printf("| No. | KEY             | VALUE      |\n");
    printf("|-----|-----------------|------------|\n");
    while(p){
        printf("| %-4d| %-15s | %-10s |\n", ++count, p->key, p->value);
        p = p->next;
    }
    printf("--------------------------------------\n");
}