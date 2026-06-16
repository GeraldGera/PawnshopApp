#pragma once

#include "Model/Entities.h"

#include <QVariantMap>
#include <QVector>

class DataController
{
public:
    explicit DataController(UserSession session);

    bool isAdmin() const;

    QVector<QVariantMap> clients(QString* errorMessage = nullptr) const;
    QVector<QVariantMap> profiles(QString* errorMessage = nullptr) const;
    QVector<QVariantMap> contracts(QString* errorMessage = nullptr) const;
    QVector<QVariantMap> products(QString* errorMessage = nullptr) const;
    QVector<QVariantMap> pledges(QString* errorMessage = nullptr) const;
    QVector<QVariantMap> prices(QString* errorMessage = nullptr) const;
    QVector<QVariantMap> sales(QString* errorMessage = nullptr) const;

    bool saveClient(QVariantMap& values, QString* errorMessage = nullptr) const;
    bool saveProfile(QVariantMap& values, QString* errorMessage = nullptr) const;
    bool saveContract(QVariantMap& values, QString* errorMessage = nullptr) const;
    bool saveProduct(QVariantMap& values, QString* errorMessage = nullptr) const;
    bool savePledge(QVariantMap& values, QString* errorMessage = nullptr) const;
    bool savePrice(QVariantMap& values, QString* errorMessage = nullptr) const;
    bool saveSale(QVariantMap& values, QString* errorMessage = nullptr) const;

    bool deleteClient(int id, QString* errorMessage = nullptr) const;
    bool deleteProfile(int id, QString* errorMessage = nullptr) const;
    bool deleteContract(int id, QString* errorMessage = nullptr) const;
    bool deleteProduct(int id, QString* errorMessage = nullptr) const;
    bool deletePledge(int id, QString* errorMessage = nullptr) const;
    bool deletePrice(int id, QString* errorMessage = nullptr) const;
    bool deleteSale(int id, QString* errorMessage = nullptr) const;

private:
    bool requireAdmin(QString* errorMessage) const;

    UserSession m_session;
};
