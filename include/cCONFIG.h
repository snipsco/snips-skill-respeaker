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

#ifndef __cCONFIG_H__
#define __cCONFIG_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_BUF_LEN 20
#define VAL_BUF_LEN 50

typedef struct CONFIG_LIST{
    char key[KEY_BUF_LEN];
    char value[VAL_BUF_LEN];
    struct CONFIG_LIST *next;
}cCONFIG;
#define NODE_LEN sizeof(cCONFIG)

/**
 * @brief: Load a config file
 *
 * @param[in] DIR: Config file
 *
 * @returns: int\ The number of loaded config options
 *            -1\ Error
 */
int cCONFIG_Parse_Config(const char *DIR);

/**
 * @brief: Get a value by key (string)
 *
 * @param[in] key: Config key
 *
 * @returns: const char *\ The pointer of the result string
 *                   NULL\ No correspond value
 */
const char *cCONFIG_Value_Raw(const char *key);

/**
 * @brief: Check if value is true
 *
 * @param[in] key: Config key
 *
 * @returns:  0\ False
 *            1\ True
 *           -1\ No correspond value or it's not a true/false string
 */
int cCONFIG_Value_Is_True(const char *key);

/**
 * @brief: Print all the config options
 */
void cCONFIG_Dump(void);

/**
 * @brief: Free the memory where the loaded config options saved
 */
void cCONFIG_Delete_List(void);
#endif