/********************************************************************************
** Form generated from reading UI file 'formdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMDIALOG_H
#define UI_FORMDIALOG_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FormDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *TitleFormDialog;
    QHBoxLayout *horizontalLayout;
    QRadioButton *IncomeRadioButton;
    QRadioButton *ExpenseRadioButton;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QDateTimeEdit *DateTimeSelected;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QLineEdit *InputAmountText;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_3;
    QComboBox *ListAccountDialog;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_2;
    QComboBox *ListCategoryDialog;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_6;
    QLineEdit *DescriptionText;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *DeleteButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *FormDialog)
    {
        if (FormDialog->objectName().isEmpty())
            FormDialog->setObjectName("FormDialog");
        FormDialog->resize(359, 391);
        verticalLayout = new QVBoxLayout(FormDialog);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetMaximumSize);
        verticalLayout->setContentsMargins(15, -1, 15, 10);
        TitleFormDialog = new QLabel(FormDialog);
        TitleFormDialog->setObjectName("TitleFormDialog");
        TitleFormDialog->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(TitleFormDialog);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        IncomeRadioButton = new QRadioButton(FormDialog);
        IncomeRadioButton->setObjectName("IncomeRadioButton");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(IncomeRadioButton->sizePolicy().hasHeightForWidth());
        IncomeRadioButton->setSizePolicy(sizePolicy);
        IncomeRadioButton->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        IncomeRadioButton->setChecked(false);
        IncomeRadioButton->setAutoExclusive(true);

        horizontalLayout->addWidget(IncomeRadioButton);

        ExpenseRadioButton = new QRadioButton(FormDialog);
        ExpenseRadioButton->setObjectName("ExpenseRadioButton");
        sizePolicy.setHeightForWidth(ExpenseRadioButton->sizePolicy().hasHeightForWidth());
        ExpenseRadioButton->setSizePolicy(sizePolicy);
        ExpenseRadioButton->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        ExpenseRadioButton->setChecked(true);
        ExpenseRadioButton->setAutoExclusive(true);

        horizontalLayout->addWidget(ExpenseRadioButton);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(10);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label_4 = new QLabel(FormDialog);
        label_4->setObjectName("label_4");

        horizontalLayout_4->addWidget(label_4);

        DateTimeSelected = new QDateTimeEdit(FormDialog);
        DateTimeSelected->setObjectName("DateTimeSelected");
        DateTimeSelected->setAlignment(Qt::AlignmentFlag::AlignCenter);
        DateTimeSelected->setMaximumDate(QDate(2060, 12, 31));
        DateTimeSelected->setMinimumDate(QDate(2020, 1, 1));
        DateTimeSelected->setCurrentSection(QDateTimeEdit::Section::DaySection);

        horizontalLayout_4->addWidget(DateTimeSelected);

        horizontalLayout_4->setStretch(0, 1);
        horizontalLayout_4->setStretch(1, 2);

        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(10);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        label_5 = new QLabel(FormDialog);
        label_5->setObjectName("label_5");

        horizontalLayout_5->addWidget(label_5);

        InputAmountText = new QLineEdit(FormDialog);
        InputAmountText->setObjectName("InputAmountText");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(InputAmountText->sizePolicy().hasHeightForWidth());
        InputAmountText->setSizePolicy(sizePolicy1);
        InputAmountText->setMaxLength(8);
        InputAmountText->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_5->addWidget(InputAmountText);

        horizontalLayout_5->setStretch(0, 1);
        horizontalLayout_5->setStretch(1, 2);

        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(10);
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        label_3 = new QLabel(FormDialog);
        label_3->setObjectName("label_3");

        horizontalLayout_8->addWidget(label_3);

        ListAccountDialog = new QComboBox(FormDialog);
        ListAccountDialog->setObjectName("ListAccountDialog");
        sizePolicy1.setHeightForWidth(ListAccountDialog->sizePolicy().hasHeightForWidth());
        ListAccountDialog->setSizePolicy(sizePolicy1);

        horizontalLayout_8->addWidget(ListAccountDialog);

        horizontalLayout_8->setStretch(0, 1);
        horizontalLayout_8->setStretch(1, 2);

        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(10);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        label_2 = new QLabel(FormDialog);
        label_2->setObjectName("label_2");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy2);

        horizontalLayout_6->addWidget(label_2);

        ListCategoryDialog = new QComboBox(FormDialog);
        ListCategoryDialog->setObjectName("ListCategoryDialog");
        sizePolicy1.setHeightForWidth(ListCategoryDialog->sizePolicy().hasHeightForWidth());
        ListCategoryDialog->setSizePolicy(sizePolicy1);

        horizontalLayout_6->addWidget(ListCategoryDialog);

        horizontalLayout_6->setStretch(0, 1);
        horizontalLayout_6->setStretch(1, 2);

        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        horizontalLayout_9->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        label_6 = new QLabel(FormDialog);
        label_6->setObjectName("label_6");
        label_6->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);

        horizontalLayout_9->addWidget(label_6);

        DescriptionText = new QLineEdit(FormDialog);
        DescriptionText->setObjectName("DescriptionText");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(DescriptionText->sizePolicy().hasHeightForWidth());
        DescriptionText->setSizePolicy(sizePolicy3);
        DescriptionText->setMinimumSize(QSize(0, 20));
        DescriptionText->setMaximumSize(QSize(16777215, 16777215));
        DescriptionText->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);

        horizontalLayout_9->addWidget(DescriptionText);

        horizontalLayout_9->setStretch(0, 1);
        horizontalLayout_9->setStretch(1, 2);

        verticalLayout->addLayout(horizontalLayout_9);

        groupBox = new QGroupBox(FormDialog);
        groupBox->setObjectName("groupBox");
        horizontalLayout_7 = new QHBoxLayout(groupBox);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        DeleteButton = new QPushButton(groupBox);
        DeleteButton->setObjectName("DeleteButton");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(DeleteButton->sizePolicy().hasHeightForWidth());
        DeleteButton->setSizePolicy(sizePolicy4);
        DeleteButton->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete));
        DeleteButton->setIcon(icon);
        DeleteButton->setCheckable(false);
        DeleteButton->setAutoDefault(true);
        DeleteButton->setFlat(false);

        horizontalLayout_7->addWidget(DeleteButton);

        buttonBox = new QDialogButtonBox(groupBox);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Save);

        horizontalLayout_7->addWidget(buttonBox);


        verticalLayout->addWidget(groupBox);


        retranslateUi(FormDialog);

        DeleteButton->setDefault(false);


        QMetaObject::connectSlotsByName(FormDialog);
    } // setupUi

    void retranslateUi(QDialog *FormDialog)
    {
        FormDialog->setWindowTitle(QCoreApplication::translate("FormDialog", "Dialog", nullptr));
        TitleFormDialog->setText(QCoreApplication::translate("FormDialog", "Add New Data", nullptr));
        IncomeRadioButton->setText(QCoreApplication::translate("FormDialog", "Income", nullptr));
        ExpenseRadioButton->setText(QCoreApplication::translate("FormDialog", "Expense", nullptr));
        label_4->setText(QCoreApplication::translate("FormDialog", "Date", nullptr));
        DateTimeSelected->setDisplayFormat(QCoreApplication::translate("FormDialog", "d/M/yyyy HH:mm", nullptr));
        label_5->setText(QCoreApplication::translate("FormDialog", "Amount", nullptr));
        InputAmountText->setInputMask(QString());
        InputAmountText->setText(QString());
        InputAmountText->setPlaceholderText(QCoreApplication::translate("FormDialog", "$", nullptr));
        label_3->setText(QCoreApplication::translate("FormDialog", "Charge to", nullptr));
        label_2->setText(QCoreApplication::translate("FormDialog", "Category", nullptr));
        label_6->setText(QCoreApplication::translate("FormDialog", "Description", nullptr));
#if QT_CONFIG(accessibility)
        DeleteButton->setAccessibleName(QString());
#endif // QT_CONFIG(accessibility)
        DeleteButton->setText(QCoreApplication::translate("FormDialog", "Delete", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FormDialog: public Ui_FormDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMDIALOG_H
