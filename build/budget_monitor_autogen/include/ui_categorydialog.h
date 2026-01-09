/********************************************************************************
** Form generated from reading UI file 'categorydialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CATEGORYDIALOG_H
#define UI_CATEGORYDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_CategoryDialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QToolButton *AccountAddButton;
    QToolButton *AccountRemoveButton;
    QLabel *label_4;
    QListView *listAccountView;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *ExpenseAddButton;
    QToolButton *ExpenseRemoveButton;
    QListView *listExpenseView;
    QLabel *label_2;
    QDialogButtonBox *buttonBox;
    QListView *listIncomeView;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_3;
    QToolButton *IncomeAddButton;
    QToolButton *IncomeRemoveButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *CategoryDialog)
    {
        if (CategoryDialog->objectName().isEmpty())
            CategoryDialog->setObjectName("CategoryDialog");
        CategoryDialog->resize(429, 300);
        CategoryDialog->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        gridLayout = new QGridLayout(CategoryDialog);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        gridLayout->setHorizontalSpacing(20);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        AccountAddButton = new QToolButton(CategoryDialog);
        AccountAddButton->setObjectName("AccountAddButton");
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd));
        AccountAddButton->setIcon(icon);

        horizontalLayout->addWidget(AccountAddButton);

        AccountRemoveButton = new QToolButton(CategoryDialog);
        AccountRemoveButton->setObjectName("AccountRemoveButton");
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::ListRemove));
        AccountRemoveButton->setIcon(icon1);

        horizontalLayout->addWidget(AccountRemoveButton);


        gridLayout->addLayout(horizontalLayout, 5, 3, 1, 1);

        label_4 = new QLabel(CategoryDialog);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 0, 2, 1, 1);

        listAccountView = new QListView(CategoryDialog);
        listAccountView->setObjectName("listAccountView");
        listAccountView->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked|QAbstractItemView::EditTrigger::EditKeyPressed|QAbstractItemView::EditTrigger::SelectedClicked);

        gridLayout->addWidget(listAccountView, 3, 3, 1, 1);

        label = new QLabel(CategoryDialog);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(label, 2, 1, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        ExpenseAddButton = new QToolButton(CategoryDialog);
        ExpenseAddButton->setObjectName("ExpenseAddButton");
        ExpenseAddButton->setIcon(icon);

        horizontalLayout_2->addWidget(ExpenseAddButton);

        ExpenseRemoveButton = new QToolButton(CategoryDialog);
        ExpenseRemoveButton->setObjectName("ExpenseRemoveButton");
        ExpenseRemoveButton->setIcon(icon1);

        horizontalLayout_2->addWidget(ExpenseRemoveButton);


        gridLayout->addLayout(horizontalLayout_2, 5, 2, 1, 1);

        listExpenseView = new QListView(CategoryDialog);
        listExpenseView->setObjectName("listExpenseView");
        listExpenseView->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked|QAbstractItemView::EditTrigger::EditKeyPressed|QAbstractItemView::EditTrigger::SelectedClicked);

        gridLayout->addWidget(listExpenseView, 3, 2, 1, 1);

        label_2 = new QLabel(CategoryDialog);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(label_2, 2, 2, 1, 1);

        buttonBox = new QDialogButtonBox(CategoryDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Save);

        gridLayout->addWidget(buttonBox, 6, 2, 1, 2);

        listIncomeView = new QListView(CategoryDialog);
        listIncomeView->setObjectName("listIncomeView");
        listIncomeView->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked|QAbstractItemView::EditTrigger::EditKeyPressed|QAbstractItemView::EditTrigger::SelectedClicked);
        listIncomeView->setTabKeyNavigation(true);
        listIncomeView->setDragEnabled(true);
        listIncomeView->setDefaultDropAction(Qt::DropAction::CopyAction);
        listIncomeView->setAlternatingRowColors(false);
        listIncomeView->setViewMode(QListView::ViewMode::ListMode);
        listIncomeView->setItemAlignment(Qt::AlignmentFlag::AlignLeading);

        gridLayout->addWidget(listIncomeView, 3, 1, 1, 1);

        label_3 = new QLabel(CategoryDialog);
        label_3->setObjectName("label_3");
        label_3->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
        label_3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(label_3, 2, 3, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        IncomeAddButton = new QToolButton(CategoryDialog);
        IncomeAddButton->setObjectName("IncomeAddButton");
        IncomeAddButton->setIcon(icon);

        horizontalLayout_3->addWidget(IncomeAddButton);

        IncomeRemoveButton = new QToolButton(CategoryDialog);
        IncomeRemoveButton->setObjectName("IncomeRemoveButton");
        IncomeRemoveButton->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        IncomeRemoveButton->setIcon(icon1);

        horizontalLayout_3->addWidget(IncomeRemoveButton);


        gridLayout->addLayout(horizontalLayout_3, 5, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 2, 1, 1);

        gridLayout->setRowStretch(0, 1);

        retranslateUi(CategoryDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, CategoryDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, CategoryDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(CategoryDialog);
    } // setupUi

    void retranslateUi(QDialog *CategoryDialog)
    {
        CategoryDialog->setWindowTitle(QCoreApplication::translate("CategoryDialog", "Dialog", nullptr));
        AccountAddButton->setText(QCoreApplication::translate("CategoryDialog", "...", nullptr));
        AccountRemoveButton->setText(QCoreApplication::translate("CategoryDialog", "...", nullptr));
        label_4->setText(QCoreApplication::translate("CategoryDialog", "Categories Edit", nullptr));
        label->setText(QCoreApplication::translate("CategoryDialog", "Income", nullptr));
        ExpenseAddButton->setText(QCoreApplication::translate("CategoryDialog", "...", nullptr));
        ExpenseRemoveButton->setText(QCoreApplication::translate("CategoryDialog", "...", nullptr));
        label_2->setText(QCoreApplication::translate("CategoryDialog", "Expense", nullptr));
        label_3->setText(QCoreApplication::translate("CategoryDialog", "Account", nullptr));
        IncomeAddButton->setText(QCoreApplication::translate("CategoryDialog", "...", nullptr));
        IncomeRemoveButton->setText(QCoreApplication::translate("CategoryDialog", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CategoryDialog: public Ui_CategoryDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CATEGORYDIALOG_H
