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
    ui->searchBtn->setShortcut(Qt::Key_Return);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void searchFileNames(const QDir &dir, QVector<QString> &ret)
{
    if(!dir.exists())
    {
        return;
    }
    QDir tmp(dir);
    tmp.setFilter(QDir::Files);
    QFileInfoList list = tmp.entryInfoList();
    for(int i=0; i<list.size(); i++)
    {
        ret.append(list.at(i).fileName());
    }
    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    std::sort(ret.begin(), ret.end(), collator);
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

}


void MainWindow::on_searchBtn_clicked()
{
    ui->textBrowser->clear();
    QString dirPath = ui->dirPath->text();
    if(dirPath.isNull() || dirPath.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请选择文件夹");
        return;
    }
    QDir dir(dirPath);
    if(!dir.exists())
    {
        QMessageBox::warning(this, "提示", "文件夹不存在");
        return;
    }

    QVector<QString> fileList;

    if (ui->isRecursionFolder->isChecked())
    {
        QVector<QDir> dirQueue;
        dirQueue.append(dir);
        while(!dirQueue.empty())
        {
            QDir tmp = dirQueue.takeFirst();
            searchFileNames(tmp, fileList);

            tmp.setFilter(QDir::Dirs |QDir::NoDotAndDotDot);
            tmp.setSorting(QDir::SortFlag::DirsFirst);
            QFileInfoList list = tmp.entryInfoList();
            for(int i=0; i<list.size(); i++)
            {
                if (list.at(i).isDir())
                {
                    dirQueue.append(QDir(list.at(i).absoluteFilePath()));
                }
            }
        }
    }
    else
    {
       searchFileNames(dir, fileList);
    }

    bool withSuffix = ui->displaySuffixBtn->isChecked();
    QString keyword = ui->keyword->text();
    bool isScreenOutKeyword = ui->isScreenOutKeyword->isChecked();
    QVector<QString> displayContent;
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
        if(!keyword.isNull() && !keyword.isEmpty())
        {
            if(isScreenOutKeyword) // 如果是筛除
            {
                if(!fname.contains(keyword, Qt::CaseInsensitive))
                {
//                    ui->textBrowser->append(fname);
                    displayContent.append(fname);
                }
            }
            else // 如果是筛选
            {
                if(fname.contains(keyword, Qt::CaseInsensitive))
                {
//                    ui->textBrowser->append(fname);
                    displayContent.append(fname);
                }
            }
        }
        else{ // 如果keyword为空
//            ui->textBrowser->append(fname);
            displayContent.append(fname);
        }
    }

    QString needDelText = ui->needDelContent->text();
    for(auto i = displayContent.begin(); i != displayContent.end(); i++)
    {
        QString fname = *i;
        if(needDelText != nullptr && !needDelText.isNull() && !needDelText.isEmpty())
        {
            fname = fname.replace(needDelText, "");
        }
        ui->textBrowser->append(fname);
    }

    if(ui->textBrowser->toPlainText() == nullptr || ui->textBrowser->toPlainText().isNull() || ui->textBrowser->toPlainText().isEmpty())
    {
        ui->textBrowser->setText("没有数据");
    }

}

