#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QAbstractItemModel>
#include <QSqlRelationalDelegate>
#include <QObject>
#include <memory>
#include <QString>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    // Singleton pattern 
    static DatabaseManager& instance();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    // Singleton pattern 

    // Methods getters
    QSqlTableModel* getCategoryModel(QObject *parent = nullptr);
    QSqlTableModel* getAccountModel(QObject *parent = nullptr);
    QSqlTableModel* getIncomeModel(QObject *parent = nullptr);
    QSqlTableModel* getExpenseModel(QObject *parent = nullptr);
    QSqlRelationalTableModel* getTransactionsModel(QObject *parent = nullptr);
    
    // Utility and debugger
    void printTable(QAbstractItemModel *model);
    
    // Query helpers (preparación para futuro)
    bool executeQuery(const QString &query);
    
    QSqlDatabase& getDatabase();

private:
    // Constructor privado para singleton
    DatabaseManager();
    ~DatabaseManager() = default;
    
    QSqlDatabase db;
    
    // Helper para configurar modelos comunes
    void setupCategoryModel(QSqlTableModel *model);
    void setupAccountModel(QSqlTableModel *model);
    void setupTransactionsModel(QSqlRelationalTableModel *model);
};

#endif // DATABASEMANAGER_H