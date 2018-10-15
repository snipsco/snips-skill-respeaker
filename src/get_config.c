#include "get_config.h"

int strkv(char *src, char *key, char *value)
{
    char *p,*q;
    int len;
 
    if(*src == '#') return 0; // # ignore commento
    p = strchr(src, '=');   // p find =
    q = strchr(src, '\n');   // q find \n
 
    // if both p & q
    if (p != NULL && q != NULL){
        *q = '\0'; // set \n as end of string
        strncpy(key, src, p - src); // give the value before = to key
        strcpy(value, p+1); // give the value after = to key
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
	printf("key is %s\n",key);
	printf("value is %s\n",res_value);
        if (strcmp("1", res_value) == 0 ) return 1;
        if (strcmp("0", res_value) == 0 ) return 0;
    }
    return -1;
}
