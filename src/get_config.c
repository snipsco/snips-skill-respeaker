#include "get_config.h"

int strkv(char *src, char *key, char *value)
{
    char *p,*q;
 
    if(*src == '#') return 0;
    p = strchr(src, '=');
    q = strchr(src, '\n');
 
    if (p != NULL && q != NULL){
        *q = '\0';
        strncpy(key, src, p - src);
        strcpy(value, p+1);
        return 1;
    }else{
        return 0;
    }
}

void config(snipsSkillConfig *configVar, int configNum)
{
    int i;
    FILE *fd;
    char buf[50]="";
    char key[50]="";
    char value[50]=""; 
 
    // open config file
    fd = fopen("config.ini" , "r");
 
    if (fd == NULL){
        printf("can not open config file");
        exit(-1);
    }
 
    // get one line by one line
    while(fgets(buf, 50, fd)){
        // get key-value
        if (strkv(buf, key, value)){
            // if read success, compare with pre-set key
            for(i = 0; i< configNum; i++){
                // if same, copy
                if (strcmp(key, configVar[i].key) == 0){               
                    strcpy(configVar[i].value, value);
                }               
            }
            // empty temp variable
            memset(key, 0, strlen(key));
        }
    }
    fclose(fd);
}

char *get_config_value(char *key, snipsSkillConfig *configVar){
    char *target_key = key;
    int i;
    static char res_value[50]="";
    for(i=0; i<CONFIG_NUM; i++)
    {
        if (strcmp(configVar[i].key, target_key) == 0){

            strcpy(res_value, configVar[i].value);
            return res_value;
        }
    }
    return res_value;
}

int if_config_true(char *key, snipsSkillConfig *configVar, char *value){
    char *res_value;
    res_value = get_config_value(key, configVar);
    if (value != NULL){
        return (strcmp(value, res_value) == 0 ) ? 1: 0;
    }else if (value == NULL){
        if (strcmp("1", res_value) == 0 ) return 1;
        if (strcmp("0", res_value) == 0 ) return 0;
    }
    return -1;
}

void parse_hour_minute(char *raw_value, uint8_t *hour, uint8_t *minute){
    char *p;
    char h[3]="";
    char m[3]="";

    p = strchr(raw_value, ':');
    if (p != NULL){
        strncpy(h, raw_value, p - raw_value); 
        strcpy(m, p+1); 
        *hour = atoi(h);
        *minute = atoi(m);
        return 1;
    }else{
        return 0;
    }
}
