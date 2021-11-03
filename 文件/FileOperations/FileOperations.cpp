/**
 * @file    FileOpertions.cpp
 * @brief   取一个路径下所有的文件名(可根据正则表达式)
 * @author  xiadengma
 * @date    2021.11.3
 * @version 1.2.0
 * @note    2021.11.2:添加正则模块
 *          2021.11.3:修改正则输入方式
 */

#include "FileOperations.h"

/**
 * @brief GetFileNames 获取一个路径下所有的文件名
 * @param path 文件路径字符串
 * @param filenames 文件名容器
 * @param order_flag 升/降序标识符
 * @note  order_flag: ASC=升序 DESC=降序
 *        reg       : 正则表达式字符串,默认不使用正则
 * @example String         path = "/home/test";
            vector<string> filenames;
            GetFileNames(path, filenames, ASC, true);
            for (size_t i = 0; i < filenames.size(); i++)
               cout << filenames[i] << endl;
 */
void GetFileNames(string path, vector<string>& filenames, int order_flag, string reg)
{
    DIR*           pDir;
    struct dirent* ptr;

    if (!(pDir = opendir(path.c_str())))
    {
        return;
    }

    regex  ip_reg(reg);
    smatch match_result;

    while ((ptr = readdir(pDir)) != 0)
    {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
        {
            string temp_string = path + "/" + ptr->d_name;

            //根据正则匹配文件
            if (reg != "" && regex_search(temp_string, match_result, ip_reg))
            {
                filenames.push_back(temp_string);
            }
            else if (reg == "")
            {
                filenames.push_back(temp_string);
            }
        }
    }

    if (order_flag == ASC)
    {
        sort(filenames.begin(), filenames.end(), [](string& name_1, string& name_2) { return name_1 < name_2; });
    }
    else if (order_flag == DESC)
    {
        sort(filenames.begin(), filenames.end(), [](string& name_1, string& name_2) { return name_1 > name_2; });
    }
    else
    {
        cout << "GetFileNames()：输入升/降序标识符错误" << endl;
    }

    closedir(pDir);
}
