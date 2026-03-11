#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

class MockDatabase {
public:
    // Use ":memory:" for throwaway, or a file path to persist
    static QSqlDatabase create(const QString &connName = "mock_conn",
                               const QString &path = ":memory:") {
        auto db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(path);

        if (!db.open()) {
            qCritical() << "Failed to open mock DB";
            return db;
        }

        seed(db);
        return db;
    }

private:
    static void seed(QSqlDatabase &db) {
        QSqlQuery q(db);

        // --- Schema ---
        q.exec("CREATE TABLE categories ("
               "  id INTEGER PRIMARY KEY,"
               "  user_id TEXT,"
               "  name TEXT,"
               "  type TEXT"
               ")");

        q.exec("CREATE TABLE accounts ("
               "  id INTEGER PRIMARY KEY,"
               "  user_id TEXT,"
               "  name TEXT,"
               "  type TEXT"
               ")");

        q.exec("CREATE TABLE transactions ("
               "  id INTEGER PRIMARY KEY,"
               "  user_id TEXT,"
               "  date TEXT,"
               "  amount INTEGER,"
               "  category_id INTEGER REFERENCES categories(id),"
               "  account_id INTEGER REFERENCES accounts(id),"
               "  description TEXT"
               ")");

        // --- Categories ---
        q.exec("INSERT INTO categories VALUES (1, 'u1', 'Food',        'expense')");
        q.exec("INSERT INTO categories VALUES (2, 'u1', 'Salary',      'income')");
        q.exec("INSERT INTO categories VALUES (3, 'u1', 'Transport',   'expense')");
        q.exec("INSERT INTO categories VALUES (4, 'u1', 'Freelance',   'income')");
        q.exec("INSERT INTO categories VALUES (5, 'u1', 'Entertainment','expense')");
        q.exec("INSERT INTO categories VALUES (6, 'u1', 'Utilities',   'expense')");

        // --- Accounts ---
        q.exec("INSERT INTO accounts VALUES (1, 'u1', 'Checking', 'debit')");
        q.exec("INSERT INTO accounts VALUES (2, 'u1', 'Savings', 'debit')");
        q.exec("INSERT INTO accounts VALUES (3, 'u1', 'Credit Card', 'credit')");

        // --- Transactions (3 months of realistic data) ---
        QStringList txns = {
            // January
            "(1,'u1','2026-01-05',120000,2,1,'January salary')",
            "(2,'u1','2026-01-07',35000,1,1,'Supermarket')",
            "(3,'u1','2026-01-10',8000,3,3,'Metro card')",
            "(4,'u1','2026-01-15',15000,5,3,'Cinema + dinner')",
            "(5,'u1','2026-01-20',42000,6,1,'Electricity + water')",
            "(6,'u1','2026-01-25',28000,1,1,'Groceries')",
            // February
            "(7,'u1','2026-02-05',120000,2,1,'February salary')",
            "(8,'u1','2026-02-06',25000,4,2,'Side project payment')",
            "(9,'u1','2026-02-08',31000,1,1,'Supermarket')",
            "(10,'u1','2026-02-12',9500,3,3,'Bus pass')",
            "(11,'u1','2026-02-14',22000,5,3,'Valentine dinner')",
            "(12,'u1','2026-02-20',38000,6,1,'Utilities')",
            "(13,'u1','2026-02-28',19000,1,3,'Restaurant')",
            // March
            "(14,'u1','2026-03-05',120000,2,1,'March salary')",
            "(15,'u1','2026-03-07',33000,1,1,'Supermarket')",
            "(16,'u1','2026-03-09',8500,3,3,'Metro card')",
            "(17,'u1','2026-03-12',50000,4,2,'Freelance invoice')",
            "(18,'u1','2026-03-15',12000,5,3,'Concert tickets')",
            "(19,'u1','2026-03-18',27000,1,1,'Groceries')",
        };

        for (const auto &row : txns) {
            q.exec("INSERT INTO transactions VALUES " + row);
        }

        qDebug() << "Mock DB seeded:"
                 << "6 categories, 3 accounts,"
                 << txns.size() << "transactions";
    }
};