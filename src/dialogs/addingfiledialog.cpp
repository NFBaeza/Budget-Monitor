#include "dialogs/addingfiledialog.h"
#include "database/databaseworker.h"
#include "./ui_addingfiledialog.h"
#include <QFileInfo>

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
            this, "Select a file", QString(),
            "Statements (*.xlsx *.xls *.csv *.pdf)");
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

    const int accountRow = ui->accountComboBox->currentIndex();
    const QString bankSelect = ui->accountComboBox->currentText();
    if (accountRow < 0 || bankSelect.isEmpty()) {
        QMessageBox::warning(this, "Error", "Invalid account selection");
        return;
    }

    const QString accountType = accountModel->record(accountRow).value("type").toString();

    qDebug()<<bankSelect<<" "<<accountType;

    QString filePath = ui->filePathLineEdit->text();
    const QString suffix = QFileInfo(filePath).suffix().toLower();
    const QString accountName = ui->accountComboBox->currentText();

    QVariantList txList;

    if (suffix == "pdf") {
        auto bank = pdfparser::BankFactory::create(bankSelect, accountType, filePath);
        if (!bank) {
            QMessageBox::warning(this, "Error", "Could not create PDF bank reader");
            return;
        }

        bank->readBankMovements(filePath);

        const auto &txs = bank->getTransactions();
        if (txs.isEmpty()) {
            QMessageBox::warning(this, "Error", "No transactions found in file");
            return;
        }

        for (const auto &t : txs) {
            QVariantMap m;
            m["date"] = t.date;
            m["amount"] = t.amount;
            m["category"] = t.category;
            m["account"] = accountName;
            m["description"] = t.description;
            txList.append(m);
        }
    } else {
        auto bank = BankFactory::create(bankSelect, accountType, filePath);
        if (!bank) {
            QMessageBox::warning(this, "Error", "Could not create bank reader");
            return;
        }

        bank->readBankMovements(filePath);

        if (bank->transactions.empty()) {
            QMessageBox::warning(this, "Error", "No transactions found in file");
            return;
        }

        for (const auto &t : bank->transactions) {
            QVariantMap m;
            m["date"] = t.date;
            m["amount"] = t.amount;
            m["category"] = t.category;
            m["account"] = accountName;
            m["description"] = t.description;
            txList.append(m);
        }
    }

    ui->buttonBox->setEnabled(false);

    auto *worker = DatabaseManager::instance().worker();

    connect(worker, &DatabaseWorker::bulkImportProgress, this, [](int current, int total) {
        qDebug() << "[BulkImport] Progress:" << current << "/" << total;
    });

    connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
        if (op == "bulkImport") {
            emit dataInserted();
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
