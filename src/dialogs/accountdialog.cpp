#include "dialogs/accountdialog.h"
#include "database/databaseworker.h"
#include "./ui_accountdialog.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QSqlRecord>

extern QString user_id;

AccountManager::AccountManager(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AccountDialog)
{
    ui->setupUi(this);

    accountsModel = DatabaseManager::instance().getAccountModel(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AccountManager::onSaveClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AccountManager::onCancelClicked);
    connect(ui->addAccountButton, &QToolButton::clicked, this, &AccountManager::onAddAccountClicked);
    connect(ui->accountListView, &QListView::doubleClicked, this, &AccountManager::onAccountDoubleClicked);

    initView();
}

AccountManager::~AccountManager() {
    accountsModel->clear();
    delete ui;
}

void AccountManager::initView() {
    loadAccounts();
    ui->accountListView->setModel(accountsModel);
    ui->accountListView->setModelColumn(2); // name column
}

void AccountManager::loadAccounts() {
    accountsModel->select();
}

void AccountManager::setButtonsEnabled(bool enabled) {
    ui->buttonBox->setEnabled(enabled);
    ui->addAccountButton->setEnabled(enabled);
}

// ==================== ADD ACCOUNT ====================

void AccountManager::onAddAccountClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add Account");

    auto *layout = new QFormLayout(&dlg);

    auto *nameEdit  = new QLineEdit(&dlg);
    auto *typeCombo = new QComboBox(&dlg);
    auto *limitEdit = new QLineEdit(&dlg);
    auto *limitLabel = new QLabel("Limit:", &dlg);

    typeCombo->addItems({"debit", "credit", "investment"});

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);

    layout->addRow("Name:", nameEdit);
    layout->addRow("Type:", typeCombo);
    layout->addRow(limitLabel, limitEdit);
    layout->addRow(buttons);

    limitLabel->setVisible(false);
    limitEdit->setVisible(false);

    connect(typeCombo, &QComboBox::currentTextChanged, &dlg,
        [limitLabel, limitEdit](const QString &t) {
            const bool isCredit = (t == "credit");
            limitLabel->setVisible(isCredit);
            limitEdit->setVisible(isCredit);
    });
    
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted)
        return;

    const QString name = nameEdit->text().trimmed();
    if (name.isEmpty())
        return;

    const QString type = typeCombo->currentText();

    qlonglong limit = -1;
    if (type == "credit") {
        bool ok = false;
        limit = limitEdit->text().trimmed().toLongLong(&ok);
        if (!ok || limit < 0) {
            QMessageBox::warning(this, "Invalid limit",
                                 "Credit accounts require a non-negative limit.");
            return;
        }
    }

    setButtonsEnabled(false);
    auto *worker = DatabaseManager::instance().worker();

    connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
        if (op == "addAccount") {
            loadAccounts();
            setButtonsEnabled(true);
            emit dataUpdated();
        }
    }, Qt::SingleShotConnection);

    connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
        if (op == "addAccount") {
            QMessageBox::critical(this, "Error", "Failed to add account: " + err);
            setButtonsEnabled(true);
        }
    }, Qt::SingleShotConnection);

    QMetaObject::invokeMethod(worker, "addAccount", Qt::QueuedConnection,
                              Q_ARG(QString, user_id),
                              Q_ARG(QString, name),
                              Q_ARG(QString, type),
                              Q_ARG(qlonglong, limit));
}

// ==================== EDIT / DELETE ACCOUNT ====================

void AccountManager::onAccountDoubleClicked(const QModelIndex &index) {
    int row = index.row();
    const QSqlRecord rec = accountsModel->record(row);
    const int id = rec.value(QStringLiteral("id")).toInt();
    const QString currentName = rec.value(QStringLiteral("name")).toString();
    const QString currentType = rec.value(QStringLiteral("type")).toString();
    const qlonglong currentLimit = rec.value(QStringLiteral("limit")).toLongLong();
    const bool isCredit = (currentType == "credit");

    QDialog dlg(this);
    dlg.setWindowTitle("Edit Account");

    auto *layout = new QVBoxLayout(&dlg);
    auto *form   = new QFormLayout();

    auto *nameEdit = new QLineEdit(currentName, &dlg);
    form->addRow("Name:", nameEdit);

    QLineEdit *limitEdit = nullptr;
    if (isCredit) {
        limitEdit = new QLineEdit(QString::number(currentLimit), &dlg);
        form->addRow("Limit:", limitEdit);
    }
    layout->addLayout(form);

    auto *buttons   = new QDialogButtonBox(&dlg);
    auto *saveBtn   = buttons->addButton("Save",   QDialogButtonBox::AcceptRole);
    auto *deleteBtn = buttons->addButton("Delete", QDialogButtonBox::DestructiveRole);
    auto *cancelBtn = buttons->addButton(QDialogButtonBox::Cancel);
    Q_UNUSED(cancelBtn);
    layout->addWidget(buttons);

    bool deleteRequested = false;

    connect(deleteBtn, &QPushButton::clicked, [&]() {
        QMessageBox::StandardButton reply = QMessageBox::warning(
            &dlg, "Confirm Delete",
            QString("Delete account '%1'?\n\n"
                    "Warning: All transactions linked to this account will lose their account reference.")
                .arg(currentName),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            deleteRequested = true;
            dlg.accept();
        }
    });

    connect(saveBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted)
        return;

    auto *worker = DatabaseManager::instance().worker();

    if (deleteRequested) {
        setButtonsEnabled(false);

        connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
            if (op == "deleteAccount") {
                loadAccounts();
                setButtonsEnabled(true);
                emit dataUpdated();
            }
        }, Qt::SingleShotConnection);

        connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
            if (op == "deleteAccount") {
                QMessageBox::critical(this, "Error", "Failed to delete account: " + err);
                setButtonsEnabled(true);
            }
        }, Qt::SingleShotConnection);

        QMetaObject::invokeMethod(worker, "deleteAccount", Qt::QueuedConnection,
                                  Q_ARG(int, id));
    } else {
        const QString newName = nameEdit->text().trimmed();
        if (newName.isEmpty())
            return;

        qlonglong newLimit = -1;
        if (isCredit) {
            bool ok = false;
            newLimit = limitEdit->text().trimmed().toLongLong(&ok);
            if (!ok || newLimit < 0) {
                QMessageBox::warning(this, "Invalid limit",
                                     "Credit accounts require a non-negative limit.");
                return;
            }
        }

        const bool nameChanged  = (newName  != currentName);
        const bool limitChanged = isCredit && (newLimit != currentLimit);
        if (!nameChanged && !limitChanged)
            return;

        setButtonsEnabled(false);

        connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
            if (op == "updateAccount") {
                loadAccounts();
                setButtonsEnabled(true);
                emit dataUpdated();
            }
        }, Qt::SingleShotConnection);

        connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
            if (op == "updateAccount") {
                QMessageBox::critical(this, "Error", "Failed to update account: " + err);
                setButtonsEnabled(true);
            }
        }, Qt::SingleShotConnection);

        QMetaObject::invokeMethod(worker, "updateAccount", Qt::QueuedConnection,
                                  Q_ARG(int, id),
                                  Q_ARG(QString, newName),
                                  Q_ARG(qlonglong, isCredit ? newLimit : -1));
    }
}

// ==================== SAVE / CANCEL ====================

void AccountManager::onSaveClicked() {
    accept();
}

void AccountManager::onCancelClicked() {
    reject();
}
