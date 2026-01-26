#include "dialogs/addingfiledialog.h"
#include "./ui_addingfiledialog.h"

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
    ui->accountComboBox->setModelColumn(1);
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
    QString filePath = ui->filePathLineEdit->text();

    auto bank = BankFactory::create(bankSelect[0], bankSelect[1], filePath);

    if (bank) {
        bank->readBankMovements(ui->filePathLineEdit->text());
        qDebug()<<"------------------------";
        bank->uploadDataToDataBase(db, "money_transactions");
        accept();
    }
     
}

void AddingFileDialog::onCancelClicked() {
    reject();
}

