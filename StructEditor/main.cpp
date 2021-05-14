#include <QApplication>
#include <QDir>
#include <QStandardPaths>

#include "MainWindow.h"
#include "common/defines.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    auto const &paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    auto const &localPath = paths[0];
    QDir dir {localPath};
    dir.mkpath(STRUCT_PATH);

    MainWindow w;
    w.show();
    return a.exec();
}
