/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created: Tue Feb 4 14:10:51 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLWIDGET_H
#define UI_CONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDockWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *dockWidgetContents;
    QGroupBox *groupBox_3;
    QPushButton *pushButtonGenerateGrid;
    QRadioButton *radioButtonGridPattern1;
    QRadioButton *radioButtonGridPattern1_2;
    QGroupBox *groupBox_6;
    QPushButton *pushButtonGenerateRadial;
    QCheckBox *checkBoxRoadTypeAvenue;
    QCheckBox *checkBoxRoadTypeLocalStreet;
    QGroupBox *groupBox_7;
    QPushButton *pushButtonGenerateRandom;
    QButtonGroup *buttonGroupGridPattern;

    void setupUi(QDockWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QString::fromUtf8("ControlWidget"));
        ControlWidget->resize(190, 831);
        ControlWidget->setMinimumSize(QSize(190, 240));
        ControlWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(181, 181, 181);"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 60, 171, 131));
        pushButtonGenerateGrid = new QPushButton(groupBox_3);
        pushButtonGenerateGrid->setObjectName(QString::fromUtf8("pushButtonGenerateGrid"));
        pushButtonGenerateGrid->setGeometry(QRect(20, 90, 131, 31));
        radioButtonGridPattern1 = new QRadioButton(groupBox_3);
        buttonGroupGridPattern = new QButtonGroup(ControlWidget);
        buttonGroupGridPattern->setObjectName(QString::fromUtf8("buttonGroupGridPattern"));
        buttonGroupGridPattern->addButton(radioButtonGridPattern1);
        radioButtonGridPattern1->setObjectName(QString::fromUtf8("radioButtonGridPattern1"));
        radioButtonGridPattern1->setGeometry(QRect(30, 20, 101, 17));
        radioButtonGridPattern1_2 = new QRadioButton(groupBox_3);
        buttonGroupGridPattern->addButton(radioButtonGridPattern1_2);
        radioButtonGridPattern1_2->setObjectName(QString::fromUtf8("radioButtonGridPattern1_2"));
        radioButtonGridPattern1_2->setGeometry(QRect(30, 40, 101, 17));
        groupBox_6 = new QGroupBox(dockWidgetContents);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 210, 171, 61));
        pushButtonGenerateRadial = new QPushButton(groupBox_6);
        pushButtonGenerateRadial->setObjectName(QString::fromUtf8("pushButtonGenerateRadial"));
        pushButtonGenerateRadial->setGeometry(QRect(20, 20, 131, 31));
        checkBoxRoadTypeAvenue = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeAvenue->setObjectName(QString::fromUtf8("checkBoxRoadTypeAvenue"));
        checkBoxRoadTypeAvenue->setGeometry(QRect(10, 10, 70, 17));
        checkBoxRoadTypeLocalStreet = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeLocalStreet->setObjectName(QString::fromUtf8("checkBoxRoadTypeLocalStreet"));
        checkBoxRoadTypeLocalStreet->setGeometry(QRect(80, 10, 91, 17));
        groupBox_7 = new QGroupBox(dockWidgetContents);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setGeometry(QRect(10, 290, 171, 61));
        pushButtonGenerateRandom = new QPushButton(groupBox_7);
        pushButtonGenerateRandom->setObjectName(QString::fromUtf8("pushButtonGenerateRandom"));
        pushButtonGenerateRandom->setGeometry(QRect(20, 20, 131, 31));
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        groupBox_3->setTitle(QApplication::translate("ControlWidget", "Grid Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateGrid->setText(QApplication::translate("ControlWidget", "Generate Grid", 0, QApplication::UnicodeUTF8));
        radioButtonGridPattern1->setText(QApplication::translate("ControlWidget", "Grid Pattern 1", 0, QApplication::UnicodeUTF8));
        radioButtonGridPattern1_2->setText(QApplication::translate("ControlWidget", "Grid Pattern 2", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("ControlWidget", "Radial Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateRadial->setText(QApplication::translate("ControlWidget", "Generate Radial", 0, QApplication::UnicodeUTF8));
        checkBoxRoadTypeAvenue->setText(QApplication::translate("ControlWidget", "Avenues", 0, QApplication::UnicodeUTF8));
        checkBoxRoadTypeLocalStreet->setText(QApplication::translate("ControlWidget", "Local Streets", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("ControlWidget", "Random Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateRandom->setText(QApplication::translate("ControlWidget", "Generate Random", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
