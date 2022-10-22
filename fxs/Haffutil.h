#ifndef HAFFUTIL_H
#define HAFFUTIL_H
 // !Haffutil.h

#include <string>
#include <bitset>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

using namespace std;
using namespace std::filesystem;
extern char encode10to2(string s);
extern int encode2to10(string s);
extern string encode10to2(int length, int value);
extern int getEncodeOrDecode();
extern vector<string> getEncodeName();
extern vector<string> getDecodeName();
//��ȡѹ���������ļ��Ĵ�С
extern vector<long long> getAfterSize(string path, int nums);
extern long long encodeSingleFile(string sourceFilename, string desFilename, int writeMode);
extern void decodeSingleFile(string sourceFilename, string desFilename);
extern void encodeDir(string path, string desFilename);
extern void decodeDir(string sourceFilename, string desFilename);

#endif