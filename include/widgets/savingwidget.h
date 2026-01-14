#ifndef SAVINGWIDGET_H
#define SAVINGWIDGET_H

#include "database/databasemanager.h"
#include <iostream>
#include <QWidget>
#include <QLabel>
#include <QDate>
#include <QPushButton>
#include <QSqlError>
#include <QDebug>
#include <QLocale>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QMap>
#include <QLayout>
#include <QSqlRecord>


namespace Ui { 
    class SavingView;

    struct MONEY{
        int expenses=0;
        int incomes=0;
        int saving=0;
        QDate date;
    }; 
}

class SavingView : public QWidget {
    Q_OBJECT

public:
    explicit SavingView(QWidget *parent = nullptr);
    QDate dateViewSelected;
    QString month_name;
    QString monthFilter;
    int totalIncomes = 0;
    int totalExpenses = 0;
    int savings = 0;
    ~SavingView();

signals:
    void backbutton_was_pressed(); 

private slots:
    void backButtonWasPressed();

private:
    Ui::SavingView *ui;
    QSqlTableModel *categoryModel{nullptr};
    QSqlTableModel *incomeModel{nullptr};
    QSqlTableModel *expenseModel{nullptr};
    QSqlRelationalTableModel *transactionsModel{nullptr};
    std::vector<Ui::MONEY> money_by_month;

    void initView();
    void getAmountByMonth(const QDate date);
    void updateBarGraph();
    void updateSummary();

};

#endif