#include "dialogs/formdialog.h"
#include "database/databaseworker.h"
#include "./ui_formdialog.h"

extern QString user_id;

FormDialog::FormDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FormDialog)
{
    ui->setupUi(this);
    ui->DeleteButton->setVisible(false);

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    accountModel = DatabaseManager::instance().getAccountModel(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &FormDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &FormDialog::onCancelClicked);
    connect(ui->IncomeRadioButton, &QRadioButton::toggled, this, &FormDialog::updateComboText);
    ui->DateTimeSelected->setDate(QDate::currentDate());
    initView();
}

FormDialog::FormDialog(int transactionId, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::FormDialog),
      editingTransactionId(transactionId) {

    ui->setupUi(this);
    ui->DeleteButton->setVisible(true);

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    accountModel = DatabaseManager::instance().getAccountModel(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &FormDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &FormDialog::onCancelClicked);
    connect(ui->DeleteButton, &QPushButton::pressed, this, &FormDialog::onDeleteClicked);
    connect(ui->IncomeRadioButton, &QRadioButton::toggled, this, &FormDialog::updateComboText);

    initView();
    loadTransactionData(transactionId);
}

FormDialog::~FormDialog() {
    delete ui;
}

void FormDialog::setButtonsEnabled(bool enabled) {
    ui->buttonBox->setEnabled(enabled);
    ui->DeleteButton->setEnabled(enabled);
}

void FormDialog::onDeleteClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        "Are you sure you want to delete this transaction?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) {
        return;
    }

    setButtonsEnabled(false);
    auto *worker = DatabaseManager::instance().worker();

    connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
        if (op == "deleteTransaction") {
            emit dataDeleted();
            accept();
        }
    }, Qt::SingleShotConnection);

    connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
        if (op == "deleteTransaction") {
            QMessageBox::critical(this, "Error", "Failed to delete transaction: " + err);
            setButtonsEnabled(true);
        }
    }, Qt::SingleShotConnection);

    QMetaObject::invokeMethod(worker, "deleteTransaction", Qt::QueuedConnection,
                              Q_ARG(int, editingTransactionId));
}

void FormDialog::loadTransactionData(int transactionId) {
    // Keep synchronous on UI thread — single-row SELECT at dialog open
    QSqlQuery query(db);
    query.prepare("SELECT date, amount, category_id, account_id, description "
                  "FROM money_transactions WHERE id = :id AND user_id = :user_id");
    query.bindValue(":id", transactionId);
    query.bindValue(":user_id", user_id);

    if (query.exec() && query.next()) {
        QString dateStr = query.value("date").toString();
        QDateTime dateTime = QDateTime::fromString(dateStr, Qt::ISODate);
        ui->DateTimeSelected->setDateTime(dateTime);

        int amount = query.value("amount").toInt();
        ui->InputAmountText->setText(QString::number(amount));

        QString description = query.value("description").toString();
        ui->DescriptionText->setText(description);

        int categoryId = query.value("category_id").toInt();
        int accountId = query.value("account_id").toInt();

        QSqlQuery typeQuery(db);
        typeQuery.prepare("SELECT type FROM categories WHERE id = :id AND user_id = :user_id");
        typeQuery.bindValue(":id", categoryId);
        typeQuery.bindValue(":user_id", user_id);
        if (typeQuery.exec() && typeQuery.next()) {
            QString type = typeQuery.value("type").toString();
            if (type == "income") {
                ui->IncomeRadioButton->setChecked(true);
            } else {
                ui->ExpenseRadioButton->setChecked(true);
            }
        }

        for (int i = 0; i < categoryModel->rowCount(); ++i) {
            if (categoryModel->index(i, 0).data().toInt() == categoryId) {
                ui->ListCategoryDialog->setCurrentIndex(i);
                break;
            }
        }

        for (int i = 0; i < accountModel->rowCount(); ++i) {
            if (accountModel->index(i, 0).data().toInt() == accountId) {
                ui->ListAccountDialog->setCurrentIndex(i);
                break;
            }
        }
    }
}


void FormDialog::updateComboText(){
    if(ui->IncomeRadioButton->isChecked()){
        categoryModel->setFilter(QString("type = 'income'AND user_id = '%1'").arg(user_id));
    }else{
        categoryModel->setFilter(QString("type = 'expense'AND user_id = '%1'").arg(user_id));
    }

    categoryModel->select();

    ui->ListCategoryDialog->setModel(categoryModel);
    ui->ListCategoryDialog->setModelColumn(2);
}

void FormDialog::initView(){
    if(editingTransactionId >= 0){
        ui->TitleFormDialog->setText("Edit Transaction");
    }else{
        ui->TitleFormDialog->setText("Add new data");
    }

    categoryModel->select();
    updateComboText();

    accountModel->select();
    ui->ListAccountDialog->setModel(accountModel);
    ui->ListAccountDialog->setModelColumn(2);
}

// ==================== OK / CANCEL ====================

void FormDialog::onAcceptClicked() {
    if (ui->InputAmountText->text().isEmpty() || ui->DateTimeSelected->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all the options");
        return;
    }

    int categoryRow = ui->ListCategoryDialog->currentIndex();
    int categoryId = categoryModel->index(categoryRow, 0).data().toInt();

    int accountRow = ui->ListAccountDialog->currentIndex();
    int accountId = accountModel->index(accountRow, 0).data().toInt();

    QString date = ui->DateTimeSelected->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    
    int amount = ui->InputAmountText->text().toDouble();
    
    QString description = ui->DescriptionText->text();

    setButtonsEnabled(false);
    
    auto *worker = DatabaseManager::instance().worker();

    QString opName = (editingTransactionId != -1) ? "updateTransaction" : "insertTransaction";

    connect(worker, &DatabaseWorker::operationFinished, this, [this, opName](const QString &op) {
        if (op == opName) {
            op == "updateTransaction" ? emit dataUpdated() :  emit dataInserted();
            accept();
        }
    }, Qt::SingleShotConnection);

    connect(worker, &DatabaseWorker::operationError, this, [this, opName](const QString &op, const QString &err) {
        if (op == opName) {
            QMessageBox::critical(this, "Error", "Failed to save transaction: " + err);
            setButtonsEnabled(true);
        }
    }, Qt::SingleShotConnection);

    if (editingTransactionId != -1) {
        QMetaObject::invokeMethod(worker, "updateTransaction", Qt::QueuedConnection,
                                  Q_ARG(int, editingTransactionId),
                                  Q_ARG(QString, user_id),
                                  Q_ARG(QString, date),
                                  Q_ARG(int, amount),
                                  Q_ARG(int, categoryId),
                                  Q_ARG(int, accountId),
                                  Q_ARG(QString, description));
    } else {
        QMetaObject::invokeMethod(worker, "insertTransaction", Qt::QueuedConnection,
                                  Q_ARG(QString, user_id),
                                  Q_ARG(QString, date),
                                  Q_ARG(int, amount),
                                  Q_ARG(int, categoryId),
                                  Q_ARG(int, accountId),
                                  Q_ARG(QString, description));
    }
}

void FormDialog::onCancelClicked() {
    reject();
}