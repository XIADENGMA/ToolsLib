#include "FileOperations.h"

int main()
{
    string         path = "/home/xiadengma/code/other/TestFiles";
    vector<string> filenames;
    GetFileNames(path, filenames, ASC, true);
    for (size_t i = 0; i < filenames.size(); i++)
    {
        cout << filenames[i] << endl;
    }

    return 0;
}
