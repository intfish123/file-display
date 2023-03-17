#include "mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QVector>
#include <QCollator>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>



#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
//    ui->searchBtn->setShortcut(Qt::Key_Return);
    re.setPattern("[\r\n]");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void searchFileNames(const QDir &dir,bool withDir, QVector<QString> &ret)
{
    if(!dir.exists())
    {
        return;
    }
    QDir tmp(dir);
    if(!withDir)
    {
        tmp.setFilter(QDir::Files);
    }
    else
    {
        tmp.setFilter(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    }

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
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dirPath = QFileDialog::getExistingDirectory(this,"选择目录", curPath, QFileDialog::ShowDirsOnly);
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
            searchFileNames(tmp, false, fileList);

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
       searchFileNames(dir, true, fileList);
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

    QString needDelText = ui->needDelContent->toPlainText();
    for(auto i = displayContent.begin(); i != displayContent.end(); i++)
    {
        QString fname = *i;
        if(needDelText != nullptr && !needDelText.isNull() && !needDelText.isEmpty())
        {
            QStringList delLines = needDelText.split(re, Qt::SkipEmptyParts);
            for(auto j = delLines.begin(); j != delLines.end(); j++)
            {
                fname = fname.replace(*j, "");
            }

        }
        ui->textBrowser->append(fname);
    }

    if(ui->textBrowser->toPlainText() == nullptr || ui->textBrowser->toPlainText().isNull() || ui->textBrowser->toPlainText().isEmpty())
    {
        ui->textBrowser->setText("没有数据");
    }

}


void MainWindow::on_dealContent_clicked()
{
    QString content = ui->textBrowser->toPlainText();
    if(content.isNull() || content.isEmpty())
    {
        return;
    }
    QStringList lines = content.split(re, Qt::SkipEmptyParts);
    QVector<QString> newLines;
    QString keyword = ui->keyword->text();
    bool isScreenOutKeyword = ui->isScreenOutKeyword->isChecked();
    if(!keyword.isNull() && !keyword.isEmpty())
    {
        for(auto it = lines.begin(); it != lines.end(); it++)
        {
            QString fname = *it;
            if(isScreenOutKeyword) // 如果是筛除
            {
                if(!fname.contains(keyword, Qt::CaseInsensitive))
                {
                    newLines.append(fname);
                }
            }
            else // 如果是筛选
            {
                if(fname.contains(keyword, Qt::CaseInsensitive))
                {
                    newLines.append(fname);
                }
            }
        }
    }
    else
    {
        for(auto it = lines.begin(); it != lines.end(); it++)
        {
            newLines.append(*it);
        }
    }

    QString needDelText = ui->needDelContent->toPlainText();
    if(needDelText != nullptr && !needDelText.isNull() && !needDelText.isEmpty())
    {

        for(auto it = newLines.begin(); it != newLines.end(); it++)
        {
            QString fname = *it;
            QStringList delLines = needDelText.split(re, Qt::SkipEmptyParts);
            for(auto j = delLines.begin(); j != delLines.end(); j++)
            {
                *it = fname.replace(*j, "");
            }
        }
    }

    QString newContent;
    for(auto it = newLines.begin(); it != newLines.end(); it++)
    {
        newContent.append(*it);
        newContent.append("\n");
    }
    ui->textBrowser->setPlainText(newContent);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}
void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> list = event->mimeData()->urls();
    if(list.empty())
    {
        return;
    }
    QString fileName = list.first().toLocalFile();
    ui->dirPath->setText(fileName);
}
