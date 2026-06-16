#include "Controller/AuthController.h"

#include "Model/PasswordUtils.h"
#include "Model/Repositories.h"

bool AuthController::login(const QString& login, const QString& password, UserSession& session, QString* errorMessage) const
{
    if (login.trimmed().isEmpty() || password.isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Введите логин и пароль.");
        }
        return false;
    }

    AuthRepository repository;
    const auto foundSession = repository.findSessionByLogin(login.trimmed(), errorMessage);
    if (!foundSession.has_value()) {
        if (errorMessage && errorMessage->isEmpty()) {
            *errorMessage = QStringLiteral("Пользователь не найден.");
        }
        return false;
    }

    if (!PasswordUtils::verifyPassword(password, foundSession->profile.passwordHash)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Неверный пароль.");
        }
        return false;
    }

    session = *foundSession;
    return true;
}

bool AuthController::registerUser(Client client, const QString& login, const QString& password, QString* errorMessage) const
{
    if (client.lastName.trimmed().isEmpty() || client.firstName.trimmed().isEmpty()
        || client.passport.trimmed().isEmpty() || client.address.trimmed().isEmpty()
        || client.phone.trimmed().isEmpty() || login.trimmed().isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Заполните обязательные регистрационные поля.");
        }
        return false;
    }

    if (!PasswordUtils::isStrongPassword(password)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Пароль должен быть не короче 8 символов и содержать цифру, заглавную букву и спецсимвол.");
        }
        return false;
    }

    ProfileRepository profileRepository;
    if (profileRepository.loginExists(login.trimmed(), 0, errorMessage)) {
        if (errorMessage && errorMessage->isEmpty()) {
            *errorMessage = QStringLiteral("Такой логин уже занят.");
        }
        return false;
    }

    Profile profile;
    profile.login = login.trimmed();
    profile.passwordHash = PasswordUtils::hashPassword(password);
    profile.role = UserRole::User;

    AuthRepository authRepository;
    return authRepository.createUser(client, profile, errorMessage);
}

bool AuthController::passwordIsStrong(const QString& password) const
{
    return PasswordUtils::isStrongPassword(password);
}
