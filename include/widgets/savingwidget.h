#ifndef SAVINGWIDGET_H
#define SAVINGWIDGET_H

#include "reportservices/monthlyreportservice.h"
#include <QWidget>
#include <QDate>
#include <QDebug>
#include <QLocale>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QVector>

namespace Ui { class SavingView; }

class SavingView : public QWidget {
    Q_OBJECT

public:
    explicit SavingView(QWidget *parent = nullptr);
    ~SavingView();

signals:
    void backbutton_was_pressed();
    void monthSelected(QDate date);

private slots:
    void backButtonWasPressed();

private:
    struct MonthData {
        QDate                       date;
        MonthlyReportService::Totals totals;
    };

    Ui::SavingView *ui;

    static constexpr int MAX_MONTHS = 12;
    int displayMonths = MAX_MONTHS;
    QVector<MonthData> m_monthData;

    void precomputeData();
    void refreshView();
    void updateSummary();
    void updateBarGraph();
};

#endif