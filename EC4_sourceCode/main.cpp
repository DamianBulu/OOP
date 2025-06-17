#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Word Ladder Game");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Word Ladder Corp");
    MainWindow window;
    window.show();

    return app.exec();
}
