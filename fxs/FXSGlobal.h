#include"AESutil.h"
/*
    源文件和目的文件路径不能相同
    加解密不能同时跑
*/
using namespace std;

extern string mergeStr(const char* Path, const char* FileName);

extern int encryption(const char* sourcePath, const char* sourceFileName, const char* targetPath, const char* targetFileName,const string &code) ;

extern int deEncryption(const char* sourcePath, const char* sourceFileName, const char* targetPath, const char* targetFileName,const string &code);

extern int compress(const char* sourcePath, const char* sourceFileName, const char* targetPath, const char* targetFileName) ;

extern int deCompress(const char* sourcePath, const char* sourceFileName, const char* targetPath, const char* targetFileName) ;
