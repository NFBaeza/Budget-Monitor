#ifndef CATEGORYDIALOG_H
#define CATEGORYDIALOG_H

#include "database/databasemanager.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelation>
#include <QRadioButton>
#include <QDebug>
#include <QPushButton>
#include <QInputDialog>
#include <QListView>
#include <QToolButton>

namespace Ui { class CategoryDialog; }

class CategoryDialog : public QDialog {
    Q_OBJECT

public:
    explicit CategoryDialog(QWidget *parent = nullptr);
    ~CategoryDialog();

signals:
    void dataUpdated();

private slots:
    void onSaveClicked();
    void onCancelClicked();

    // Income category actions
    void onAddIncomeClicked();
    void onDeleteIncomeClicked();
    void onIncomeDoubleClicked(const QModelIndex &index);

    // Expense category actions
    void onAddExpenseClicked();
    void onDeleteExpenseClicked();
    void onExpenseDoubleClicked(const QModelIndex &index);

    // Account actions
    void onAddAccountClicked();
    void onDeleteAccountClicked();
    void onAccountDoubleClicked(const QModelIndex &index);

private:
    Ui::CategoryDialog *ui;
    QSqlDatabase& db = DatabaseManager::instance().getDatabase();

    QSqlTableModel *incomeModel{nullptr};
    QSqlTableModel *expenseModel{nullptr};
    QSqlTableModel *accountModel{nullptr};

    void initView();
    void loadCategories();
    void loadAccounts();

    bool addCategory(const QString &categoryName, const QString &type);
    bool updateCategory(int id, const QString &newName);
    bool deleteCategory(int id);

    bool addAccount(const QString &accountName);
    bool updateAccount(int id, const QString &newName);
    bool deleteAccount(int id);
};

#endif