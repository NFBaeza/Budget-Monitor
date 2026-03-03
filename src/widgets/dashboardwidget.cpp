#include "widgets/dashboardwidget.h"
#include "views/mainwindow.h"
#include "./ui_dashboardwidget.h"
#include "dialogs/formdialog.h"
#include "dialogs/categorydialog.h"
#include "dialogs/addingfiledialog.h"

extern QString user_id;
static QLocale s_locale(QLocale::German);

MonthView::MonthView(QDate date_selected, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonthView),
    dateViewSelected(date_selected) {
    
    ui->setupUi(this);
    this->setMinimumSize(1080, 650);
    ui->typeAccountView->setCurrentIndex(0);

    month_name_ = QLocale().monthName(dateViewSelected.month());
    month_name_[0] = month_name_[0].toUpper();
    ui->MonthNameLabel->setText(QString("%1 %2").arg(month_name_).arg(dateViewSelected.year()));

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    incomesModel = DatabaseManager::instance().getIncomeModel(this);
    expensesModel = DatabaseManager::instance().getExpenseModel(this);
    accountModel = DatabaseManager::instance().getAccountModel(this);
    transactionModel = DatabaseManager::instance().getTransactionsModel(this);
    
    connect(ui->BackButton, &QPushButton::clicked, this, &MonthView::backButtonWasPressed);
    connect(ui->AddEntryButton, &QPushButton::clicked, this, &MonthView::onAddButtonClicked);
    connect(ui->TableViewLastEntry, &QTableView::doubleClicked, this, &MonthView::onTableRowDoubleClicked);
    connect(ui->EditCategoryButton, &QPushButton::clicked, this, &MonthView::onEditButtonClicked);
    connect(ui->addFileButton, &QPushButton::clicked, this, &MonthView::onAddFileButtonClicked);

    initView();
}

MonthView::~MonthView() {
    delete ui;
}

void MonthView::backButtonWasPressed(){
    emit backbutton_was_pressed();

}

void MonthView::onAddButtonClicked() {
    FormDialog dialog(this);

    connect(&dialog, &FormDialog::dataInserted, this, [this]() {
        qDebug() << "[onAddButtonClicked] Transaction inserted, refreshing...";
        updateView();
    });

    dialog.exec();
}

void MonthView::onAddFileButtonClicked() {
    AddingFileDialog dialog(this);

    connect(&dialog, &AddingFileDialog::dataInserted, this, [this]() {
        qDebug() << "[onAddFileButtonClicked] File inserted, refreshing...";
        updateView();
    });

    dialog.exec();
}

void MonthView::onEditButtonClicked() {
    CategoryDialog dialog(this);

    connect(&dialog, &CategoryDialog::dataUpdated, this, [this]() {
        qDebug() << "[onEditButtonClicked] Categories Updated, refreshing...";
        updateView();
    });

    dialog.exec();
}

void MonthView::onTableRowDoubleClicked(const QModelIndex &index) {
    int row = index.row();

    int transactionId = transactionModel->data(
        transactionModel->index(row, 0)).toInt();

    FormDialog dialog(transactionId, this);

    connect(&dialog, &FormDialog::dataDeleted, this, [this]() {
        qDebug() << "[OnTableRowDoubleClicked] Transaction deleted, refreshing...";
        updateView();
    });

    connect(&dialog, &FormDialog::dataInserted, this, [this]() {
        qDebug() << "[OnTableRowDoubleClicked] Transaction inserted, refreshing...";
        updateView();
    });

     connect(&dialog, &FormDialog::dataUpdated, this, [this]() {
        qDebug() << "[OnTableRowDoubleClicked] Transaction updated, refreshing...";
        updateView();
    });

    dialog.exec();
}

void  MonthView::updateTransactions(){
    ui->TableViewLastEntry->resizeColumnsToContents();
    ui->TableViewLastEntry->horizontalHeader()->setStretchLastSection(true);
    ui->TableViewLastEntry->setColumnHidden(0,true);
    ui->TableViewLastEntry->setColumnHidden(1,true);
    
    ui->TableViewLastEntry->setColumnHidden(numberOfLabelPerCreditCard,true);
    ui->TableViewLastEntry->setColumnHidden(8,true);
    ui->TableViewLastEntry->setColumnHidden(9,true);
}

void MonthView::updateCreditReview(){
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    if (!query.prepare("SELECT card.name, SUM(t.amount) "
        "FROM money_transactions t "
        "JOIN accounts card ON t.account_id = card.id "
        "WHERE t.date >= :start "
        "AND t.date <= :end "
        "AND t.user_id = :user "
        "AND card.type = 'credit' "
        "GROUP BY card.name")) {
        qDebug() << "[updateCreditReview] PREPARE ERROR:" << query.lastError().text();
        return;
    }

    auto [firstDate, lastDate] = monthDateRange();
    query.bindValue(":start", firstDate.toString("yyyy-MM-dd"));
    query.bindValue(":end", lastDate.toString("yyyy-MM-dd"));
    query.bindValue(":user", user_id);

    if (!query.exec()) {
        qDebug() << "[updateCreditReview] ERROR:" << query.lastError().text();
        return;
    }

    QMap<QString, int> usedCreditMap;
    while (query.next()) {
        usedCreditMap[query.value(0).toString()] = query.value(1).toInt();
    }

    for(int i = 0; i < numberOfCreditCards; i++){
        QString bankName   = accountModel->index(i, 2).data().toString();
        int limitCredit    = accountModel->index(i, 3).data().toInt();
        int usedCredit     = usedCreditMap.value(bankName, 0);
        int availableCredit = limitCredit - usedCredit;

        m_labels[i*numberOfLabelPerCreditCard]->setTextFormat(Qt::MarkdownText);
        m_labels[i*numberOfLabelPerCreditCard]->setText(QString("**%1**").arg(bankName));
        m_labels[i*numberOfLabelPerCreditCard+2]->setText(QString::number(usedCredit));
        m_labels[i*numberOfLabelPerCreditCard+4]->setText(QString::number(availableCredit));
        m_labels[i*numberOfLabelPerCreditCard+6]->setText(QString::number(limitCredit));

        for(int posY = 2; posY < numberOfLabelPerCreditCard; posY+=2){
            m_labels[i*numberOfLabelPerCreditCard+posY]->setAlignment(Qt::AlignRight);
        }
    }
}

void MonthView::updateView(){
    incomesModel->select();
    expensesModel->select();
    categoryModel->select();
    transactionModel->setFilter(monthFilter);

    updateTransactions();

    amountByCategoryMap.clear();

    setAmountByCategory();

    updateLabelsFromFilter(incomesModel, "Income");
    updateLabelsFromFilter(expensesModel, "Expense");

    updateCreditReview();

    updateSummary();
    updatePieChart();

    this->update();
}

void MonthView::setAmountByCategory() {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    if (!query.prepare("SELECT c.name, SUM(t.amount) "
                  "FROM money_transactions t "
                  "JOIN categories c ON t.category_id = c.id "
                  "WHERE t.date >= :start AND t.date <= :end AND t.user_id = :user "
                  "GROUP BY c.name")) {
        qDebug() << "[setAmountByCategory] PREPARE ERROR:" << query.lastError().text();
        return;
    }

    auto [firstDate, lastDate] = monthDateRange();
    query.bindValue(":start", firstDate.toString("yyyy-MM-dd"));
    query.bindValue(":end", lastDate.toString("yyyy-MM-dd"));
    query.bindValue(":user", user_id);

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(0).toString().toLower();
            int total = query.value(1).toInt();
            amountByCategoryMap[name] = total;
        }
    } else {
        qDebug() << "[setAmountByCategory] ERROR:" << query.lastError().text();
    }
}

QPair<QDate, QDate> MonthView::monthDateRange() const {
    return {
        QDate(dateViewSelected.year(), dateViewSelected.month(), 1),
        QDate(dateViewSelected.year(), dateViewSelected.month(), dateViewSelected.daysInMonth())
    };
}

QString MonthView::getTypeFromCategory(const QString& category) {
    for (int i = 0; i < incomesModel->rowCount(); ++i) {
        if (incomesModel->record(i).value("name").toString().toLower() == category) {
            return "income";
        }
    }
    return "expense";
}

void MonthView::updateLabelsFromFilter(QSqlTableModel *model, const QString &labelPrefix) {
    const int maxLabels = 6;

    // Collect categories from the model with their amounts
    QList<QPair<QString, int>> sorted;
    for (int i = 0; i < model->rowCount(); ++i) {
        QString categoryName = model->index(i, 2).data().toString();
        if (!categoryName.isEmpty()) {
            categoryName[0] = categoryName[0].toUpper();
        }
        int amount = amountByCategoryMap.value(categoryName.toLower(), 0);
        sorted.append({categoryName, amount});
    }

    // Sort by amount descending
    std::sort(sorted.begin(), sorted.end(), [](const QPair<QString, int> &a, const QPair<QString, int> &b) {
        return a.second > b.second;
    });

    for (int i = 0; i < maxLabels; ++i) {
        QString objectName = QString("%1%2").arg(labelPrefix).arg(i + 1);
        QLabel *nameLabel   = m_labelCache.value(objectName, nullptr);
        QLabel *amountLabel = m_labelCache.value(objectName + "Amount", nullptr);

        if (i < sorted.size()) {
            if (nameLabel) {
                nameLabel->setText(sorted[i].first);
                nameLabel->setVisible(true);
            }
            if (amountLabel) {
                amountLabel->setText(s_locale.toString(sorted[i].second));
                amountLabel->setVisible(true);
            }
        } else {
            if (nameLabel) {
                nameLabel->setText("");
                nameLabel->setVisible(false);
            }
            if (amountLabel) {
                amountLabel->setText("0");
                amountLabel->setVisible(false);
            }
        }
    }
}



void MonthView::updateSummary(){
    totalIncomes = 0;
    totalExpenses = 0;

    for (const auto& [category, amount] : amountByCategoryMap.asKeyValueRange()) {
        QString type = getTypeFromCategory(category.toLower());
        if(type == "expense"){
            totalExpenses += amount;
        }else{
            totalIncomes += amount;
        }
    }

    savings = totalIncomes - totalExpenses;

    ui->TotalSavingsAmount->setText(s_locale.toString(savings));
    ui->TotalIncomesAmount->setText(s_locale.toString(totalIncomes));
    ui->TotalExpensesAmount->setText(s_locale.toString(totalExpenses));
}

void MonthView::updatePieChart() {
    auto *series = new QPieSeries();
    for (const auto& [category, amount] : amountByCategoryMap.asKeyValueRange()) {
        if(getTypeFromCategory(category.toLower()) == "expense"){
            series->append(category, amount);
        }
    }

    if (totalIncomes > totalExpenses) {
        QPieSlice *slice = series->append("Available balance", savings);
        slice->setBrush(Qt::lightGray);
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Money Distribution");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);

    for (QPieSlice *slice : series->slices()) {
        if (slice->value() <= 0) {
            slice->setLabelVisible(false);
            slice->setExploded(false);
        } else {
            double percentage = 100 * slice->percentage();
            slice->setLabel(QString("%1: %2%").arg(slice->label()).arg(percentage, 0, 'f', 1));
            slice->setLabelVisible(true);
        }
    }

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    QChart *oldChart = ui->graphicsView->chart();
    ui->graphicsView->setChart(chart);
    if (oldChart) {
        delete oldChart;
    }
}

void MonthView::initView(){
    QString currentDateTime = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm");
    ui->DateNowLabel->setText(QString("Current time:\n %1").arg(currentDateTime));

    auto [firstDate, lastDate] = monthDateRange();
    monthFilter = QString("money_transactions.date >= '%1' AND money_transactions.date <= '%2' AND money_transactions.user_id = '%3'")
        .arg(firstDate.toString("yyyy-MM-dd"))
        .arg(lastDate.toString("yyyy-MM-dd"))
        .arg(user_id);

    transactionModel->setFilter(monthFilter);
    transactionModel->setHeaderData(4, Qt::Horizontal, "category");
    transactionModel->setHeaderData(5, Qt::Horizontal, "account");
    if(!transactionModel->select()){qDebug()<<"[updateTransactions] error:" << transactionModel->lastError().text();return;};
    
    if (ui->TableViewLastEntry->model() != transactionModel) {
        ui->TableViewLastEntry->setModel(transactionModel);
        ui->TableViewLastEntry->setItemDelegate(new QSqlRelationalDelegate(ui->TableViewLastEntry));
    }

    QString creditAccountFilter = QString("accounts.type = '%1' AND accounts.user_id = '%2'").arg("credit").arg(user_id);
    
    accountModel->setFilter(creditAccountFilter);
    accountModel->select();

    numberOfCreditCards = accountModel->rowCount() <= maxNumberOfCreditCardsCanDisplay ? accountModel->rowCount() : maxNumberOfCreditCardsCanDisplay;
    m_labels.resize(numberOfCreditCards * numberOfLabelPerCreditCard);

    for (int newLabelNumber = 0; newLabelNumber < numberOfCreditCards*numberOfLabelPerCreditCard; newLabelNumber++) {
        m_labels[newLabelNumber] = new QLabel(this);
        m_labels[newLabelNumber]->setVisible(true);
        m_labels[newLabelNumber]->setText(labels[newLabelNumber % numberOfLabelPerCreditCard]);
        
        ui->gridLayout_2->addWidget(m_labels[newLabelNumber], newLabelNumber%numberOfLabelPerCreditCard, newLabelNumber/numberOfLabelPerCreditCard);
        
    }
    ui->gridLayout_2->setHorizontalSpacing(50);

    for (const QString &prefix : {QString("Income"), QString("Expense")}) {
        for (int i = 1; i <= 6; ++i) {
            QString name = QString("%1%2").arg(prefix).arg(i);
            m_labelCache[name]            = findChild<QLabel*>(name);
            m_labelCache[name + "Amount"] = findChild<QLabel*>(name + "Amount");
        }
    }

    updateView();
}