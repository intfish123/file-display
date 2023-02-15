#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_chooseBtn_clicked();

    void on_searchBtn_clicked();

    void on_dealContent_clicked();

private:
    Ui::MainWindow *ui;
    QRegularExpression re;
};
#endif // MAINWINDOW_H
