#include "configPage.h"
#include "ui_configPage.h"

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

/* this won't move the back up files from old path to new path
   this just merely change back up dir. */
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

