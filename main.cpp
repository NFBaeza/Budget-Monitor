#include "mainwindow.h"

#include <QApplication>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFile>

bool DataBaseConnection() {
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    // Intenta encontrarla en la carpeta del ejecutable (donde CMake la copió)
    QString path = QCoreApplication::applicationDirPath() + "/budget_monitor.db";
    
    // Si no está ahí, intenta en la carpeta raíz (donde VS Code suele iniciar)
    if (!QFile::exists(path)) {
        path = "budget_monitor.db"; 
    }

    db.setDatabaseName(path);

    if (!db.open()) {
        qDebug() << "Error:" << db.lastError().text();
        return false;
    }

    qDebug() << "Conectado a:" << QFileInfo(path).absoluteFilePath();
    qDebug() << "Tablas:" << db.tables();
    
    return true;
}

int main(int argc, char *argv[]) {
    // QApplication gestiona el ciclo de vida de la interfaz y eventos
    QApplication a(argc, argv);

    if (!DataBaseConnection()) {
        return -1;
    }

    // Instanciamos la ventana principal
    MainWindow w;
    w.show();

    // Iniciamos el bucle de eventos
    return a.exec();
}