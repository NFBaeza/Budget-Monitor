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

    ui->DateTimeSelected->setDate(QDate::currentDate());

    setupConnections();
    initView();
}

FormDialog::FormDialog(int transactionId, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::FormDialog),
      editingTransactionId(transactionId)
{
    ui->setupUi(this);
    ui->DeleteButton->setVisible(true);

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    accountModel = DatabaseManager::instance().getAccountModel(this);

    setupConnections();
    connect(ui->DeleteButton, &QPushButton::pressed, this, &FormDialog::onDeleteClicked);

    initView();
    loadTransactionData(transactionId);
}

void FormDialog::setupConnections()
{
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &FormDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &FormDialog::onCancelClicked);
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
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT t.date, t.amount, t.category_id, t.account_id, t.description, c.type "
                  "FROM money_transactions t "
                  "JOIN categories c ON c.id = t.category_id "
                  "WHERE t.id = :id AND t.user_id = :user_id");
    query.bindValue(":id", transactionId);
    query.bindValue(":user_id", user_id);

    if (query.exec() && query.next()) {
        ui->DateTimeSelected->setDateTime(QDateTime::fromString(query.value("date").toString(), Qt::ISODate));
        ui->InputAmountText->setText(QString::number(query.value("amount").toInt()));
        ui->DescriptionText->setText(query.value("description").toString());

        int categoryId = query.value("category_id").toInt();
        for (int i = 0; i < categoryModel->rowCount(); ++i) {
            if (categoryModel->index(i, 0).data().toInt() == categoryId) {
                ui->ListCategoryDialog->setCurrentIndex(i);
                break;
            }
        }

        int accountId = query.value("account_id").toInt();
        for (int i = 0; i < accountModel->rowCount(); ++i) {
            if (accountModel->index(i, 0).data().toInt() == accountId) {
                ui->ListAccountDialog->setCurrentIndex(i);
                break;
            }
        }
    }
}


void FormDialog::initView(){
    if(editingTransactionId >= 0){
        ui->TitleFormDialog->setText("Edit Transaction");
    }else{
        ui->TitleFormDialog->setText("Add new data");
    }

    ui->ListCategoryDialog->setModel(categoryModel);
    ui->ListCategoryDialog->setModelColumn(2);

    ui->ListAccountDialog->setModel(accountModel);
    ui->ListAccountDialog->setModelColumn(2);
}

// ==================== OK / CANCEL ====================

void FormDialog::onAcceptClicked() {
    bool ok;
    int amount = ui->InputAmountText->text().toInt(&ok);
    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "Error", "Please enter a valid amount greater than 0");
        return;
    }

    int categoryRow = ui->ListCategoryDialog->currentIndex();
    int categoryId = categoryModel->index(categoryRow, 0).data().toInt();

    int accountRow = ui->ListAccountDialog->currentIndex();
    int accountId = accountModel->index(accountRow, 0).data().toInt();

    QString date = ui->DateTimeSelected->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString description = ui->DescriptionText->text();

    setButtonsEnabled(false);

    auto *worker = DatabaseManager::instance().worker();
    QString opName = (editingTransactionId != -1) ? "updateTransaction" : "insertTransaction";

    connect(worker, &DatabaseWorker::operationFinished, this, [this, opName](const QString &op) {
        if (op == opName) {
            if (editingTransactionId != -1) {
                emit dataUpdated();
            } else {
                emit dataInserted();
            }
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