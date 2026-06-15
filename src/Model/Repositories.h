#pragma once

#include "Model/Entities.h"

#include <QVector>
#include <optional>

class ClientRepository
{
public:
    QVector<Client> findAll(QString* errorMessage = nullptr) const;
    std::optional<Client> findById(int id, QString* errorMessage = nullptr) const;
    bool create(Client& client, QString* errorMessage = nullptr) const;
    bool update(const Client& client, QString* errorMessage = nullptr) const;
    bool remove(int id, QString* errorMessage = nullptr) const;
};

class ProfileRepository
{
public:
    QVector<Profile> findAll(QString* errorMessage = nullptr) const;
    std::optional<Profile> findById(int id, QString* errorMessage = nullptr) const;
    bool loginExists(const QString& login, int exceptProfileId = 0, QString* errorMessage = nullptr) const;
    bool create(Profile& profile, QString* errorMessage = nullptr) const;
    bool update(const Profile& profile, bool updatePassword, QString* errorMessage = nullptr) const;
    bool remove(int id, QString* errorMessage = nullptr) const;
};

class AuthRepository
{
public:
    std::optional<UserSession> findSessionByLogin(const QString& login, QString* errorMessage = nullptr) const;
    bool createUser(Client& client, Profile& profile, QString* errorMessage = nullptr) const;
};

class ContractRepository
{
public:
    QVector<Contract> findAll(QString* errorMessage = nullptr) const;
    QVector<Contract> findByClientId(int clientId, QString* errorMessage = nullptr) const;
    bool create(Contract& contract, QString* errorMessage = nullptr) const;
    bool update(const Contract& contract, QString* errorMessage = nullptr) const;
    bool remove(int id, QString* errorMessage = nullptr) const;
};

class ProductRepository
{
public:
    QVector<Product> findAll(QString* errorMessage = nullptr) const;
    QVector<Product> findByClientId(int clientId, QString* errorMessage = nullptr) const;
    bool create(Product& product, QString* errorMessage = nullptr) const;
    bool update(const Product& product, QString* errorMessage = nullptr) const;
    bool remove(int id, QString* errorMessage = nullptr) const;
};

class PledgeRepository
{
public:
    QVector<Pledge> findAll(QString* errorMessage = nullptr) const;
    QVector<Pledge> findByClientId(int clientId, QString* errorMessage = nullptr) const;
    bool create(Pledge& pledge, QString* errorMessage = nullptr) const;
    bool update(const Pledge& pledge, QString* errorMessage = nullptr) const;
    bool remove(int id, QString* errorMessage = nullptr) const;
};

class PriceRepository
{
public:
    QVector<Price> findAll(QString* errorMessage = nullptr) const;
    QVector<Price> findByClientId(int clientId, QString* errorMessage = nullptr) const;
    bool create(Price& price, QString* errorMessage = nullptr) const;
    bool update(const Price& price, QString* errorMessage = nullptr) const;
    bool remove(int id, QString* errorMessage = nullptr) const;
};

class SaleRepository
{
public:
    QVector<Sale> findAll(QString* errorMessage = nullptr) const;
    QVector<Sale> findByClientId(int clientId, QString* errorMessage = nullptr) const;
    bool create(Sale& sale, QString* errorMessage = nullptr) const;
    bool update(const Sale& sale, QString* errorMessage = nullptr) const;
    bool remove(int id, QString* errorMessage = nullptr) const;
};
