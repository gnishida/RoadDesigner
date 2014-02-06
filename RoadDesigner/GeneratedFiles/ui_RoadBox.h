/********************************************************************************
** Form generated from reading UI file 'RoadBox.ui'
**
** Created: Thu Feb 6 15:13:10 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROADBOX_H
#define UI_ROADBOX_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RoadBox
{
public:

    void setupUi(QWidget *RoadBox)
    {
        if (RoadBox->objectName().isEmpty())
            RoadBox->setObjectName(QString::fromUtf8("RoadBox"));
        RoadBox->resize(400, 300);

        retranslateUi(RoadBox);

        QMetaObject::connectSlotsByName(RoadBox);
    } // setupUi

    void retranslateUi(QWidget *RoadBox)
    {
        RoadBox->setWindowTitle(QApplication::translate("RoadBox", "RoadBox", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RoadBox: public Ui_RoadBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROADBOX_H
