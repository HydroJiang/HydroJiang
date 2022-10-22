#ifndef FILEIO_H
#define FILEIO_H
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <bitset>
#include "Haffutil.h"
#include <stack>
#include "Haffman.h"
using namespace std;
// ÿ���ļ���ͷ��,�����ļ��Ļ���Ϣ
struct fileHead {
	//�ַ�����
	int alphaVarity;
	//Դ�ļ����ֽ���
	long long originBytes;
};
//��ĸ����Ƶ��
struct alphaCode {
	char alpha;
	//Ƶ��
	long long freq;
	alphaCode() {}
	alphaCode(const pair<char, long long>& x) {
		this->alpha = x.first;
		this->freq = x.second;
	}
};

class FileIO {
public:
	string sourceFileName;
	string desFileName;
	//ѹ���ļ��ķ���
	void encodeFile(string desFileName, map<char, string> charCode, map<char, long long> charFreq, int writeMode);
	map<char, long long> getCharFreq();
	FileIO(string sourceFileName, string desFileName) {
		this->sourceFileName = sourceFileName;
		this->desFileName = desFileName;
	}
	//��ȡ���һ���ֽڵ���Чλ���ķ���
	int getLastValidBit(map<char, long long> charFreq, map<char, string> charCode);


	//��ȡһ���ļ��е�ͷ��Ϣ�ķ���
	fileHead readFileHead();
	//��ȡ�ļ�ԭchar��Ƶ�ʵ���Ϣ�Իָ���������
	map<char, long long> readFileHaffmanFreq(int alphaVariety);
	//��ѹ���ļ�
	void decodeFile(fileHead filehead, map<char, long long> decodeHaffmanFreq);

private:
};
#endif
