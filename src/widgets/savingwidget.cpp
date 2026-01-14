#include "widgets/savingwidget.h"
#include "views/mainwindow.h"
#include "./ui_savingwidget.h"
#include <time.h>
#include <QDateTime>
#include <QCoreApplication>


SavingView::SavingView(QDate month, QWidget *parent) 
    : QWidget(parent),
      ui(new Ui::SavingView),
      dateViewSelected(month) {

    ui->setupUi(this);

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    transactionsModel = DatabaseManager::instance().getTransactionsModel(this);
    incomeModel = DatabaseManager::instance().getIncomeModel(this);
    expenseModel = DatabaseManager::instance().getExpenseModel(this);
    
    connect(ui->backButton, &QPushButton::clicked, this, &SavingView::backButtonWasPressed);

    QString fechaFormateada = QString("Current Date/Time:\n%1").arg(QDateTime::currentDateTime().toString("dd-MM-yyyy  HH:mm"));
    ui->labelCurrentTime->setText(fechaFormateada);
    ui->labelMonth->setText("Savings Year Report");

    initView();

}

SavingView::~SavingView() {
    delete ui;
}

void SavingView::backButtonWasPressed(){
    emit backbutton_was_pressed();
}

void SavingView::getAmountByMonth(const QDate date){
    int number_of_transactions = transactionsModel->rowCount();
    Ui::MONEY money_distrubition = {0,0,0};

    for(int row = 0; row < number_of_transactions; row++){
        QString category_from_row = transactionsModel->record(row).value("category").toString();
        int amount_from_row = transactionsModel->record(row).value("amount").toInt();
        categoryModel->setFilter(QString("category == '%1'").arg(category_from_row));
        categoryModel->select();
        if(categoryModel->record(0).value("type").toString() == "expense"){
            money_distrubition.expenses+=amount_from_row;
        }else{
            money_distrubition.incomes+=amount_from_row;
        }
    }
    money_distrubition.saving = money_distrubition.incomes - money_distrubition.expenses;
    money_distrubition.date =  date;
    money_by_month.emplace_back(money_distrubition);
    qDebug()<<"vector last element: "<< money_by_month.back().date.toString("MM-yy") <<" "<<money_by_month.back().expenses<<" "<<money_by_month.back().incomes<<" "<<money_by_month.back().saving;
}

void SavingView::updateSummary(){
    if (money_by_month.empty()) return;

    int total_savings = 0;
    for (const auto& money : money_by_month) {
        total_savings += money.saving;
    }
    ui->labelTotalAmount->setText(QString::number(total_savings));
    ui->labelAverageAmount->setText(QString::number(total_savings / 12));

    // Lambda para comparar por saving
    auto compareBySaving = [](const Ui::MONEY& a, const Ui::MONEY& b) {
        return a.saving < b.saving;
    };

    // Max
    auto it_max = std::max_element(money_by_month.begin(), money_by_month.end(), compareBySaving);
    ui->labelHigherAmount->setText(QString::number(it_max->saving));

    // Min
    auto it_min = std::min_element(money_by_month.begin(), money_by_month.end(), compareBySaving);
    ui->labelLowerAmount->setText(QString::number(it_min->saving));

    // Ratio de crecimiento (comparando el mes más reciente con el más antiguo)
    if (money_by_month.size() >= 2) {
        int saving_reciente = money_by_month.front().saving;  // El más reciente está al inicio
        int saving_antiguo = money_by_month.back().saving;    // El más antiguo está al final

        if (saving_antiguo != 0) {
            double ratio = ((double)(saving_reciente - saving_antiguo) / std::abs(saving_antiguo)) * 100.0;
            ui->labelRatioAmount->setText(QString::number(ratio, 'f', 1) + "%");
        } else {
            ui->labelRatioAmount->setText("N/A");
        }
    }
}

void SavingView::updateBarGraph(){
    QBarSet *savingsSet = new QBarSet("Savings");
    //QBarSet *expensesSet = new QBarSet("Expenses");
    QStringList categories;

    for (auto it = money_by_month.rbegin(); it != money_by_month.rend(); ++it) {
        *savingsSet << it->saving;
        //*expensesSet << it->expenses;
        categories << it->date.toString("MMM yy");
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
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void SavingView::initView() {
    QDate initDate = dateViewSelected;
    for(int i_month = 0; i_month < 12; i_month++){
        qDebug()<<"[initView] current month: "<< initDate.month();

        QString firstDate = QDate(initDate.year(), initDate.month(), 1).toString("yyyy-MM-dd");
        QString lastDate = QDate(initDate.year(), initDate.month(), initDate.daysInMonth()).toString("yyyy-MM-dd");
        
        monthFilter = QString("money_transactions.date >= '%1' AND money_transactions.date <= '%2'").arg(firstDate).arg(lastDate);

        transactionsModel->setFilter(monthFilter);
        transactionsModel->select();
        getAmountByMonth(QDate::fromString(firstDate, "yyyy-MM-dd"));

        initDate = initDate.addMonths(-1);
    }

    updateBarGraph();
    updateSummary();
}

