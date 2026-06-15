#include "Model/PasswordUtils.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QRandomGenerator>
#include <QRegularExpression>

namespace
{
constexpr int kIterations = 120000;
constexpr int kSaltSize = 16;
constexpr int kHashSize = 32;

QByteArray randomSalt()
{
    QByteArray salt;
    salt.resize(kSaltSize);
    for (int i = 0; i < salt.size(); ++i) {
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    return salt;
}

QByteArray intToBigEndian(int value)
{
    QByteArray result;
    result.resize(4);
    result[0] = static_cast<char>((value >> 24) & 0xff);
    result[1] = static_cast<char>((value >> 16) & 0xff);
    result[2] = static_cast<char>((value >> 8) & 0xff);
    result[3] = static_cast<char>(value & 0xff);
    return result;
}

QByteArray hmacSha256(const QByteArray& key, const QByteArray& message)
{
    return QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha256);
}
}

bool PasswordUtils::isStrongPassword(const QString& password)
{
    static const QRegularExpression expression(
        QStringLiteral("^(?=.*[A-Z])(?=.*\\d)(?=.*[^A-Za-z0-9]).{8,}$"));
    return expression.match(password).hasMatch();
}

QString PasswordUtils::hashPassword(const QString& password)
{
    const QByteArray salt = randomSalt();
    const QByteArray hash = pbkdf2Sha256(password.toUtf8(), salt, kIterations, kHashSize);

    return QStringLiteral("pbkdf2_sha256:%1:%2:%3")
        .arg(kIterations)
        .arg(QString::fromLatin1(salt.toBase64()))
        .arg(QString::fromLatin1(hash.toBase64()));
}

bool PasswordUtils::verifyPassword(const QString& password, const QString& storedHash)
{
    const QStringList parts = storedHash.split(QLatin1Char(':'));
    if (parts.size() != 4 || parts[0] != QStringLiteral("pbkdf2_sha256")) {
        return false;
    }

    bool ok = false;
    const int iterations = parts[1].toInt(&ok);
    if (!ok || iterations <= 0) {
        return false;
    }

    const QByteArray salt = QByteArray::fromBase64(parts[2].toLatin1());
    const QByteArray expectedHash = QByteArray::fromBase64(parts[3].toLatin1());
    const QByteArray actualHash = pbkdf2Sha256(password.toUtf8(), salt, iterations, expectedHash.size());
    return constantTimeEquals(actualHash, expectedHash);
}

QByteArray PasswordUtils::pbkdf2Sha256(const QByteArray& password, const QByteArray& salt, int iterations, int length)
{
    QByteArray derived;
    const int hashLength = 32;
    const int blockCount = (length + hashLength - 1) / hashLength;

    for (int block = 1; block <= blockCount; ++block) {
        QByteArray u = hmacSha256(password, salt + intToBigEndian(block));
        QByteArray t = u;

        for (int i = 1; i < iterations; ++i) {
            u = hmacSha256(password, u);
            for (int j = 0; j < t.size(); ++j) {
                const auto left = static_cast<unsigned char>(t.at(j));
                const auto right = static_cast<unsigned char>(u.at(j));
                t[j] = static_cast<char>(left ^ right);
            }
        }

        derived.append(t);
    }

    return derived.left(length);
}

bool PasswordUtils::constantTimeEquals(const QByteArray& left, const QByteArray& right)
{
    if (left.size() != right.size()) {
        return false;
    }

    unsigned char diff = 0;
    for (int i = 0; i < left.size(); ++i) {
        const auto leftByte = static_cast<unsigned char>(left.at(i));
        const auto rightByte = static_cast<unsigned char>(right.at(i));
        diff |= static_cast<unsigned char>(leftByte ^ rightByte);
    }
    return diff == 0;
}
