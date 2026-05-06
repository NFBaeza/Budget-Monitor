#ifndef ADDINGFILEDIALOG_H
#define ADDINGFILEDIALOG_H

#include "database/databasemanager.h"
#include "bankFactory.h"
#include "pdfparser/bankFactory.h"
#include "pdfparser/bank.h"
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
#include <QPushButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QToolButton>

namespace Ui { class AddingFileDialog;}

class AddingFileDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddingFileDialog(QWidget *parent = nullptr);
    ~AddingFileDialog();

signals:
    void dataInserted(); 

private:
    Ui::AddingFileDialog *ui;

    QSqlTableModel *incomeModel{nullptr};
    QSqlTableModel *expenseModel{nullptr};
    QSqlTableModel *accountModel{nullptr};

    void onCancelClicked();
    void onAcceptClicked();
};

#endif