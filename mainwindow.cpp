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

void sendKey(unsigned short key)
{
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

void sendUnicode(QString line)
{
    for(auto wc : line.toStdWString())
        sendUnicode(wc);
}

void sendEnter()
{
    sendKey(VK_RETURN);
}

const auto SEPARATOR = '\t';
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    LONG exs = GetWindowLong((HWND)this->winId(),GWL_EXSTYLE);
    exs |= WS_EX_NOACTIVATE;
    exs |= WS_EX_APPWINDOW;
    exs |= WS_EX_TOPMOST;
    SetWindowLong((HWND)this->winId(),GWL_EXSTYLE,exs);

    setCentralWidget(new QWidget);

    auto *lay = new QHBoxLayout(centralWidget());
    auto *innerLay = new QVBoxLayout();
    lay->addLayout(innerLay);

    auto *enterButton = new QPushButton("Enter", centralWidget());
    enterButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    lay->addWidget(enterButton);
    connect(enterButton, &QPushButton::clicked, this, [] { sendEnter(); });

    const QString filename = QFileDialog::getOpenFileName(this);
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);
    in.setCodec("UTF-8");
    while(!in.atEnd())
    {
        auto *lineLay = new QHBoxLayout();
        innerLay->addLayout(lineLay);

        const auto line = in.readLine();
        if(line.isEmpty())
        {
            lineLay->addWidget(new QLabel);
        }else
        {
            const auto lineTexts = line.split(SEPARATOR);
            for(auto text : lineTexts)
            {
                auto *pb = new QPushButton(text, centralWidget());
                lineLay->addWidget(pb);
                connect(pb, &QPushButton::clicked, this, [text] { sendUnicode(text); });
            }
        }
    }

    setWindowTitle(QFileInfo(filename).fileName());
}

MainWindow::~MainWindow()
{
}

