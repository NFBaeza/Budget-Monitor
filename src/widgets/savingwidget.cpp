#include "widgets/savingwidget.h"
#include "views/mainwindow.h"
#include "./ui_savingwidget.h"
#include <time.h>
#include <QDateTime>
#include <QCoreApplication>

extern QString user_id;

SavingView::SavingView(QWidget *parent) 
    : QWidget(parent),
      ui(new Ui::SavingView) {

    ui->setupUi(this);
    this->setFixedSize(1080, 650);

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    transactionsModel = DatabaseManager::instance().getTransactionsModel(this);
    incomeModel = DatabaseManager::instance().getIncomeModel(this);
    expenseModel = DatabaseManager::instance().getExpenseModel(this);

    ui->titleLabel->setStyleSheet("font-size: 16pt");
    
    connect(ui->backButton, &QPushButton::clicked, this, &SavingView::backButtonWasPressed);
    
    initView();
}

SavingView::~SavingView() {
    delete ui;
}

void SavingView::backButtonWasPressed(){
    emit backbutton_was_pressed();
}

void SavingView::getAmountByMonth(const QString firstDate, const QString lastDate){
    Ui::MONEY money_distrubition;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT c.type, SUM(t.amount) "
                  "FROM money_transactions t "
                  "JOIN categories c ON t.category_id = c.id "
                  "WHERE t.date >= :start AND t.date <= :end AND t.user_id = :user "
                  "GROUP BY c.type");

    query.bindValue(":start", firstDate);
    query.bindValue(":end", lastDate);
    query.bindValue(":user", user_id);

    if (query.exec()) {
        while (query.next()) {
            QString type = query.value(0).toString().toLower();
            int total = query.value(1).toInt();
            if(type == "expense"){
                money_distrubition.expenses = total;
            } else{
                money_distrubition.incomes = total;
            }
        }
    } else {
        qDebug() << "[setAmountByCategory] ERROR:" << query.lastError().text();
    }

    money_distrubition.saving = money_distrubition.incomes - money_distrubition.expenses;
    money_distrubition.date = firstDate;
    money_by_month.emplace_back(money_distrubition);
}

void SavingView::updateSummary(){
    if (money_by_month.empty()) return;

    int total_savings = 0;
    for (const auto& money : money_by_month) {
        total_savings += money.saving;
    }

    if(total_savings < 0){
        ui->labelTotalAmount->setText("0");
    }else{
        ui->labelTotalAmount->setText(QString::number(total_savings));
    }

    ui->labelAverageAmount->setText(QString::number(total_savings / 12));

    auto compareBySaving = [](const Ui::MONEY& a, const Ui::MONEY& b) {
        return a.saving < b.saving;
    };

    auto it_max = std::max_element(money_by_month.begin(), money_by_month.end(), compareBySaving);
    ui->labelHigherAmount->setText(QString::number((it_max->saving < 0) ? 0 : (it_max->saving)));

    auto it_min = std::min_element(money_by_month.begin(), money_by_month.end(), compareBySaving);
    ui->labelLowerAmount->setText(QString::number((it_min->saving < 0) ? 0 : (it_min->saving)));

    int total_incomes = 0;
    for(const auto &money: money_by_month){
        total_incomes+=money.incomes;
    }
 
    double ratio = ((double)(total_savings) / std::abs(total_incomes)) * 100.0;
    ui->labelRatioAmount->setText(QString::number(ratio, 'f', 1) + "%");

    if(ratio < 0){
        ui->labelRatioAmount->setStyleSheet("color: red;");
    }else if(ratio > 0 && ratio < 20){
        ui->labelRatioAmount->setStyleSheet("color: yellow;");
    }else{
        ui->labelRatioAmount->setStyleSheet("color: green;");
    }

}

void SavingView::updateBarGraph(){
    QBarSet *savingsSet = new QBarSet("Savings");
    //QBarSet *expensesSet = new QBarSet("Expenses");
    QStringList monthsList;

    for (auto it = money_by_month.rbegin(); it != money_by_month.rend(); ++it) {
        *savingsSet << it->saving;
        //*expensesSet << it->expenses;
        monthsList << QDate::fromString(it->date, "yyyy-MM-dd").toString("MM-yy");
    }

    QBarSeries *series = new QBarSeries();
    series->append(savingsSet);
    //series->append(expensesSet);

    QChart *chart = new QChart();
    savingsSet->setBrush(QBrush(QColor("#a0eb74cc")));
    chart->addSeries(series);
    chart->setTitle("Monthly Savings");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(monthsList);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    chart->setTheme(QChart::ChartThemeDark);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    connect(series, &QBarSeries::doubleClicked, this, [this, axisX](int index, QBarSet *) {
        QDate month = QDate::fromString("01-" + axisX->at(index), "dd-MM-yy", 2000);
        emit monthSelected(month);
    });

}


void SavingView::initView() {
    
    QString fechaFormateada = QString("Current Date/Time:\n%1").arg(QDateTime::currentDateTime().toString("dd-MM-yyyy  HH:mm"));
    ui->labelCurrentTime->setText(fechaFormateada);

    QDate initDate = QDate::currentDate();
    for(int i_month = 0; i_month < 12; i_month++){

        QString firstDate = QDate(initDate.year(), initDate.month(), 1).toString("yyyy-MM-dd");
        QString lastDate = QDate(initDate.year(), initDate.month(), initDate.daysInMonth()).toString("yyyy-MM-dd");
        
        transactionsModel->select();

        getAmountByMonth(firstDate, lastDate);

        initDate = initDate.addMonths(-1);
    }

    updateBarGraph();
    updateSummary();
}

