#pragma once

#include "Controller/DataController.h"
#include "Controller/ProfileController.h"
#include "Model/Entities.h"
#include "View/RecordDialog.h"

#include <QMainWindow>

class QLineEdit;
class QTabWidget;
class QWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(const UserSession& session, QWidget* parent = nullptr);

private:
    QWidget* createProfileTab();
    QWidget* createAdminTabs();
    void addUserTabs();

    QVector<FieldMeta> clientFields() const;
    QVector<FieldMeta> profileFields() const;
    QVector<FieldMeta> contractFields() const;
    QVector<FieldMeta> productFields() const;
    QVector<FieldMeta> pledgeFields() const;
    QVector<FieldMeta> priceFields() const;
    QVector<FieldMeta> saleFields() const;

    UserSession m_session;
    DataController m_dataController;
    ProfileController m_profileController;
    QTabWidget* m_tabs = nullptr;
};
