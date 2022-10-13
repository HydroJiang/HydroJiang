#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <windows.h>

void listFiles(const char * dir)
{
	using namespace std;

	HANDLE hFind;
	WIN32_FIND_DATA findData;
	LARGE_INTEGER size;
	char dirNew[100];

	// 向目录加通配符，用于搜索第一个文件 
	strcpy(dirNew, dir);
	strcat(dirNew, "\\*.*");

	hFind = FindFirstFile(dirNew, &findData);
	do
	{
		// 是否是文件夹，并且名称不为"."或".." 
		if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
			&& strcmp(findData.cFileName, ".") != 0
			&& strcmp(findData.cFileName, "..") != 0
			)
		{
			// 将dirNew设置为搜索到的目录，并进行下一轮搜索 
			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.cFileName);
			listFiles(dirNew);
		}
		else if(strcmp(findData.cFileName, ".") == 0
			|| strcmp(findData.cFileName, "..") == 0)
		{
			size.LowPart = findData.nFileSizeLow;
			size.HighPart = findData.nFileSizeHigh;
			cout << findData.cFileName << "\t" << size.QuadPart << " bytes\n";
		}
		else {
			size.LowPart = findData.nFileSizeLow;
			size.HighPart = findData.nFileSizeHigh;
			cout << findData.cFileName << "\t" << size.QuadPart << " bytes\n";
		}
	} while (FindNextFile(hFind, &findData));

	FindClose(hFind);
}
std::size_t fileSize(const char* filePath) {
	FILE* fin = fopen(filePath, "r");
	if (fin == nullptr) {
		std::cout << "open " << filePath << " fail" << std::endl;
		exit(0);
	}

	fseek(fin, 0L, SEEK_END);
	std::size_t fileSize = ftell(fin);
	fclose(fin);
	return fileSize;
}

void writeFile(unsigned char* buffer, std::size_t size, const char* filePath) {
	FILE* fout = fopen(filePath, "wb");  // w for write, b for binary
	if (fout == nullptr) {
		std::cout << "open " << filePath << " fail" << std::endl;
		return;
	}

	fwrite(buffer, sizeof(unsigned char), size, fout);
}

void readFile(unsigned char* buffer, std::size_t size, const char* filePath) {
	FILE* fin = fopen(filePath, "rb");  // r for read, b for binary
	if (fin == nullptr) {
		std::cout << "open " << filePath << " fail" << std::endl;
		return;
	}

	fread(buffer, sizeof(unsigned char), size, fin); // read sizeof(buffer) elements to our buffer
}
int main()
{
	std::size_t size = fileSize("C:\\Users\\谪仙\\Desktop\\1\\1.docx");
	std::cout << size << std::endl;
	unsigned char buffer[1000000];
	readFile(buffer, size, "C:\\Users\\谪仙\\Desktop\\1\\1.docx");
	writeFile(buffer, size, "C:\\Users\\谪仙\\Desktop\\2\\1.docx");
	return 0;
	
	using namespace std;

	char dir[100];
	cout << "Enter a directory (do not add \'\\\' in the end): ";
	cin.getline(dir, 100);

	listFiles(dir);
	return 0;
	
}