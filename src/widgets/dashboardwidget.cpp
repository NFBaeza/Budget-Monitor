#include "widgets/dashboardwidget.h"
#include "views/mainwindow.h"
#include "./ui_dashboardwidget.h"
#include "dialogs/formdialog.h"
#include "dialogs/categorydialog.h"
#include "dialogs/addingfiledialog.h"
#include <time.h>
#include <QDateTime>


extern QString user_id;

MonthView::MonthView(QDate date_selected, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonthView),
    dateViewSelected(date_selected) {
    
    ui->setupUi(this);
    this->setMinimumSize(1080, 650);

    month_name_ = QLocale().monthName(dateViewSelected.month());
    month_name_[0] = month_name_[0].toUpper();
    ui->MonthNameLabel->setText(QString("%1 %2").arg(month_name_).arg(dateViewSelected.year()));

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    incomesModel= DatabaseManager::instance().getIncomeModel(this);
    expensesModel = DatabaseManager::instance().getExpenseModel(this);
    transactionModel = DatabaseManager::instance().getTransactionsModel(this);
    
    connect(ui->BackButton, &QPushButton::clicked, this, &MonthView::backButtonWasPressed);
    connect(ui->AddEntryButton, &QPushButton::clicked, this, &MonthView::onAddButtonClicked);
    connect(ui->TableViewLastEntry, &QTableView::doubleClicked, this, &MonthView::onTableRowDoubleClicked);
    connect(ui->EditCategoryButton, &QPushButton::clicked, this, &MonthView::onEditButtonCliked);
    connect(ui->addFileButton, &QPushButton::clicked, this, &MonthView::onAddFileButtonCliked);

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
        qDebug() << "[OnEditButtonCliked] Categories Updated, refreshing...";
        updateView();
    });

    dialog.exec();
}

void MonthView::onAddFileButtonCliked() {
    AddingFileDialog dialog(this);

    connect(&dialog, &AddingFileDialog::dataInserted, this, [this]() {
        qDebug() << "[OnEditButtonCliked] Categories Updated, refreshing...";
        updateView();
    });

    dialog.exec();
}

void MonthView::onEditButtonCliked() {
    CategoryDialog dialog(this);

    connect(&dialog, &CategoryDialog::dataUpdated, this, [this]() {
        qDebug() << "[OnEditButtonCliked] Categories Updated, refreshing...";
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
    ui->TableViewLastEntry->setColumnHidden(7,true);
    ui->TableViewLastEntry->setColumnHidden(8,true);
}

void MonthView::updateView(){
    incomesModel->select();
    expensesModel->select();
    categoryModel->select();
    transactionModel->setFilter(MonthFilter);

    updateTransactions();

    amountByCategoryMap.clear();

    setAmountByCategory();

    updateLabelsFromFilter(incomesModel, "Income");
    updateLabelsFromFilter(expensesModel, "Expense");

    updateSummary();
    updatePieChart();

    this->update();
}

void MonthView::setAmountByCategory() {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT c.name, SUM(t.amount) "
                  "FROM money_transactions t "
                  "JOIN categories c ON t.category_id = c.id "
                  "WHERE t.date >= :start AND t.date <= :end AND t.user_id = :user "
                  "GROUP BY c.name");

    QDate firstDate(dateViewSelected.year(), dateViewSelected.month(), 1);
    QDate lastDate(dateViewSelected.year(), dateViewSelected.month(), dateViewSelected.daysInMonth());
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
        QLabel *nameLabel = this->findChild<QLabel*>(objectName);
        QLabel *amountLabel = this->findChild<QLabel*>(objectName + "Amount");

        if (i < sorted.size()) {
            if (nameLabel) {
                nameLabel->setText(sorted[i].first);
                nameLabel->setVisible(true);
            }
            if (amountLabel) {
                amountLabel->setText(QString::number(sorted[i].second));
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
    totalIncomes = 0.0;
    totalExpenses = 0.0;

    for (const auto& [category, monto] : amountByCategoryMap.asKeyValueRange()) {
        QString tipo = getTypeFromCategory(category.toLower());
        if(tipo == "expense"){
            totalExpenses += monto;
        }else{
            totalIncomes += monto;
        }
    }

    savings = totalIncomes - totalExpenses;

    ui->TotalSavingsAmount->setText(QString::number(savings));
    ui->TotalIncomesAmount->setText(QString::number(totalIncomes));
    ui->TotalExpensesAmount->setText(QString::number(totalExpenses));
}

void MonthView::updatePieChart() {
    if(amountByCategoryMap.isEmpty()) {updateSummary();}

    auto *series = new QPieSeries();
    for (const auto& [categoria, monto] : amountByCategoryMap.asKeyValueRange()) {
        if(getTypeFromCategory(categoria.toLower()) == "expense"){
            series->append(categoria, monto);
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
            double porcentaje = 100 * slice->percentage();
            slice->setLabel(QString("%1: %2%").arg(slice->label()).arg(porcentaje, 0, 'f', 1));
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

    QString firstDate = QDate(dateViewSelected.year(), dateViewSelected.month(), 1).toString("yyyy-MM-dd");
    QString lastDate = QDate(dateViewSelected.year(), dateViewSelected.month(), dateViewSelected.daysInMonth()).toString("yyyy-MM-dd");

    MonthFilter = QString("money_transactions.date >= '%1' AND money_transactions.date <= '%2' AND money_transactions.user_id = '%3'").arg(firstDate).arg(lastDate).arg(user_id);

    transactionModel->setFilter(MonthFilter);
    transactionModel->setHeaderData(4, Qt::Horizontal, "category");
    transactionModel->setHeaderData(5, Qt::Horizontal, "account");
    if(!transactionModel->select()){qDebug()<<"[updateTransactions] error:" << transactionModel->lastError().text();return;};
    
    if (ui->TableViewLastEntry->model() != transactionModel) {
        ui->TableViewLastEntry->setModel(transactionModel);
        ui->TableViewLastEntry->setItemDelegate(new QSqlRelationalDelegate(ui->TableViewLastEntry));
    }

    updateView();
}