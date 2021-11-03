#include "FileOperations.h"

int main()
{
    string         path = "/home/xiadengma/code/other/TestFiles";
    string         reg  = "[0-9]+(?=.(jpe?g|png|bmp|JPG?G|PNG|BMP))";
    vector<string> filenames;
    GetFileNames(path, filenames, ASC, reg);
    for (size_t i = 0; i < filenames.size(); i++)
    {
        cout << filenames[i] << endl;
    }

    return 0;
}
