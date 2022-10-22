#include "Haffutil.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include <filesystem>
#include"fileIO.h"

char encode10to2(string s) {
	bitset<8> bit(s);
	char res = bit.to_ulong();
	return res;
}

int getEncodeOrDecode() {
	cout << "======================Welcome To Use Haffman Encode & Decode Tool========================" << endl
		<< endl << "Encode: input '1'" << endl << "Decode: input '2' :" << endl;
	char mode;
	cin >> mode;
	while (1) {
		if (mode == 49) {
			return 1;
		}
		else if (mode == 50) {
			return 2;
		}
		else {
			cout << "your input is invalid,input it again:" << endl;
			cin >> mode;
		}
	}
}

int encode2to10(string s) {
	bitset<256> bit(s);
	int res = bit.to_ulong();
	return res;
}

string encode10to2(int length, int value) {
	bitset<256> bit(value);
	string temp = bit.to_string();
	return temp.substr(temp.length() - length, length);
}

vector<string> getEncodeName() {
	vector<string> res;
	//�����ļ�(��)
	cout << "------------------------------------" << endl;
	cout << "please input the filename that you want to encode:" << endl << "(include the suffix)" << endl;
	cin.sync();
	//������ļ�(��)Ŀ¼
	while (1) {
		string sourceFileName;
		getchar();
		getline(cin, sourceFileName);
		path str(sourceFileName);
		directory_entry entry(str);
		//�ļ���
		if (entry.status().type() == file_type::directory) {
			res.push_back("0");
			res.push_back(sourceFileName);
			break;
		}
		//δ�ҵ��ļ�
		else if (entry.status().type() == file_type::not_found) {
			cout << "not found file or dir. input again:" << endl;
		}
		//�ļ�
		else {
			res.push_back("1");
			res.push_back(sourceFileName);
			break;
		}
	}
	cout << "------------------------------------" << endl;
	//����ļ�(��)
	cout << "please input the output filename that you want:" << endl << "(include the suffix)" << endl;
	string desFileName;
	getline(cin, desFileName);
	res.push_back(desFileName);
	return res;
}

vector<string> getDecodeName() {
	//�����ļ�
	cout << "------------------------------------" << endl;
	cout << "please input the filename that you want to decode:" << endl << "(include the suffix)" << endl;
	string sourceFileName;
	cin.sync();
	getline(cin, sourceFileName);
	fstream testFile;
	vector<string> res;
	while (1) {
		testFile.open(sourceFileName);
		if (!testFile) {
			cout << "can not find this file,please input again:" << endl;
			getline(cin, sourceFileName);
		}
		else {
			//�ж����ļ��л����ļ�
			string firstLine;
			getline(testFile, firstLine);
			if (firstLine == "0") {
				//�ļ���
				res.push_back("0");
			}
			else {
				if (atoi(firstLine.c_str()) == 0) {
					//�ļ�
					res.push_back("1");
				}
				else {
					//�ļ���
					res.push_back("0");
				}
			}
			testFile.close();
			res.push_back(sourceFileName);
			break;
		}
	}
	cout << "------------------------------------" << endl;
	//����ļ�
	cout << "please input the output file/dir name that you want:" << endl << "(include the suffix)" << endl;
	string desFileName;
	getline(cin, desFileName);
	res.push_back(desFileName);

	return res;
}

vector<long long> getAfterSize(string path, int nums) {
	ifstream fin(path, ios::ate);
	fin.seekg(-1, fin.cur);
	// �鿴ǰһ���ַ��Ƿ�Ϊ�س���
	while ((char)fin.peek() != '\n') {
		fin.seekg(-1, fin.cur);
	}
	fin.seekg(2, fin.cur);
	string line;
	getline(fin, line);
	fin.clear();
	fin.close();
	vector<long long> res;
	istringstream istr(line);
	for (int i = 0; i < nums; i++) {
		string temp;
		istr >> temp;
		res.push_back(atoll(temp.c_str()));
	}
	return res;
}

long long encodeSingleFile(string sourceFilename, string desFilename, int writeMode) {

	long long size = 0;
	//�ж��Ƿ����Ŀ���ļ�
	path str(desFilename);

	fstream fs;
	fs.open(desFilename, ios::in);

	if (!fs)
	{
		//�����ļ�
		ofstream fout(desFilename);
		if (fout)
		{
			fout.close();
		}
	}
	fs.close();

	directory_entry entry(str);
	//�����Ŀ���ļ�,�����С����0
	if (entry.status().type() == file_type::not_found)
		size = 0;
	else
		size = file_size(str);
	FileIO fileIO = FileIO(sourceFilename, desFilename);
	//�ַ���Ƶ��map
	map<char, long long> charFreq;
	map<char, string> charCode;
	charFreq = fileIO.getCharFreq();

	Haffman haffman = Haffman(charFreq);
	//�ַ��Ĺ���������map
	charCode = haffman.createHaffmanCode();
	//���ݹ��������������ļ�ѹ��
	fileIO.encodeFile(fileIO.desFileName, charCode, charFreq, writeMode);
	//����д�ļ��Ĵ�С
	return file_size(str) - size;
}

void decodeSingleFile(string sourceFilename, string desFilename) {
	FileIO fileIO = FileIO(sourceFilename, desFilename);
	//��ȡͷ��Ϣ
	fileHead filehead = fileIO.readFileHead();
	//��ȡ��Ӧ�ļ����ַ���Ƶ��
	map<char, long long> charFreq = fileIO.readFileHaffmanFreq(filehead.alphaVarity);
	//��ѹ���ļ�
	fileIO.decodeFile(filehead, charFreq);
}

void encodeDir(string path, string desFilename) {
	ofstream out(desFilename);
	vector<string> fileName;
	vector<string> dirName;
	filesystem::path p(path);
	//�ѵ�ǰ�ļ������ּ�¼��ȥ
	//dirName.push_back(p.filename().string());
	//Ҫѹ�����ļ��е�ǰ��ָ����Ŀ¼���ַ���
	string headPath=path;
	/*if (path.find('\\') != string::npos) {
		headPath = path.substr(0, path.find(p.filename().string()));
	}
	else {
		headPath = "";
	}*/
	//��¼���ļ�(��)
	for (auto const& entry : recursive_directory_iterator(path)) {
		if (entry.status().type() == file_type::directory) {
			dirName.push_back(entry.path().string().substr(headPath.length()));
		}
		else {
			fileName.push_back(entry.path().string().substr(headPath.length()));
		}
	}
	//д��Ŀ¼�ļ�
	out << dirName.size() << endl;
	for (int i = 0; i < dirName.size(); i++) {
		out << dirName[i] << endl;
	}
	//д���ļ��ļ�
	out << fileName.size() << endl;
	for (int i = 0; i < fileName.size(); i++) {
		//д�ļ���
		out << fileName[i] << endl;
	}
	out.close();
	// д�����������ļ�
	vector<long long> afterSizes;
	for (int i = 0; i < fileName.size(); i++) {
		int aftersize = encodeSingleFile(headPath + fileName[i], desFilename, 1);
		afterSizes.push_back(aftersize);
	}
	// дѹ����Ĵ�С
	ofstream fout(desFilename, ios::app);
	fout << "\n";
	for (int i = 0; i < afterSizes.size(); i++) {
		fout << " " << afterSizes[i];
	}
}

void decodeDir(string sourceFilename, string desFilename) {
	//�ȴ����û�ָ�����ļ���
	if (desFilename != "")
		create_directories(desFilename);
	ifstream is(sourceFilename);
	string dirNum, filesNum, str_filesize;
	string path;
	//��ȡ�ļ���Ŀ¼
	getline(is, dirNum);
	int fileNum = atoi(dirNum.c_str());
	for (int i = 0; i < fileNum; i++) {
		getline(is, path);
		if (desFilename != "")
			create_directories(desFilename + "/" + path);
		else {
			create_directories(path);
		}
	}
	//��ȡ�ļ�
	getline(is, filesNum);
	int fileNums = atoi(filesNum.c_str());
	//�ļ�·��
	vector<string> filePaths;
	for (int i = 0; i < fileNums; i++) {
		getline(is, path);
		filePaths.push_back(path);
	}
	int startIndex = is.tellg();
	is.clear();
	is.close();
	vector<long long> aftersize = getAfterSize(sourceFilename, filePaths.size());
	//��ѹ�������ļ�
	char buffer[512 * 1024];
	ifstream is2(sourceFilename, ios::binary);
	is2.seekg(startIndex);
	for (int i = 0; i < aftersize.size(); i++) {
		ofstream temp("temp.hfm", ios::binary);
		//��ÿ���ļ��Ķ���������д��temp��
		for (int j = 0; j < (aftersize[i] / (512 * 1024)); j++) {
			is2.read(buffer, 512 * 1024 * sizeof(char));
			temp.write(buffer, 512 * 1024 * sizeof(char));
		}
		is2.read(buffer, (aftersize[i] % (512 * 1024)) * sizeof(char));
		temp.write(buffer, (aftersize[i] % (512 * 1024)) * sizeof(char));
		temp.close();
		//��ѹ�������ļ�
		if (desFilename != "")
			decodeSingleFile("temp.hfm", desFilename + "/" + filePaths[i]);
		else {
			decodeSingleFile("temp.hfm", filePaths[i]);
		}
		remove("temp.hfm");
	}
}