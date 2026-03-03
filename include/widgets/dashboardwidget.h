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
#include <time.h>
#include <QDateTime>

namespace Ui { class MonthView; }

class MonthView : public QWidget {
    Q_OBJECT

public:
    explicit MonthView(QDate date_selected, QWidget *parent = nullptr);
    QMap<QString, int> amountByCategoryMap;
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
    QSqlTableModel *categoryModel{nullptr};
    QSqlTableModel *incomesModel{nullptr};
    QSqlTableModel *expensesModel{nullptr};
    QSqlTableModel *accountModel{nullptr};
    QSqlRelationalTableModel *transactionModel{nullptr};

    QString month_name_;
    QDate dateViewSelected;
    int numberOfCreditCards = 0;
    int totalIncomes = 0;
    int totalExpenses = 0;
    int savings = 0;
    int maxNumberOfCreditCardsCanDisplay = 3;
    int numberOfLabelPerCreditCard = 7;
    
    QStringList labels = {"name","Used Credit","0","Available Credit","0","Limit Credit","0"};
    QVector<QLabel*> m_labels;
    QMap<QString, QLabel*> m_labelCache;

    void setAmountByCategory();
    void updateLabelsFromFilter(QSqlTableModel *model, const QString &labelPrefix);
    void updateTransactions();
    void initView();
    void updatePieChart();
    void updateSummary();
    void updateCategories();
    void updateView();
    void onAddFileButtonClicked();
    void updateCreditReview();

    QPair<QDate, QDate> monthDateRange() const;
    QString getTypeFromCategory(const QString& category);
    QString monthFilter;
 
    
};

#endif