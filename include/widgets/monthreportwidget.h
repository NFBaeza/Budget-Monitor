#ifndef MONTHREPORT_H
#define MONTHREPORT_H

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
#include <QSqlQuery>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>


namespace Ui { class MonthReport; }

class MonthReport : public QWidget {
    Q_OBJECT
public:
    explicit MonthReport(QDate dateSelected, QWidget *parent = nullptr);

signals:
    void backToSavingButtonWasPressed();
    void backToMenuButtonWasPressed();

private:
    Ui::MonthReport *ui;
    QDate date;

    int totalExpenses, totalIncomes, totalSavings = 0;

    void getTotalIncomeExpenseAmount(QDate date);
    void getExpenseBreakdown(QDate date);
    void createPieChart();
};

#endif