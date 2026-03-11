#ifndef SAVINGWIDGET_H
#define SAVINGWIDGET_H

#include "database/databasemanager.h"
#include "widgets/monthreportwidget.h"
#include "reportservices/monthlyreportservice.h"
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
#include <QVector>
#include <QLayout>
#include <QSqlRecord>
#include <QSqlQuery>


namespace Ui { 
    class SavingView;

    struct MONEY{
        MonthlyReportService::Totals totals;
        QMap<QString, MonthlyReportService::Totals> cards;
        QString date;
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
    void monthSelected(QDate date); 

private slots:
    void backButtonWasPressed();

private:
    Ui::SavingView *ui;

    QSqlTableModel *categoryModel{nullptr};
    QSqlTableModel *incomeModel{nullptr};
    QSqlTableModel *expenseModel{nullptr};
    QSqlRelationalTableModel *transactionsModel{nullptr};
    int numberOfMonthInAYear = 12;

    QVector<Ui::MONEY> moneyByMonth;

    void initView();
    void getAmountByMonth(const QString firstDate);
    void updateBarGraph();
    void updateSummary();
};

#endif