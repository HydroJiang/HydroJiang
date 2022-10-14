#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>
#include<vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <unistd.h>

using namespace std;

#define MAX_PATH 100    //工作路径名最大长度
#define LINE_SIZE 128   //每次拷贝最大字节数

string recordPath;
int len;

/* 将字符串s，以c为分隔符，分为部分塞到向量v中 */
void split(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	size_t len = s.length();
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.emplace_back(s.substr(pos1, pos2 - pos1));
 
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != len)
		v.emplace_back(s.substr(pos1));
}

/* 检查路径上的每一个文件夹是否存在，不存在则创建文件夹，用于还原备份，还原备份之前先创建路径 */
void createDirList(const char* sourcePath){
    vector<string> dir;
    split(sourcePath,dir,"/");
    string path=dir[0];
    for(int i=1 ;i<dir.size();i++){
        string temp=path+'/'+dir[i];
        cout<<"now with : "<<temp<<endl;
        if(access(temp.c_str(),0)){
            cout<<"mkdir "<<temp<<endl;
            mkdir(temp.c_str(),16877);
        }
        path=path+'/'+dir[i];
    }
}

void changeStat(const char* targetFile, const struct stat &s){
    int flag;
    flag=chmod(targetFile,s.st_mode);//同步权限
    if(!flag) cout<<targetFile<<" chmod success!"<<endl;
    flag=utimensat(0,targetFile,&(s.st_atim),0);//同步时间
    if(!flag) cout<<targetFile<<" change time success!"<<endl;
    
}

class Record{
public:
    string sourcePath;
    string fileName;
    struct stat s;
    Record* next;
    Record(){
        char workPath[MAX_PATH];  
        getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
        recordPath=workPath;
        recordPath=recordPath+"/record";
        next=NULL;
    }
    /* 从本地record文件中获取记录信息 */
    void getRecord(){
        char tmp[LINE_SIZE];
        Record* node=this;
        ifstream file;
        file.open(recordPath,ios::in);
        file.getline(tmp,LINE_SIZE);
        len=atoi(tmp);
        for(int i=0;i<len;i++){
            vector<string> v;
            file.getline(tmp,LINE_SIZE);
            split(tmp,v," ");
            node->sourcePath=v[0];
            node->fileName=v[1];
            node->next=new Record;
            node=node->next;
        }
        delete node;//删除最后一个多余节点
    }
    /* 全部输出 */
    void coutRecord(){
        Record* node=this;
        for(int i=0;i<len;i++){
            cout<<node->sourcePath<<'/'<<node->fileName<<endl;
            node=node->next;
        }
    }
    /* 搜索是否在其中，返回在链表中位置 */
    int isInRecord(const char* sourcePath,const char* fileName){
        for(int i=0;i<len;i++){

        }
    }
};

// int main(){
//     Record header;
//     header.getRecord();
//     header.coutRecord();
// }