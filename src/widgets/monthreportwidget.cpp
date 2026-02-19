#include "widgets/monthreportwidget.h"
#include "./ui_monthreportwidget.h"


extern QString user_id;

MonthReport::MonthReport(QDate dateSelected, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MonthReport),
      date(dateSelected)  {

    ui->setupUi(this);

    connect(ui->backToSavingButton, &QPushButton::clicked, this, [this](){
        emit backToSavingButtonWasPressed();
    });
    connect(ui->backToMenu, &QPushButton::clicked, this, [this](){
        emit backToMenuButtonWasPressed();
    });
    

    QString month_name = QLocale().monthName(date.month());
    month_name[0] = month_name[0].toUpper();
    ui->titleMonthLabel->setText(QString("%1 %2").arg(month_name).arg(date.year()));
    ui->titleMonthLabel->setStyleSheet("font-size: 16pt");
    getTotalIncomeExpenseAmount(date);
    createPieChart();
    getExpenseBreakdown(date);

}

void MonthReport::getTotalIncomeExpenseAmount(QDate date){
    QString lastDate = QDate(date.year(), date.month(), date.daysInMonth()).toString("yyyy-MM-dd");
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT c.type, SUM(t.amount) "
                  "FROM money_transactions t "
                  "JOIN categories c ON t.category_id = c.id "
                  "WHERE t.date >= :start AND t.date <= :end AND t.user_id = :user "
                  "GROUP BY c.type");

    query.bindValue(":start", date.toString("yyyy-MM-dd"));
    query.bindValue(":end", lastDate);
    query.bindValue(":user", user_id);

    if (query.exec()) {
        while (query.next()) {
            QString type = query.value(0).toString().toLower();
            int total = query.value(1).toInt();
            if(type == "expense"){
                totalExpenses = total;
            } else{
                totalIncomes = total;
            }
        }
    } else {
        qDebug() << "[setAmountByCategory] ERROR:" << query.lastError().text();
    }

    totalSavings = (totalIncomes - totalExpenses > 0) ? totalIncomes - totalExpenses : 0;
    
    ui->totalExpensesAmount->setText(QString::number(totalExpenses));
    ui->totalIncomesAmount->setText(QString::number(totalIncomes));
    ui->totalSavingsAmount->setText(QString::number(totalSavings));
}


void MonthReport::createPieChart() {
    auto *series = new QPieSeries();

    series->append("Expenses", totalExpenses);
    series->append("Incomes", totalIncomes);
    if (totalSavings > 0) {
        QPieSlice *slice = series->append("Savings", totalSavings);
        slice->setBrush(Qt::lightGray);
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Money Distribution");
    chart->legend()->show();
    chart->legend()->setAlignment(Qt::AlignBottom);   
    chart->setTheme(QChart::ChartThemeDark);

    for (QPieSlice *slice : series->slices()) {
        if (slice->value() <= 0) {
            slice->setLabelVisible(false);
        } else {
            double porcentaje = 100 * slice->percentage();
            slice->setLabel(QString("%1: %2%").arg(slice->label()).arg(porcentaje, 0, 'f', 1));
        }
    }

    ui->graphicsView->setChart(chart);
    chart->setMargins(QMargins(6,6, 6, 6));
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

void MonthReport::getExpenseBreakdown(QDate date){
    QString lastDate = QDate(date.year(), date.month(), date.daysInMonth()).toString("yyyy-MM-dd");
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT c.name, SUM(t.amount) "
                  "FROM money_transactions t "
                  "JOIN categories c ON t.category_id = c.id "
                  "WHERE t.date >= :start AND t.date <= :end AND t.user_id = :user AND c.type = 'expense' "
                  "GROUP BY c.name");

    query.bindValue(":start", date.toString("yyyy-MM-dd"));
    query.bindValue(":end", lastDate);
    query.bindValue(":user", user_id);

    int row = 0;
    ui->tableBreakdownWidget->setColumnCount(3);
    ui->tableBreakdownWidget->setHorizontalHeaderLabels({"Category","Amount","%"});
    ui->tableBreakdownWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableBreakdownWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if (query.exec()) {
        while (query.next()) {
            ui->tableBreakdownWidget->insertRow(row);
            QString category = query.value(0).toString().toLower();
            category[0] = category[0].toUpper();
            int total = query.value(1).toInt();
            float percentOfExpenses = (total*100)/totalExpenses;

            ui->tableBreakdownWidget->setItem(row, 0, new QTableWidgetItem(category));
            ui->tableBreakdownWidget->setItem(row, 1, new QTableWidgetItem(QString::number(total)));
            ui->tableBreakdownWidget->setItem(row, 2, new QTableWidgetItem(QString::number(percentOfExpenses, 'f', 1) + "%"));  
            row++;       
        }
    } else {
        qDebug() << "[setAmountByCategory] ERROR:" << query.lastError().text();
    }
}