/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created: Mon Feb 3 21:35:26 2014
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
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *dockWidgetContents;
    QGroupBox *groupBox_3;
    QPushButton *pushButtonGenerateGrid;
    QGroupBox *groupBox_6;
    QPushButton *pushButtonGenerateRadial;
    QGroupBox *groupBox_7;
    QPushButton *pushButtonDetectGridRadial;
    QCheckBox *checkBoxRoadTypeAvenue;
    QCheckBox *checkBoxRoadTypeLocalStreet;

    void setupUi(QDockWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QString::fromUtf8("ControlWidget"));
        ControlWidget->resize(218, 831);
        ControlWidget->setMinimumSize(QSize(218, 240));
        ControlWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(181, 181, 181);"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 60, 201, 61));
        pushButtonGenerateGrid = new QPushButton(groupBox_3);
        pushButtonGenerateGrid->setObjectName(QString::fromUtf8("pushButtonGenerateGrid"));
        pushButtonGenerateGrid->setGeometry(QRect(40, 20, 121, 31));
        groupBox_6 = new QGroupBox(dockWidgetContents);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 140, 201, 61));
        pushButtonGenerateRadial = new QPushButton(groupBox_6);
        pushButtonGenerateRadial->setObjectName(QString::fromUtf8("pushButtonGenerateRadial"));
        pushButtonGenerateRadial->setGeometry(QRect(40, 20, 121, 31));
        groupBox_7 = new QGroupBox(dockWidgetContents);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setGeometry(QRect(10, 710, 201, 61));
        pushButtonDetectGridRadial = new QPushButton(groupBox_7);
        pushButtonDetectGridRadial->setObjectName(QString::fromUtf8("pushButtonDetectGridRadial"));
        pushButtonDetectGridRadial->setGeometry(QRect(40, 20, 121, 31));
        checkBoxRoadTypeAvenue = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeAvenue->setObjectName(QString::fromUtf8("checkBoxRoadTypeAvenue"));
        checkBoxRoadTypeAvenue->setGeometry(QRect(20, 10, 70, 17));
        checkBoxRoadTypeLocalStreet = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeLocalStreet->setObjectName(QString::fromUtf8("checkBoxRoadTypeLocalStreet"));
        checkBoxRoadTypeLocalStreet->setGeometry(QRect(100, 10, 91, 17));
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        groupBox_3->setTitle(QApplication::translate("ControlWidget", "Grid Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateGrid->setText(QApplication::translate("ControlWidget", "Generate Grid", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("ControlWidget", "Radial Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateRadial->setText(QApplication::translate("ControlWidget", "Generate Radial", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("ControlWidget", "Grid / Radial Detection", 0, QApplication::UnicodeUTF8));
        pushButtonDetectGridRadial->setText(QApplication::translate("ControlWidget", "Detect Grid / Radial", 0, QApplication::UnicodeUTF8));
        checkBoxRoadTypeAvenue->setText(QApplication::translate("ControlWidget", "Avenues", 0, QApplication::UnicodeUTF8));
        checkBoxRoadTypeLocalStreet->setText(QApplication::translate("ControlWidget", "Local Streets", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
