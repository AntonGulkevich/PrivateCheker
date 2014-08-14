#include "cheker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Cheker w;
    w.show();

    return a.exec();
}
