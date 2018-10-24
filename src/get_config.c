#include "get_config.h"

int read_key_value(char *src, char *key, char *value){
    char *p,*q;
 
    if(*src == '#') return 0;
    p = strchr(src, '=');
    q = strchr(src, '\n');
 
    if (p != NULL && q != NULL){
        *q = '\0';
        strncpy(key, src, p - src);
        strcpy(value, p+1);
        return 1;
    }
    return 0;
}

void read_config_file(snipsSkillConfig *configVar, int configNum){
    int i;
    FILE *fd;
    char buf[50]="";
    char key[50]="";
    char value[50]=""; 
 
    fd = fopen(CONFIG_FILE, "r");
    if (fd == NULL){
        fprintf(stderr, "[Error] Can not open config file");
        exit(-1);
    }

    while(fgets(buf, 50, fd)){
        if (read_key_value(buf, key, value)){
            for(i = 0; i< configNum; i++)
                if (!strcmp(key, configVar[i].key))              
                    strcpy(configVar[i].value, value);             
            memset(key, 0, strlen(key));
        }
    }
    fclose(fd);
}

char *get_config_value(const char *key, snipsSkillConfig *configVar){
    int i;
    static char res_value[50]="";
    for(i=0; i<CONFIG_NUM; i++){   
        if (!strcmp(configVar[i].key, key)){
            strcpy(res_value, configVar[i].value);
            return res_value;
        }
    }
    return res_value;
}

int if_config_true(const char *key, snipsSkillConfig *configVar, char *value){
    char *res_value;
    res_value = get_config_value(key, configVar);
    if (value != NULL){
        return (!strcmp(value, res_value)) ? 1: 0;
    }else if (value == NULL){
        if (!strcmp(TRUE_S, res_value)) return 1;
        if (!strcmp(FALSE_S, res_value)) return 0;
    }
    return -1;
}

int parse_hour_minute(char *raw_value, uint8_t *hour, uint8_t *minute){
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
    }
    return 0;
}
