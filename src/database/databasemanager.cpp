#include "database/databasemanager.h"
#include "database/databaseworker.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlRelation>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>

extern QString user_id;
// Singleton thread-safe 
DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        QMap<QString, QString> env = loadEnvFile();
        db = QSqlDatabase::addDatabase("QPSQL");
        db.setHostName(env.value("DB_HOST"));
        db.setPort(env.value("DB_PORT").toInt());
        db.setDatabaseName(env.value("DB_NAME"));
        db.setUserName(env.value("DB_USER"));
        db.setPassword(env.value("DB_PASSWORD"));

        if (!db.open()) {
            qCritical() << "Error al abrir la base de datos:" << db.lastError().text();
        }
    }

    // Set up worker thread for async DB operations
    m_worker = new DatabaseWorker();
    m_worker->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::started, m_worker, &DatabaseWorker::initConnection);
    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    m_workerThread.start();
}

DatabaseManager::~DatabaseManager() {
    m_workerThread.quit();
    m_workerThread.wait();
}

QMap<QString, QString> DatabaseManager::loadEnvFile() {
    QMap<QString, QString> env;
    QFile file(QCoreApplication::applicationDirPath() + "/../.env");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << ".env file not found:" << file.fileName();
        return env;
    }
    while (!file.atEnd()) {
        QString line = file.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;
        int sep = line.indexOf('=');
        if (sep > 0) {
            env.insert(line.left(sep), line.mid(sep + 1));
        }
    }
    return env;
}

QSqlTableModel* DatabaseManager::getCategoryModel(QObject *parent) {
    auto model = new QSqlTableModel(parent, db);
    setupCategoryModel(model);
    return model;
}

QSqlTableModel* DatabaseManager::getAccountModel(QObject *parent) {
    auto model = new QSqlTableModel(parent, db);
    setupAccountModel(model);
    return model;
}

QSqlTableModel* DatabaseManager::getIncomeModel(QObject *parent) {
    auto model = new QSqlTableModel(parent, db);
    model->setTable("categories");
    model->setFilter(QString("type = 'income' AND user_id = '%1'").arg(user_id));
    model->select();
    return model;
}

QSqlTableModel* DatabaseManager::getExpenseModel(QObject *parent) {
    auto model = new QSqlTableModel(parent, db);
    model->setTable("categories");
    model->setFilter(QString("type = 'expense' AND user_id = '%1'").arg(user_id));
    model->select();
    return model;
}

QSqlRelationalTableModel* DatabaseManager::getTransactionsModel(QObject *parent) {
    auto model = new QSqlRelationalTableModel(parent, db);
    setupTransactionsModel(model);
    return model;
}

void DatabaseManager::setupCategoryModel(QSqlTableModel *model) {
    model->setTable("categories");
    model->setFilter(QString("user_id = '%1'").arg(user_id));
    model->select();
}

void DatabaseManager::setupAccountModel(QSqlTableModel *model) {
    model->setTable("accounts");
    model->setFilter(QString("user_id = '%1'").arg(user_id));
    model->select();
}

void DatabaseManager::setupTransactionsModel(QSqlRelationalTableModel *model) {
    model->setTable("money_transactions");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setRelation(4, QSqlRelation("categories", "id", "name"));
    model->setRelation(5, QSqlRelation("accounts", "id", "name"));
    model->setFilter(QString("user_id = '%1'").arg(user_id));
    model->select();
}

void DatabaseManager::printTable(QAbstractItemModel *model) {
    if (!model) {
        qDebug() << "The model is null.";
        return;
    }

    int rows = model->rowCount();
    int cols = model->columnCount();

    qDebug() << "--- Printing Table ---";
    qDebug() << "Rows:" << rows << "| Columns:" << cols;

    // Print Headers
    QString headerLine = "| ";
    for (int c = 0; c < cols; ++c) {
        headerLine += model->headerData(c, Qt::Horizontal).toString() + " | ";
    }
    qDebug() << headerLine;
    qDebug() << QString("-").repeated(headerLine.length());

    // Print Data
    for (int r = 0; r < rows; ++r) {
        QString rowLine = "| ";
        for (int c = 0; c < cols; ++c) {
            rowLine += model->data(model->index(r, c)).toString() + " | ";
        }
        qDebug() << rowLine;
    }
    qDebug() << "--- End of Table ---";
}

bool DatabaseManager::executeQuery(const QString &query) {
    QSqlQuery sqlQuery(db);
    if (!sqlQuery.exec(query)) {
        qWarning() << "Query failed:" << sqlQuery.lastError().text();
        return false;
    }
    return true;
}

QSqlDatabase& DatabaseManager::getDatabase() {
    return db;
}

DatabaseWorker* DatabaseManager::worker() const {
    return m_worker;
}