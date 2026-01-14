#ifndef FORMDIALOG_H
#define FORMDIALOG_H

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

namespace Ui { class FormDialog; }

class FormDialog : public QDialog {
    Q_OBJECT

public:
    explicit FormDialog(QWidget *parent = nullptr);
    explicit FormDialog(int transactionId, QWidget *parent = nullptr);
    ~FormDialog();

    QString getDate() const;
    QString getAmount() const;
    QString getCategory() const;
    QString getAcount() const;
    QString getDescription() const;

signals:
    void dataInserted(); 
    void dataDeleted();

private slots:
    void onAcceptClicked();
    void onCancelClicked();
    void onDeleteClicked();

private:
    Ui::FormDialog *ui;
    QSqlDatabase& db = DatabaseManager::instance().getDatabase();
    QSqlTableModel *categoryModel{nullptr};
    QSqlTableModel *accountModel{nullptr};
    int editingTransactionId = -1;
    void loadTransactionData(int transactionId);


    void initView();
    void updateComboText();
    bool insertTransaction();
};

#endif