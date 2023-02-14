#include "mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QVector>
#include <QCollator>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_chooseBtn_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this,"选择目录", "./", QFileDialog::ShowDirsOnly);
    QDir dir(dirPath);
    if(!dir.exists())
    {
        QMessageBox::warning(this, "提示", "文件夹不存在");
        return;
    }
    ui->dirPath->setText(dirPath);
    ui->textBrowser->clear();
    dir.setFilter(QDir::Files);
    QFileInfoList list = dir.entryInfoList();

    QVector<QString> fileList;

    bool withSuffix = ui->displaySuffixBtn->isChecked();

    for(int i=0; i<list.size(); i++)
    {
        fileList.append(list.at(i).fileName());
    }

    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);

    std::sort(fileList.begin(), fileList.end(), collator);

    for(auto i = fileList.begin(); i != fileList.end(); i++)
    {
        QString fname = *i;
        if(!withSuffix)
        {
            int dot = fname.lastIndexOf(".");
            if(dot != -1)
            {
                fname = fname.left(dot);
            }
        }
        ui->textBrowser->append(fname);
    }

}

