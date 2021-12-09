/**
 * @file    FileOpertions.h
 * @brief   取一个路径下所有的文件名(可根据正则表达式)
 * @author  xiadengma
 * @date    2021.11.2
 * @version 1.2.0
 * @note    2021.11.2:添加正则模块
 *          2021.11.3:修改正则输入方式
 */

#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include "iostream"
#include <algorithm>
#include <dirent.h>
#include <regex>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

enum ORDER_FLAG
{
    ASC  = 0,
    DESC = 1
};

//获取目录下的所有文件
void GetFileNames(string path, vector<string>& filenames, int order_flag, string reg = "");

#endif  // FILEOPERATIONS_H
