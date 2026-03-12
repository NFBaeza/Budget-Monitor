#ifndef     ACCOUNTDIALOG_H
#define     ACCOUNTDIALOG_H

#include "database/databasemanager.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QDebug>
#include <QInputDialog>
#include <QListView>
#include <QToolButton>

namespace Ui { class AccountDialog; }

class AccountManager : public QDialog {
    Q_OBJECT

public:
    explicit AccountManager(QWidget *parent = nullptr);
    ~AccountManager();

signals:
    void dataUpdated();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onAddAccountClicked();
    void onAccountDoubleClicked(const QModelIndex &index);

private:
    Ui::AccountDialog *ui;

    QSqlTableModel *accountsModel{nullptr};

    void initView();
    void loadAccounts();
    void setButtonsEnabled(bool enabled);
};

#endif
