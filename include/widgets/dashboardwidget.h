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
    ~MonthView();

signals:
    void backbutton_was_pressed(); 

private slots:
    void BackButtonWasPressed();

private:
    Ui::MonthView *ui;
    void InitTransactionsView();
    void PrintTable(QAbstractItemModel *model); 
    void UpdateLabelsFromFilter(QSqlTableModel *model, const QString &filter, const QString &labelPrefix);
    void UpdateAmountByCategory();
    void InitIncomeExpensesView();
    void InitView();
    void InitAmount();
    void UpdatePieChart();
    void UpdateAmountView(QString category, QString type ,int amount);
    void UpdateSummary(bool update_view);

    QWidget* FindWidgetByTexto(QLayout *layout, const QString &textoBuscado);
    QString GetTypeFromCategory(QString category);

    template <typename T>
    void SumAmountByCategory(T &data_by_category) {
        QString originalFilter = relational_model->filter();
        
        int max_row = simple_model->rowCount();
        
        for (int i = 0; i < max_row; ++i) {
            QString category_idx  = simple_model->index(i,0).data().toString();
            QString category_name = simple_model->index(i,1).data().toString();    
            QString category_type = simple_model->index(i,2).data().toString();  

            QString filter = QString("money_transactions.category = '%1'").arg(category_idx);
            relational_model->setFilter(filter);

            if(relational_model->select()){
                for(int j = 0; j < relational_model->rowCount(); j++){
                    int amount = relational_model->data(relational_model->index(j, 2)).toInt();
                    data_by_category[category_name] += amount; 
                }
            }
        }
        
        relational_model->setFilter(originalFilter);
        relational_model->select();
    }
};

#endif