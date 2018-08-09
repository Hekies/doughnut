#include <QtGui>

#include "mainwindow.h"

int main(int argv, char **args)
{
    QApplication app(argv, args);
    app.setApplicationName("Music Player");
    app.setQuitOnLastWindowClosed(true);

    MainWindow window;
#if defined(Q_OS_SYMBIAN)
    window.showMaximized();
#else
    window.show();
#endif

    return app.exec();
}
