#include <QApplication>
#include <QDir>
#include <QFontDatabase>
#include <QStandardPaths>

#include "MainWindow/MainWindow.h"
#include "common/defines.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    auto const font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    a.setFont(font);

    auto const &paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    auto const &localPath = paths[0];
    QDir dir {localPath};
    dir.mkpath(STRUCT_PATH);

    MainWindow w;
    w.show();

    return a.exec();
}
