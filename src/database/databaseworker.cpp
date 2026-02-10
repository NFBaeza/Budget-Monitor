#include "database/databaseworker.h"
#include "database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

const QString DatabaseWorker::CONNECTION_NAME = "worker_connection";

DatabaseWorker::DatabaseWorker(QObject *parent)
    : QObject(parent)
{
}

DatabaseWorker::~DatabaseWorker()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    QSqlDatabase::removeDatabase(CONNECTION_NAME);
}

void DatabaseWorker::initConnection()
{
    QMap<QString, QString> env = DatabaseManager::loadEnvFile();
    m_db = QSqlDatabase::addDatabase("QPSQL", CONNECTION_NAME);
    m_db.setHostName(env.value("DB_HOST"));
    m_db.setPort(env.value("DB_PORT").toInt());
    m_db.setDatabaseName(env.value("DB_NAME"));
    m_db.setUserName(env.value("DB_USER"));
    m_db.setPassword(env.value("DB_PASSWORD"));

    if (!m_db.open()) {
        qCritical() << "[DatabaseWorker] Failed to open DB connection:" << m_db.lastError().text();
        emit operationError("initConnection", m_db.lastError().text());
    } else {
        qDebug() << "[DatabaseWorker] DB connection established";
    }
}

// ==================== CATEGORY OPERATIONS ====================

void DatabaseWorker::addCategory(const QString &userId, const QString &name, const QString &type)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO categories (user_id, name, type) VALUES (:user_id, :category, :type)");
    query.bindValue(":user_id", userId);
    query.bindValue(":category", name);
    query.bindValue(":type", type);

    if (!query.exec()) {
        emit operationError("addCategory", query.lastError().text());
        return;
    }
    emit operationFinished("addCategory");
}

void DatabaseWorker::updateCategory(int id, const QString &newName)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE categories SET name = :category WHERE id = :id");
    query.bindValue(":category", newName);
    query.bindValue(":id", id);

    if (!query.exec()) {
        emit operationError("updateCategory", query.lastError().text());
        return;
    }
    emit operationFinished("updateCategory");
}

void DatabaseWorker::deleteCategory(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM categories WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        emit operationError("deleteCategory", query.lastError().text());
        return;
    }
    emit operationFinished("deleteCategory");
}

// ==================== ACCOUNT OPERATIONS ====================

void DatabaseWorker::addAccount(const QString &userId, const QString &name)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO accounts (user_id, name) VALUES (:user, :method)");
    query.bindValue(":method", name);
    query.bindValue(":user", userId);

    if (!query.exec()) {
        emit operationError("addAccount", query.lastError().text());
        return;
    }
    emit operationFinished("addAccount");
}

void DatabaseWorker::updateAccount(int id, const QString &newName)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE accounts SET name = :method WHERE id = :id");
    query.bindValue(":method", newName);
    query.bindValue(":id", id);

    if (!query.exec()) {
        emit operationError("updateAccount", query.lastError().text());
        return;
    }
    emit operationFinished("updateAccount");
}

void DatabaseWorker::deleteAccount(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM accounts WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        emit operationError("deleteAccount", query.lastError().text());
        return;
    }
    emit operationFinished("deleteAccount");
}

// ==================== TRANSACTION OPERATIONS ====================

void DatabaseWorker::insertTransaction(const QString &userId, const QString &date,
                                       int amount, int categoryId, int accountId,
                                       const QString &description)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO money_transactions (user_id, date, amount, category_id, account_id, description) "
                  "VALUES (:user_id, :date, :amount, :category, :account, :description)");
    query.bindValue(":user_id", userId);
    query.bindValue(":date", date);
    query.bindValue(":amount", amount);
    query.bindValue(":category", categoryId);
    query.bindValue(":account", accountId);
    query.bindValue(":description", description);

    if (!query.exec()) {
        emit operationError("insertTransaction", query.lastError().text());
        return;
    }
    emit operationFinished("insertTransaction");
}

void DatabaseWorker::updateTransaction(int id, const QString &userId, const QString &date,
                                       int amount, int categoryId, int accountId,
                                       const QString &description)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE money_transactions "
                  "SET user_id = :user_id, date = :date, amount = :amount, category_id = :category, "
                  "account_id = :account, description = :description "
                  "WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":user_id", userId);
    query.bindValue(":date", date);
    query.bindValue(":amount", amount);
    query.bindValue(":category", categoryId);
    query.bindValue(":account", accountId);
    query.bindValue(":description", description);

    if (!query.exec()) {
        emit operationError("updateTransaction", query.lastError().text());
        return;
    }
    emit operationFinished("updateTransaction");
}

void DatabaseWorker::deleteTransaction(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM money_transactions WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        emit operationError("deleteTransaction", query.lastError().text());
        return;
    }
    emit operationFinished("deleteTransaction");
}

// ==================== BULK IMPORT ====================

void DatabaseWorker::bulkImportTransactions(const QString &userId,
                                            const QVariantList &transactions)
{
    int total = transactions.size();
    if (total == 0) {
        emit operationError("bulkImport", "No transactions to import");
        return;
    }

    QMap<QString, int> categoryCache;
    QMap<QString, int> accountCache;

    for (int i = 0; i < total; ++i) {
        QVariantMap t = transactions[i].toMap();
        QString category = t["category"].toString();
        QString account = t["account"].toString();
        QString date = t["date"].toString();
        QString amount = t["amount"].toString();
        QString description = t["description"].toString();

        // Resolve category ID
        int categoryId;
        if (categoryCache.contains(category)) {
            categoryId = categoryCache[category];
        } else {
            QSqlQuery catQuery(m_db);
            catQuery.prepare("SELECT id FROM categories WHERE name = :category AND user_id = :user");
            catQuery.bindValue(":category", category);
            catQuery.bindValue(":user", userId);
            if (!catQuery.exec()) {
                emit operationError("bulkImport", catQuery.lastError().text());
                return;
            }
            if (catQuery.next()) {
                categoryId = catQuery.value(0).toInt();
            } else {
                QSqlQuery insertCat(m_db);
                insertCat.prepare("INSERT INTO categories (user_id, name, type) VALUES (:user, :category, :type)");
                insertCat.bindValue(":category", category);
                insertCat.bindValue(":type", (category.toLower() == "abonos") ? "income" : "expense");
                insertCat.bindValue(":user", userId);
                if (!insertCat.exec()) {
                    emit operationError("bulkImport", insertCat.lastError().text());
                    return;
                }
                categoryId = insertCat.lastInsertId().toInt();
            }
            categoryCache[category] = categoryId;
        }

        // Resolve account ID
        int accountId;
        if (accountCache.contains(account)) {
            accountId = accountCache[account];
        } else {
            QSqlQuery accQuery(m_db);
            accQuery.prepare("SELECT id FROM accounts WHERE name = :account AND user_id = :user");
            accQuery.bindValue(":account", account);
            accQuery.bindValue(":user", userId);
            if (!accQuery.exec()) {
                emit operationError("bulkImport", accQuery.lastError().text());
                return;
            }
            if (accQuery.next()) {
                accountId = accQuery.value(0).toInt();
            } else {
                QSqlQuery insertAcc(m_db);
                insertAcc.prepare("INSERT INTO accounts (user_id, name) VALUES (:user, :method)");
                insertAcc.bindValue(":method", account);
                insertAcc.bindValue(":user", userId);
                if (!insertAcc.exec()) {
                    emit operationError("bulkImport", insertAcc.lastError().text());
                    return;
                }
                accountId = insertAcc.lastInsertId().toInt();
            }
            accountCache[account] = accountId;
        }

        // Insert transaction
        QSqlQuery insertQuery(m_db);
        insertQuery.prepare("INSERT INTO money_transactions (user_id, date, amount, category_id, account_id, description) "
                            "VALUES (:user, :date, :amount, :category, :account, :description)");
        insertQuery.bindValue(":user", userId);
        insertQuery.bindValue(":date", date);
        insertQuery.bindValue(":amount", amount);
        insertQuery.bindValue(":category", categoryId);
        insertQuery.bindValue(":account", accountId);
        insertQuery.bindValue(":description", description);

        if (!insertQuery.exec()) {
            emit operationError("bulkImport", insertQuery.lastError().text());
            return;
        }

        emit bulkImportProgress(i + 1, total);
    }

    emit operationFinished("bulkImport");
}
