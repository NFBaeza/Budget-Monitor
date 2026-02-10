#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include "database/databasemanager.h"
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
#include <QDateTime>

namespace Ui { class MonthView; }

class MonthView : public QWidget {
    Q_OBJECT

public:
    explicit MonthView(QDate date_selected, QWidget *parent = nullptr);
    QString month_name_;
    QDate dateViewSelected;
    int totalIncomes = 0.0;
    int totalExpenses = 0.0;
    int savings =0.0;
    QMap<QString, int> amountByCategoryMap;
    QString MonthFilter;
    ~MonthView();

signals:
    void backbutton_was_pressed(); 

private slots:
    void backButtonWasPressed();
    void onAddButtonClicked();
    void onTableRowDoubleClicked(const QModelIndex &index);
    void onEditButtonCliked();

private:
    Ui::MonthView *ui;
    QSqlTableModel *categoryModel{nullptr};
    QSqlTableModel *incomesModel{nullptr};
    QSqlTableModel *expensesModel{nullptr};
    QSqlRelationalTableModel *transactionModel{nullptr};

    void setAmountByCategory();
    void updateLabelsFromFilter(QSqlTableModel *model, const QString &labelPrefix);
    void updateTransactions();
    void initView();
    void updatePieChart();
    void updateSummary();
    void updateCategories();
    void updateView();
    void onAddFileButtonCliked();

    QString getTypeFromCategory(const QString& category);
};

#endif