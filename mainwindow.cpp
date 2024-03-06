#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QCheckBox>

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

void sendKey(QList<unsigned short> keys)
{
    QVector<INPUT> inputs;
    inputs.resize(keys.size() * 2);
    ZeroMemory(inputs.data(), keys.size());

    for(int i = 0; i < keys.size(); ++i)
    {
        inputs[i].type = INPUT_KEYBOARD;
        inputs[i].ki.wVk = keys[i];
    }
    for(int i = 0; i < keys.size(); ++i)
    {
        inputs[keys.size() + i].type = INPUT_KEYBOARD;
        inputs[keys.size() + i].ki.wVk = keys[keys.size() - i - 1];
        inputs[keys.size() + i].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    SendInput(keys.size() * 2, inputs.data(), sizeof(INPUT));
}

void sendUnicode(QString line)
{
    for(auto wc : line.toStdWString())
        sendUnicode(wc);
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
    loadNewFile();

    QMenuBar *menubar = new QMenuBar(this);
    setMenuBar(menubar);
    QMenu *menu = menubar->addMenu("File");

    QAction *reloadAction = menu->addAction("Reload");
    QAction *loadAction = menu->addAction("Load another file");
    QAction *createNewKeyboradAction = menu->addAction("Create another keyboard");
    connect(reloadAction, &QAction::triggered, this, &MainWindow::reloadFile);
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadNewFile);
    connect(createNewKeyboradAction, &QAction::triggered, this, &MainWindow::createNewKeyboard);
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadFile(QString filename)
{
    setCentralWidget(new QWidget);

    auto *lay = new QHBoxLayout(centralWidget());
    auto *innerLay = new QVBoxLayout();
    lay->addLayout(innerLay);

    {
        auto *enterLayout = new QVBoxLayout;
        lay->addLayout(enterLayout);
        auto ctrlCheck =  new QCheckBox("CTRL+");
        auto altCheck =  new QCheckBox("ALT+");
        auto shiftCheck =  new QCheckBox("SHIFT+");

        auto *enterButton = new QPushButton("Enter", centralWidget());
        enterButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        enterLayout->addWidget(enterButton);
        enterLayout->addWidget(ctrlCheck);
        enterLayout->addWidget(altCheck);
        enterLayout->addWidget(shiftCheck);

        connect(enterButton, &QPushButton::clicked, this, [=] {
            if(ctrlCheck->isChecked())
                sendKey({VK_CONTROL, VK_RETURN});
            else if(altCheck->isChecked())
                sendKey({VK_MENU, VK_RETURN}); // ALT
            else if(shiftCheck->isChecked())
                sendKey({VK_SHIFT, VK_RETURN});
            else
                sendKey(VK_RETURN);
        });
    }

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

void MainWindow::loadNewFile()
{
    const QString filename = QFileDialog::getOpenFileName(this);

    if(filename.isEmpty())
        return;

    m_filename = filename;
    loadFile(filename);
}

void MainWindow::reloadFile()
{
    loadFile(m_filename);
}

