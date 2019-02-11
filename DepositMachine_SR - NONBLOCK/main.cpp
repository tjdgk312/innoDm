#include "mainwindow.h"
#include <QApplication>

#include <QtPlugin>
//Q_IMPORT_PLUGIN(BasicToolsPlugin)

QTranslator *translator;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOverrideCursor(Qt::ArrowCursor);

    translator = new QTranslator();
    a.installTranslator( translator );
    MainWindow w;
    w.show();

    return a.exec();
}
