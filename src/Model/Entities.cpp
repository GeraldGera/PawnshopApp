#include "Model/Entities.h"

QString userRoleToString(UserRole role)
{
    return role == UserRole::Admin ? QStringLiteral("admin") : QStringLiteral("user");
}

UserRole userRoleFromString(const QString& value)
{
    return value.compare(QStringLiteral("admin"), Qt::CaseInsensitive) == 0
        ? UserRole::Admin
        : UserRole::User;
}

QString Client::displayName() const
{
    return QStringLiteral("%1 %2 %3")
        .arg(lastName, firstName, middleName)
        .simplified();
}

bool UserSession::isAdmin() const
{
    return profile.role == UserRole::Admin;
}
