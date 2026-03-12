#ifndef CREDITCARDWIDGET_H
#define CREDITCARDWIDGET_H

#include "database/databasemanager.h"
#include "reportservices/monthlyreportservice.h"
#include <QDate>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDebug>
#include <QLocale>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>


namespace Ui { class CreditCardWidget; }

class CreditCardView : public QWidget {
    Q_OBJECT

public:
    explicit CreditCardView(QDate date, QWidget *parent = nullptr);
    ~CreditCardView();

signals:
    void backToMain();
    void goToCurrentMonth(); 

private:
    Ui::CreditCardWidget *ui;
    QSqlTableModel *accountsModel{nullptr};
    MonthlyReportService service;
    QDate initDate;

    void initView();
    void updatePieChart();
    void updateSummary();
    void updateViewByComboBox();
};

#endif