#include "views/mainwindow.h"
#include "database/databasemanager.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.getDatabase().isOpen()) {
        qCritical() << "Cannot possible to connect to database";
        return -1;
    }
    qDebug() << "Connected to database";

    MainWindow w;
    w.show();

    return a.exec();
}