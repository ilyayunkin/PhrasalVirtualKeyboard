#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadFile(QString filename);
    void loadNewFile();
    void reloadFile();

private:
    QString m_filename;
};
#endif // MAINWINDOW_H
