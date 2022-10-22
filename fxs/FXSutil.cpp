//#include<���ͷ�ļ�>
#include"AESutil.h"
#include"Haffutil.h"
#include"../jgqj/global.h"
/*
���룺Դ·����Դ�ļ�����Ŀ��·����Ŀ���ļ���������
�����������ɷ���0�����ܷ������󷵻�-1

���ܣ�����Դ·��/Դ�ļ�����ָ����ļ����ļ�����code���ܣ����ܺ���ļ����浽��Ŀ��·��/Ŀ���ļ�����·����
ע�⣺��Դ·��/Դ�ļ������͡�Ŀ��·��/Ŀ���ļ�������ͬʱ������Դ�ļ���
����û�в����µ��ļ�
��Դ·��/Դ�ļ�����ָ��Ŀ������ļ���Ҳ�������ļ������������жϣ������ܺ����ļ���
���ļ��н��м���ʱ�������ļ�ʹ��ͬһ����
*/
using namespace std;
using namespace std::filesystem;

string mergeStr(const char* Path, const char* FileName) {
	string temp = Path;
	temp += "/";
	temp += FileName;
	return temp;
}

int encryption(const char* sourcePath, const char* sourceFileName, const char* targetPath, const char* targetFileName,const string &code) {
	string SourceFileName = mergeStr(sourcePath, sourceFileName);
	string TargetFileName = mergeStr(targetPath, targetFileName);

	createDirList(targetPath);

	path str(SourceFileName);
	directory_entry entry(str);
	//�ļ���
	if (entry.status().type() == file_type::directory) {
		encryptionDir(SourceFileName, TargetFileName, code);
	}
	//δ�ҵ��ļ�
	else if (entry.status().type() == file_type::not_found) {
		cout << "not found file or dir. input again:" << endl;
	}
	//�ļ�
	else {
		encryptionSingleFile(SourceFileName, TargetFileName, 0, code);
	}
	return 0;
}

/*
���룺Դ·����Դ�ļ�����Ŀ��·����Ŀ���ļ���������
�����������ɷ���0�����ܷ��������������󷵻�-1(���ز�ͬ��Ҳ�У���ע���ﱸע)

���ܣ�����Դ·��/Դ�ļ�����ָ����ļ����ļ�����code���ܣ����ܺ���ļ����浽��Ŀ��·��/Ŀ���ļ�����·����
ע�⣺��Դ·��/Դ�ļ������͡�Ŀ��·��/Ŀ���ļ�������ͬʱ������Դ�ļ���
��Դ·��/Դ�ļ�����������ǰ���ļ��У���ԭ��Ҳ���ļ��У����ڲ��ṹ����
���ļ��н��н���ʱ�������ļ�ʹ��ͬһ����
*/
int deEncryption(const char* sourcePath, const char* sourceFileName, const char* targetPath, const char* targetFileName,const string &code) {
	string SourceFileName = mergeStr(sourcePath, sourceFileName);
	string TargetFileName = mergeStr(targetPath, targetFileName);

	createDirList(targetPath);

	fstream testFile;
	testFile.open(SourceFileName);
	if (!testFile) {
		cout << "can not find this file,please input again:" << endl;
		getline(cin, SourceFileName);
	}
	else {
		//�ж����ļ��л����ļ�
		string firstLine;
		getline(testFile, firstLine);
		if (firstLine == "0") {
			//�ļ���
			decryptionDir(SourceFileName, TargetFileName, code);
		}
		else
		{
			if (atoi(firstLine.c_str()) == 0) {
				//�ļ�
				decryptionSingleFile(SourceFileName, TargetFileName, code);
			}
			else {
				//�ļ���
				decryptionDir(SourceFileName, TargetFileName, code);
			}
		}
		testFile.close();
	}
	return 0;
}

/*
���룺Դ·����Դ�ļ�����Ŀ��·����Ŀ���ļ���
�����ѹ����ɷ���0��ѹ���������󷵻�-1

���ܣ�����Դ·��/Դ�ļ�����ָ����ļ����ļ���ѹ����ѹ������ļ����浽��Ŀ��·��/Ŀ���ļ�����·����
ע�⣺��Դ·��/Դ�ļ������͡�Ŀ��·��/Ŀ���ļ�������ͬʱ������Դ�ļ���
ѹ��û�в����µ��ļ�
��Դ·��/Դ�ļ�����ָ��Ŀ������ļ���Ҳ�������ļ������������жϣ���ѹ�������ļ���
*/
int compress(const char* sourcePath, const char* sourceFileName, const char* targetPath, const char* targetFileName) {
	string SourceFileName = mergeStr(sourcePath, sourceFileName);
	string TargetFileName = mergeStr(targetPath, targetFileName);

	createDirList(targetPath);

	path str(SourceFileName);
	directory_entry entry(str);
	//�ļ���
	if (entry.status().type() == file_type::directory) {
		encodeDir(SourceFileName, TargetFileName);
	}
	//δ�ҵ��ļ�
	else if (entry.status().type() == file_type::not_found) {
		cout << "not found file or dir. input again:" << endl;
	}
	//�ļ�
	else {
		encodeSingleFile(SourceFileName, TargetFileName, 0);
	}
	return 0;
}

/*
���룺Դ·����Դ�ļ�����Ŀ��·����Ŀ���ļ���������
�������ѹ��ɷ���0����ѹ�������󷵻�-1

���ܣ�����Դ·��/Դ�ļ�����ָ����ļ����ļ��н�ѹ����ѹ����ļ����浽��Ŀ��·��/Ŀ���ļ�����·����
ע�⣺��Դ·��/Դ�ļ������͡�Ŀ��·��/Ŀ���ļ�������ͬʱ������Դ�ļ���
��Դ·��/Դ�ļ�������ѹ��ǰ���ļ��У���ԭ��Ҳ���ļ��У����ڲ��ṹ����
*/
int deCompress(const char* sourcePath, const char* sourceFileName, const char* targetPath, const char* targetFileName) {
	string SourceFileName = mergeStr(sourcePath, sourceFileName);
	string TargetFileName = mergeStr(targetPath, targetFileName);

	createDirList(targetPath);

	fstream testFile;
	testFile.open(SourceFileName);
	if (!testFile) {
		cout << "can not find this file,please input again:" << endl;
		getline(cin, SourceFileName);
	}
	else {
		//�ж����ļ��л����ļ�
		string firstLine;
		getline(testFile, firstLine);
		if (firstLine == "0") {
			decodeDir(SourceFileName, TargetFileName);
		}
		else
		{
			if (atoi(firstLine.c_str()) == 0) {
				//�ļ�
				decodeSingleFile(SourceFileName, TargetFileName);
			}
			else {
				//�ļ���
				decodeDir(SourceFileName, TargetFileName);
			}
		}
		testFile.close();
	}
	return 0;
}