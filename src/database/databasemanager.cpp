#include "database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlRelation>
#include <QDebug>

// Singleton thread-safe 
DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("budget_monitor.db");
        
        if (!db.open()) {
            qCritical() << "Error al abrir la base de datos:" << db.lastError().text();
        }
    }
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
    model->setFilter("type = 'income'");
    model->select();
    return model;
}

QSqlTableModel* DatabaseManager::getExpenseModel(QObject *parent) {
    auto model = new QSqlTableModel(parent, db);
    model->setTable("categories");
    model->setFilter("type = 'expense'");
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
    model->select();
}

void DatabaseManager::setupAccountModel(QSqlTableModel *model) {
    model->setTable("payment_methods");
    model->select();
}

void DatabaseManager::setupTransactionsModel(QSqlRelationalTableModel *model) {
    model->setTable("money_transactions");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setRelation(3, QSqlRelation("categories", "id", "category"));
    model->setRelation(4, QSqlRelation("payment_methods", "id", "method"));
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