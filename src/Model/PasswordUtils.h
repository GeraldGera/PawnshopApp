#pragma once

#include <QByteArray>
#include <QString>

class PasswordUtils
{
public:
    static bool isStrongPassword(const QString& password);
    static QString hashPassword(const QString& password);
    static bool verifyPassword(const QString& password, const QString& storedHash);

private:
    static QByteArray pbkdf2Sha256(const QByteArray& password, const QByteArray& salt, int iterations, int length);
    static bool constantTimeEquals(const QByteArray& left, const QByteArray& right);
};
