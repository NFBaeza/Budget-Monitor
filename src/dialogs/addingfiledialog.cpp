#include "dialogs/addingfiledialog.h"
#include "database/databaseworker.h"
#include "./ui_addingfiledialog.h"

extern QString user_id;

AddingFileDialog::AddingFileDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddingFileDialog)
{
    ui->setupUi(this);

    incomeModel = DatabaseManager::instance().getIncomeModel(this);
    expenseModel = DatabaseManager::instance().getExpenseModel(this);
    accountModel = DatabaseManager::instance().getAccountModel(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddingFileDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddingFileDialog::onCancelClicked);
    connect(ui->fileAddButton, &QToolButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this, "Select a file", QString(), "Excel (*.xlsx *.xls)");
        if (!filePath.isEmpty()) {
            ui->filePathLineEdit->setText(filePath);
        }
    });

    ui->filePathLineEdit->setReadOnly(true);
    ui->filePathLineEdit->setPlaceholderText("Select a file ...");

    ui->accountComboBox->setModel(accountModel);
    ui->accountComboBox->setModelColumn(2);
}

AddingFileDialog::~AddingFileDialog() {
    delete ui;
}

void AddingFileDialog::onAcceptClicked() {
    if (ui->filePathLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please add a file");
        return;
    }

    QStringList bankSelect = ui->accountComboBox->currentText().split(" ");
    if (bankSelect.size() < 2) {
        QMessageBox::warning(this, "Error", "Invalid account selection");
        return;
    }

    QString filePath = ui->filePathLineEdit->text();
    auto bank = BankFactory::create(bankSelect[0], bankSelect[1], filePath);

    if (!bank) {
        QMessageBox::warning(this, "Error", "Could not create bank reader");
        return;
    }

    bank->filePath = filePath;
    bank->readBankMovements();

    if (bank->transactions.empty()) {
        QMessageBox::warning(this, "Error", "No transactions found in file");
        return;
    }

    // Convert transactions to QVariantList for the worker
    QVariantList txList;
    for (const auto &t : bank->transactions) {
        QVariantMap m;
        m["date"] = t.date;
        m["amount"] = t.amount;
        m["category"] = t.category;
        m["account"] = t.account;
        m["description"] = t.description;
        txList.append(m);
    }

    ui->buttonBox->setEnabled(false);

    auto *worker = DatabaseManager::instance().worker();

    connect(worker, &DatabaseWorker::bulkImportProgress, this, [](int current, int total) {
        qDebug() << "[BulkImport] Progress:" << current << "/" << total;
    });

    connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
        if (op == "bulkImport") {
            accept();
        }
    }, Qt::SingleShotConnection);

    connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
        if (op == "bulkImport") {
            QMessageBox::critical(this, "Error", "Import failed: " + err);
            ui->buttonBox->setEnabled(true);
        }
    }, Qt::SingleShotConnection);

    QMetaObject::invokeMethod(worker, "bulkImportTransactions", Qt::QueuedConnection,
                              Q_ARG(QString, user_id),
                              Q_ARG(QVariantList, txList));
}

void AddingFileDialog::onCancelClicked() {
    reject();
}
