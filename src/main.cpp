#include "views/mainwindow.h"

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

    QString projectRoot = QString(PROJECT_SOURCE_DIR);
    QString dbPath = projectRoot + "/budget_monitor.db";

    // Fallback: si la macro no está definida, buscar en el directorio padre del ejecutable
    if (!QFile::exists(dbPath)) {
        QDir buildDir(QCoreApplication::applicationDirPath());
        buildDir.cdUp();
        dbPath = buildDir.absolutePath() + "/budget_monitor.db";
    }

    // Último fallback: buscar en el directorio actual
    if (!QFile::exists(dbPath)) {
        dbPath = "budget_monitor.db";
    }

    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Error:" << db.lastError().text();
        return false;
    }

    qDebug() << "✅ Conectado a BD en:" << QFileInfo(dbPath).absoluteFilePath();
    qDebug() << "Tablas disponibles:" << db.tables();

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