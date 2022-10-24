#include "configPage.h"
#include "ui_configPage.h"

/* 初始化设置页面 */
configPage::configPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::configPage)
{
    ui->setupUi(this);

    freshConfig(ui);

}

configPage::~configPage()
{
    delete ui;
}

/* 改变备份目录并写config，不会删除原备份目录下文件 */
void configPage::on_changeDirButton_clicked()
{
    string newPath=QFileDialog::getExistingDirectory(this,"choose dir","/home").toStdString();
    if(newPath=="")return;
    configEditor newConfig;
    struct config temp;
    strcpy(temp.backUpPath,newPath.c_str());
    temp.compress=newConfig.retIsCompress();
    temp.encryption=newConfig.retIsEncryption();
    newConfig.changeConfig(temp);
    newConfig.writeConfig();
    freshConfig(this->ui);
    emit freshMain();
}

//changeEncryption : this 2 function is too complicated to write
//void configPage::on_changeCompressButton_clicked()
//{
//    configEditor newConfig;
//    struct config temp;
//    strcpy(temp.backUpPath,newConfig.retTargetPath().c_str());
//    temp.compress=!newConfig.retIsCompress();
//    temp.encryption=newConfig.retIsEncryption();
//    newConfig.changeConfig(temp);
//    newConfig.writeConfig();
//    freshConfig(this->ui);



//    emit freshMain();
//}

//void configPage::on_changeEncryptionButton_clicked()
//{
//    configEditor newConfig;
//    struct config temp;
//    strcpy(temp.backUpPath,newConfig.retTargetPath().c_str());
//    temp.compress=newConfig.retIsCompress();
//    temp.encryption=!newConfig.retIsEncryption();
//    newConfig.changeConfig(temp);
//    newConfig.writeConfig();
//    freshConfig(this->ui);
//    emit freshMain();
//}

/* 刷新设置页面 */
void freshConfig(Ui::configPage* ui){
    configEditor config;
    string dir=config.retTargetPath();
    string isCom;
    if(config.retIsCompress()) isCom="Yes";
    else isCom="No";
    string isEn;
    if(config.retIsEncryption()) isEn="Yes";
    else isEn="No";
    ui->dirLabel->setText(QString::fromStdString(dir));
    ui->encryptionLabel->setText(QString::fromStdString(isEn));
    ui->compressLabel->setText(QString::fromStdString(isCom));
}

