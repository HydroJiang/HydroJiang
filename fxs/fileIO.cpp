#include "fileIO.h"
#include <ctime>

map<char, long long> FileIO::getCharFreq() {
	//�ö�����������
	ifstream fin(sourceFileName, ios::binary);
	//��һ��mapƥ���ַ�������ֵ�Ƶ��

	map<char, long long> charFreq;
	char buffer;
	long long charFreqArray[256] = { 0L };
	//һMһM�Ķ�ȡ,����IO����
	long long filesize = file_size(sourceFileName);
	if (filesize == 0) {
		while (!fin.eof()) {
			//ÿ��ֻ����һ���ַ�,���ǻ���2^8���ַ�
			fin.read(&buffer, sizeof(char));
			//��Ӧ�����±�λ�ü�һ
			charFreqArray[(int)buffer + 128]++;
		}
	}
	else {
		char bufferArray[1024 * 1024];
		for (int i = 0; i < filesize / (1024 * 1024); i++) {
			fin.read(bufferArray, 1024 * 1024 * sizeof(char));
			for (int j = 0; j < 1024 * 1024; j++) {
				charFreqArray[(int)bufferArray[j] + 128]++;
			}
		}
		//����1M��ֱ�Ӷ�ȡ
		fin.read(bufferArray, (filesize % (1024 * 1024)) * sizeof(char));
		for (int j = 0; j < filesize % (1024 * 1024); j++) {
			charFreqArray[(int)bufferArray[j] + 128]++;
		}
	}
	//�������ݵ��ַ�д��map
	for (int i = 0; i < 256; i++) {
		if (charFreqArray[i] != 0) {
			charFreq.insert(map<char, long long>::value_type((char)i - 128, charFreqArray[i]));
		}
	}
	fin.close();
	return charFreq;
}

void FileIO::encodeFile(string desFileName, map<char, string> charCode, map<char, long long> charFreq, int writeMode) {
	ofstream fout;
	//������ļ���Ҫд,����׷��д��
	if (writeMode == 1) {
		fout = ofstream(desFileName, ios::binary | ios::app);
	}
	else
		fout = ofstream(desFileName, ios::binary);
	//�Ƚ��ļ���ͷ��Ϣд��
	fileHead filehead;
	// ��ȡ�ַ�������,дͷ��Ϣ
	filehead.originBytes = file_size(sourceFileName);
	filehead.alphaVarity = charCode.size();
	fout.write((char *)&filehead, sizeof(filehead));
	//д�ַ���Ƶ�ȵȵ�
	for (auto i : charFreq) {
		alphaCode af(i);
		fout.write((char*)&af, sizeof(af));
	}
	//����������ŵ�һ���ṹ������
	struct CharInfo {
		CharInfo() : code("") { }
		string code;
	}charInfoArray[256];
	//��ĸ�����������
	map<char, string>::reverse_iterator   iter;
	for (iter = charCode.rbegin(); iter != charCode.rend(); iter++) {
		charInfoArray[(int)iter->first + 128].code = iter->second;
	}
	//д�ļ�������
	ifstream fin(sourceFileName, ios::binary);
	long long filesize = file_size(sourceFileName);
	int bufferLength = 0;
	//ÿ��ֻ����һ���ַ�,���ǻ���2^8���ַ�
	unsigned char bufferbit = 0;
	char readChars[512 * 1024];
	char bufferArray[512 * 1024];
	int bufferArrayIndex = 0;
	char buffer;
	if (filesize == 0) {
		while (!fin.eof()) {
			fin.read(&buffer, sizeof(char));
			for (int strIdx = 0; strIdx < charInfoArray[(int)buffer + 128].code.length(); strIdx++) {
				bufferbit <<= 1;
				bufferbit |= (charInfoArray[(int)buffer + 128].code[strIdx] == '1');
				bufferLength++;
				//����һ���ַ���ʱ��ͷŽ���д������
				if (bufferLength == 8) {
					bufferArray[bufferArrayIndex] = bufferbit;
					bufferArrayIndex++;
					bufferbit = 0;
					bufferLength = 0;
				}
				//���buffer��������,��д���ļ�
				if (bufferArrayIndex == 1024) {
					fout.write(bufferArray, sizeof(char) * 1024);
					bufferArrayIndex = 0;
				}
			}
		}
	}
	else {
		//ÿ�ζ�1M���ֽ�,����IO����
		for (int i = 0; i < (filesize / (512 * 1024)); i++) {
			fin.read(readChars, (512 * 1024) * sizeof(char));
			for (int j = 0; j < (512 * 1024); j++) {
				for (int strIdx = 0; strIdx < charInfoArray[(int)readChars[j] + 128].code.length(); strIdx++) {
					bufferbit <<= 1;
					bufferbit |= (charInfoArray[(int)readChars[j] + 128].code[strIdx] == '1');
					bufferLength++;
					//����һ���ַ���ʱ��ͷŽ���д������
					if (bufferLength == 8) {
						bufferArray[bufferArrayIndex] = bufferbit;
						bufferArrayIndex++;
						bufferbit = 0;
						bufferLength = 0;
					}
					//���buffer��������,��д���ļ�
					if (bufferArrayIndex == 512 * 1024) {
						fout.write(bufferArray, sizeof(char) * 512 * 1024);
						bufferArrayIndex = 0;
					}
				}
			}
		}
		//�����1M������
		fin.read(readChars, (filesize % (512 * 1024)) * sizeof(char));
		for (int j = 0; j < (filesize % (512 * 1024)); j++) {
			for (int strIdx = 0; strIdx < charInfoArray[(int)readChars[j] + 128].code.length(); strIdx++) {
				bufferbit <<= 1;
				bufferbit |= (charInfoArray[(int)readChars[j] + 128].code[strIdx] == '1');
				bufferLength++;
				//����һ���ַ���ʱ��ͷŽ���д������
				if (bufferLength == 8) {
					bufferArray[bufferArrayIndex] = bufferbit;
					bufferArrayIndex++;
					bufferbit = 0;
					bufferLength = 0;
				}
				//���buffer��������,��д���ļ�
				if (bufferArrayIndex == 512 * 1024) {
					fout.write(bufferArray, sizeof(char) * 512 * 1024);
					bufferArrayIndex = 0;
				}
			}
		}
	}
	//�������8λ��bit��ȫ��д��
	if (bufferLength != 0) {
		while (bufferLength != 8) {
			bufferbit <<= 1;
			bufferLength++;
		}
		bufferArray[bufferArrayIndex] = bufferbit;
		bufferArrayIndex++;
	}
	//д����ʣ�µĶ���
	if (bufferArrayIndex != 0) {
		fout.write(bufferArray, bufferArrayIndex * sizeof(char));
	}
	fout.close();
}

int FileIO::getLastValidBit(map<char, long long> charFreq, map<char, string> charCode) {
	int sum = 0;
	for (auto i : charCode) {
		sum += i.second.size() * charFreq.at(i.first);
		sum &= 0xFF;
	}
	sum &= 0x7;
	return sum == 0 ? 8 : sum;
}

fileHead FileIO::readFileHead() {
	fileHead filehead;
	ifstream is(sourceFileName, ios::binary);
	is.read((char *)&filehead, sizeof(filehead));
	is.close();
	return filehead;
}

map<char, long long> FileIO::readFileHaffmanFreq(int alphaVariety) {
	ifstream is(sourceFileName, ios::binary);
	map<char, long long> codeFreq;
	//��λ��ͷ��Ϣ��
	is.seekg(sizeof(fileHead));
	//�������ַ���Ƶ�Ȼ�ȡ
	for (int i = 0; i < alphaVariety; i++) {
		alphaCode af;
		is.read((char *)&af, sizeof(af));
		codeFreq.insert(make_pair(af.alpha, af.freq));
	}
	is.close();
	return codeFreq;
}

void FileIO::decodeFile(fileHead filehead, map<char, long long> charFreq) {
	ifstream is(sourceFileName, ios::binary);
	ofstream out(desFileName, ios::binary);
	//���ļ�ֱ��return
	if (charFreq.size() == 0) {
		return;
	}

	//�ָ���������
	Haffman haffman = Haffman(charFreq);
	haffman.createHaffmanTree();
	Node root = haffman.nodeQueue.top();
	Node temp = root;
	//��λ���洢�ļ���λ��
	is.seekg(sizeof(filehead) + filehead.alphaVarity * sizeof(alphaCode), ios::beg);
	//��ʼ��ȡ
	char readBuf;
	long long writedBytes = 0;
	char readChars[512 * 1024];
	char writeBufferArray[512 * 1024];
	int writeBufferArrayIndex = 0;
	long long filesize = file_size(sourceFileName) - sizeof(filehead) + filehead.alphaVarity * sizeof(alphaCode);
	for (int i = 0; i < filesize / (512 * 1024); i++) {
		is.read(readChars, (512 * 1024) * sizeof(char));
		for (int j = 0; j < (512 * 1024); j++) {
			for (int k = 7; k >= 0; k--) {
				if (readChars[j] & (1 << k))
					temp = *temp.right;
				else
					temp = *temp.left;
				if (haffman.isLeaf(&temp)) {
					//���ַ��ŵ�����������
					writeBufferArray[writeBufferArrayIndex] = temp.c;
					//����ָ���һ
					writeBufferArrayIndex++;
					temp = root;
					writedBytes++;
				}
				//����������,д���ļ�
				if (writeBufferArrayIndex == 512 * 1024) {
					out.write(writeBufferArray, 512 * 1024 * sizeof(char));
					writeBufferArrayIndex = 0;
				}
				if (writedBytes >= filehead.originBytes) {
					goto finish;
				}
			}
		}
	}
	is.read(readChars, (filesize % (512 * 1024)) * sizeof(char));
	for (int j = 0; j < (filesize % (512 * 1024)); j++) {
		for (int k = 7; k >= 0; k--) {
			if (readChars[j] & (1 << k))
				temp = *temp.right;
			else
				temp = *temp.left;
			if (haffman.isLeaf(&temp)) {
				//���ַ��ŵ�����������
				writeBufferArray[writeBufferArrayIndex] = temp.c;
				//����ָ���һ
				writeBufferArrayIndex++;
				temp = root;
				writedBytes++;
			}
			//����������,д���ļ�
			if (writeBufferArrayIndex == 512 * 1024) {
				out.write(writeBufferArray, 512 * 1024 * sizeof(char));
				writeBufferArrayIndex = 0;
			}
			if (writedBytes >= filehead.originBytes) {
				goto finish;
			}
		}
	}
	//������������д��ȥ
finish:;
	out.write(writeBufferArray, writeBufferArrayIndex * sizeof(char));
	out.close();
}