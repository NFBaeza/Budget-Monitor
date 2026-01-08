#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

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

namespace Ui { class MonthView; }

class MonthView : public QWidget {
    Q_OBJECT

public:
    explicit MonthView(QWidget *parent = nullptr);
    QString month_name;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlTableModel *simple_model = new QSqlTableModel(this, db);
    QSqlRelationalTableModel *relational_model = new QSqlRelationalTableModel(this, db);
    QMap<QString, int> AmountByCategoryMap;
    int totalIncomes = 0.0;
    int totalExpenses = 0.0;
    int savings =0.0;
    QString MonthFilter;
    ~MonthView();

signals:
    void backbutton_was_pressed(); 

private slots:
    void BackButtonWasPressed();
    void onAddButtonClicked();


private:
    Ui::MonthView *ui;
    void InitTransactionsView();
    void PrintTable(QAbstractItemModel *model); 
    void UpdateLabelsFromFilter(QSqlTableModel *model, const QString &filter, const QString &labelPrefix);
    void UpdateAmountByCategory();
    void InitIncomeExpensesView();
    void UpdateTransactions(bool update_view);
    void InitView();
    void InitAmount();
    void UpdatePieChart();
    void UpdateExpensesIncomesAmountView();
    void UpdateAmountView(QString category, QString type ,int amount);
    void UpdateSummary();
    void UpdateCategories();
    void UpdateAccounts();
    
    QWidget* FindWidgetByTexto(QLayout *layout, const QString &textoBuscado);
    QString GetTypeFromCategory(QString category);
    void SumAmountByCategory(QMap<QString, int> &data_by_category);
};

#endif