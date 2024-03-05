#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QFileDialog>

#include "windows.h"

namespace  {
void sendUnicode(const wchar_t data)
{
    INPUT input[4];
    HWND current_hwnd = GetForegroundWindow();
    SetFocus(current_hwnd);

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = 0;
    input[0].ki.wScan = data;
    input[0].ki.dwFlags = KEYEVENTF_UNICODE;
    input[0].ki.time = 0;
    input[0].ki.dwExtraInfo = GetMessageExtraInfo();
    SendInput(1, &input[0], sizeof(INPUT));

    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = 0;
    input[1].ki.wScan = data;
    input[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    input[1].ki.time = 0;
    input[1].ki.dwExtraInfo = GetMessageExtraInfo();
    SendInput(1, &input[1], sizeof(INPUT));
}

void sendUnicode(QString line)
{
    for(auto wc : line.toStdWString())
        sendUnicode(wc);
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    LONG exs = GetWindowLong((HWND)this->winId(),GWL_EXSTYLE);
    exs |= WS_EX_NOACTIVATE;
    exs |= WS_EX_APPWINDOW;
    exs |= WS_EX_TOPMOST;
    SetWindowLong((HWND)this->winId(),GWL_EXSTYLE,exs);
    setCentralWidget(new QWidget);

    auto *lay = new QVBoxLayout(centralWidget());

    const QString filename = QFileDialog::getOpenFileName(this);
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);
    in.setCodec("UTF-8");
    while(!in.atEnd())
    {
        const auto line = in.readLine();
        if(line.isEmpty())
        {
            lay->addWidget(new QLabel);
        }else
        {
            auto *pb = new QPushButton(line, centralWidget());
            lay->addWidget(pb);
            connect(pb, &QPushButton::clicked, this, [line] { sendUnicode(line); });
        }
    }

    setWindowTitle(QFileInfo(filename).fileName());
}

MainWindow::~MainWindow()
{
}
