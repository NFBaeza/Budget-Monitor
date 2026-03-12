#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVariantList>

class DatabaseWorker : public QObject {
    Q_OBJECT

public:
    explicit DatabaseWorker(QObject *parent = nullptr);
    ~DatabaseWorker();

public slots:
    void initConnection();

    // Category operations
    void addCategory(const QString &userId, const QString &name, const QString &type);
    void updateCategory(int id, const QString &newName);
    void deleteCategory(int id);

    // Account operations
    void addAccount(const QString &userId, const QString &name, const QString &type);
    void updateAccount(int id, const QString &newName);
    void deleteAccount(int id);

    // Transaction operations
    void insertTransaction(const QString &userId, const QString &date,
                           int amount, int categoryId, int accountId,
                           const QString &description);
    void updateTransaction(int id, const QString &userId, const QString &date,
                           int amount, int categoryId, int accountId,
                           const QString &description);
    void deleteTransaction(int id);

    // Bulk import from Excel
    void bulkImportTransactions(const QString &userId,
                                const QVariantList &transactions);

signals:
    void operationFinished(const QString &operationType);
    void operationError(const QString &operationType, const QString &errorMsg);
    void bulkImportProgress(int current, int total);

private:
    QSqlDatabase m_db;
    static const QString CONNECTION_NAME;
};

#endif // DATABASEWORKER_H
