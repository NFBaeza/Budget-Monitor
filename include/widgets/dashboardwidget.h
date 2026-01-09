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
    int totalIncomes = 0.0;
    int totalExpenses = 0.0;
    int savings =0.0;
    QMap<QString, int> AmountByCategoryMap;
    QString MonthFilter;
    ~MonthView();

signals:
    void backbutton_was_pressed(); 

private slots:
    void BackButtonWasPressed();
    void onAddButtonClicked();
    void OnTableRowDoubleClicked(const QModelIndex &index);
    void OnEditButtonCliked();

private:
    Ui::MonthView *ui;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlTableModel *simple_model = new QSqlTableModel(this, db);
    QSqlRelationalTableModel *relational_model = new QSqlRelationalTableModel(this, db);

    void SumAmountByCategory(QMap<QString, int> &data_by_category);
    void UpdateLabelsFromFilter(QSqlTableModel *model, const QString &filter, const QString &labelPrefix);
    void UpdateTransactions(bool update_view);
    void InitView();
    void UpdatePieChart();
    void UpdateExpensesIncomesAmountView();
    void UpdateSummary();
    void UpdateCategories();
    void UpdateAllViews();
    
    QWidget* FindWidgetByTexto(QLayout *layout, const QString &textoBuscado);
    QString GetTypeFromCategory(QString category);
};

#endif