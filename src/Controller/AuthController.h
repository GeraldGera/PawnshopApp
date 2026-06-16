#pragma once

#include "Model/Entities.h"

#include <QString>

class AuthController
{
public:
    bool login(const QString& login, const QString& password, UserSession& session, QString* errorMessage = nullptr) const;
    bool registerUser(Client client, const QString& login, const QString& password, QString* errorMessage = nullptr) const;
    bool passwordIsStrong(const QString& password) const;
};
