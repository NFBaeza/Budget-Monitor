#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include "database/databasemanager.h"
#include "reportServices/monthlyreportservice.h"
#include <QWidget>
#include <QLabel>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QDebug>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QLocale>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QMap>
#include <time.h>
#include <QDateTime>

namespace Ui { class MonthView; }

class MonthView : public QWidget {
    Q_OBJECT

public:
    explicit MonthView(QDate dateSelected, QWidget *parent = nullptr);
    ~MonthView();

signals:
    void backbutton_was_pressed(); 

private slots:
    void backButtonWasPressed();
    void onAddButtonClicked();
    void onTableRowDoubleClicked(const QModelIndex &index);
    void onEditButtonClicked();

private:
    Ui::MonthView *ui;
    MonthlyReportService ReportService;
    QSqlRelationalTableModel *transactionModel{nullptr};
    
    QPair<QDate, QDate> monthDateRange() const;
    QString monthFilter;
    QString monthName;
    QDate dateViewSelected;

    int numberOfCreditCards = 0;
    int maxNumberOfCreditCardsCanDisplay = 3;
    int numberOfLabelPerCreditCard = 7;
    
    MonthlyReportService::Totals totals;
    QMap<QString, int> amountByCategoryMap;

    QStringList labels = {"name","Used Credit","0","Available Credit","0","Limit Credit","0"};
    QVector<QLabel*> m_labels;
    QMap<QString, QLabel*> m_labelCache;

    void updateLabelsFromFilter(const QString type);
    void updateTransactions();
    void initView();
    void updatePieChart();
    void updateSummary();
    void updateCategories();
    void updateView();
    void onAddFileButtonClicked();
    void updateCreditReview();
};

#endif