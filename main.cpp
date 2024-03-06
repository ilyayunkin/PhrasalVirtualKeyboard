#include "mainwindow.h"

#include <QApplication>
#include <QSharedPointer>
#include <QList>
#include <functional>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QList<QSharedPointer<MainWindow>> keyboards;
    std::function<void(void)> keyboardCreator;
    keyboardCreator = [&keyboards, &keyboardCreator]{
        MainWindow *w = new MainWindow;
        keyboards.append(QSharedPointer<MainWindow>(w));
        w->show();
        QObject::connect(w, &MainWindow::createNewKeyboard, keyboardCreator);
    };
    keyboardCreator();

    return a.exec();
}
