#ifndef LOGINSIGNINDIALOG_H
#define LOGINSIGNINDIALOG_H

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
#include <QDebug>


namespace Ui { class LogInSignInDialog; }

class LogInSignInDialog : public QDialog {
    Q_OBJECT

public:
    explicit LogInSignInDialog(QWidget *parent = nullptr);
    ~LogInSignInDialog();

private:
    Ui::LogInSignInDialog *ui;
};

#endif