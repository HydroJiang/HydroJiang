#include "mainwindow.h"
#include "configPage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(600,400);
    w.setWindowFlags(Qt::WindowMaximizeButtonHint); //neutralize the maximize botton, however do not work
    w.show();
    return a.exec();
}

// int main(){
//    char* sourcePath="/home/jgqj/source";
//    char* targetPath="/home/jgqj/target";
//    char* otherPath="/home/jgqj/other";

//    char* file1="hello_world.cpp";//cpp
//    char* file2="floder";//可执行文件
//    char* file3="1.txt";//docx
//    char* file4="0.png";//图片
//    string code="201214";

//    encryption(sourcePath, file4, targetPath, file4,code);
//    deEncryption(targetPath, file4, otherPath, file4,code);
//    encryption(sourcePath, file2, targetPath, file2,code);
//    deEncryption(targetPath, file2, otherPath, file2,code);

//    compress(sourcePath, file4, targetPath, file4);
//    deCompress(targetPath, file4, otherPath, file4);
//    compress(sourcePath, file2, targetPath, file2);
//    deCompress(targetPath, file2, otherPath, file2);

//}
