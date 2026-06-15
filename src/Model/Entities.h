#pragma once

#include <QDate>
#include <QString>

enum class UserRole
{
    User,
    Admin
};

QString userRoleToString(UserRole role);
UserRole userRoleFromString(const QString& value);

struct Client
{
    int id = 0;
    QString lastName;
    QString firstName;
    QString middleName;
    QString passport;
    QString address;
    QString phone;

    QString displayName() const;
};

struct Profile
{
    int id = 0;
    int clientId = 0;
    QString login;
    QString passwordHash;
    UserRole role = UserRole::User;
};

struct UserSession
{
    Client client;
    Profile profile;

    bool isAdmin() const;
};

struct Contract
{
    int id = 0;
    int clientId = 0;
    double loanAmount = 0.0;
    double commission = 0.0;
    QDate issueDate;
    QDate dueDate;
    QDate actualReturnDate;
    QString status;
};

struct Product
{
    int id = 0;
    QString name;
    QString description;
    QString status;
};

struct Pledge
{
    int id = 0;
    int contractId = 0;
    int productId = 0;
    double estimatedValue = 0.0;
};

struct Price
{
    int id = 0;
    int productId = 0;
    double price = 0.0;
    QDate priceDate;
};

struct Sale
{
    int id = 0;
    int priceId = 0;
    QDate saleDate;
};
