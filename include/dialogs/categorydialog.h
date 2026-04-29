#ifndef CATEGORYDIALOG_H
#define CATEGORYDIALOG_H

#include "database/databasemanager.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlError>
#include <QToolButton>
#include <QLineEdit>
#include <QComboBox>
#include <QList>
#include <functional>

namespace Ui { class CategoryDialog; }
class DatabaseWorker;

class CategoryDialog : public QDialog {
    Q_OBJECT

public:
    explicit CategoryDialog(QWidget *parent = nullptr);
    ~CategoryDialog();

signals:
    void dataUpdated();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    
    void onAddClicked();
    void onDeleteClicked();
    void onDoubleClicked(const QModelIndex &index);

private:
    Ui::CategoryDialog *ui;

    QSqlTableModel *categoryModel{nullptr};

    struct CategoryRow {
        int id;
        QString originalName;
        QString originalType;
        QLineEdit *nameEdit;
        QComboBox *typeCombo;
    };
    QList<CategoryRow> categoryRows;

    void initView();
    void loadCategories();
    void rebuildCategoryRows();
    void setButtonsEnabled(bool enabled);
    void runWorkerOp(const QString &opName,
                     const QString &errorLabel,
                     std::function<void(DatabaseWorker*)> invoke);
};

#endif