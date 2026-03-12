#ifndef CREDITCARDWIDGET_H
#define CREDITCARDWIDGET_H

#include "database/databasemanager.h"
#include <QDate>
#include <QMessageBox>
#include <QDebug>
#include <QLocale>


namespace Ui { class CreditCardWidget; }

class CreditCardView : public QWidget {
    Q_OBJECT

public:
    explicit CreditCardView(QDate date, QWidget *parent = nullptr);
    ~CreditCardView();

signals:
    void backToMain();

private slots:


private:
    Ui::CreditCardWidget *ui;
    QSqlTableModel *accountsModel{nullptr};
    MonthlyReportService service;
    QDate initDate;

    void initView();
};

#endif