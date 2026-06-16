#pragma once

#include "Model/Entities.h"

#include <QString>

class ProfileController
{
public:
    bool updateCurrentUser(UserSession& session,
                           Client updatedClient,
                           const QString& login,
                           const QString& newPassword,
                           QString* errorMessage = nullptr) const;
};
