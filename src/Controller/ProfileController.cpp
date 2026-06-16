#include "Controller/ProfileController.h"

#include "Model/DatabaseManager.h"
#include "Model/PasswordUtils.h"
#include "Model/Repositories.h"

bool ProfileController::updateCurrentUser(UserSession& session,
                                          Client updatedClient,
                                          const QString& login,
                                          const QString& newPassword,
                                          QString* errorMessage) const
{
    if (updatedClient.lastName.trimmed().isEmpty() || updatedClient.firstName.trimmed().isEmpty()
        || updatedClient.passport.trimmed().isEmpty() || updatedClient.address.trimmed().isEmpty()
        || updatedClient.phone.trimmed().isEmpty() || login.trimmed().isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Заполните обязательные поля профиля.");
        }
        return false;
    }

    ProfileRepository profileRepository;
    if (profileRepository.loginExists(login.trimmed(), session.profile.id, errorMessage)) {
        if (errorMessage && errorMessage->isEmpty()) {
            *errorMessage = QStringLiteral("Такой логин уже занят.");
        }
        return false;
    }

    if (!newPassword.isEmpty() && !PasswordUtils::isStrongPassword(newPassword)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Новый пароль не соответствует требованиям сложности.");
        }
        return false;
    }

    auto& database = DatabaseManager::instance();
    if (!database.beginTransaction(errorMessage)) {
        return false;
    }

    ClientRepository clientRepository;
    updatedClient.id = session.client.id;
    if (!clientRepository.update(updatedClient, errorMessage)) {
        database.rollback();
        return false;
    }

    Profile updatedProfile = session.profile;
    updatedProfile.login = login.trimmed();
    const bool updatePassword = !newPassword.isEmpty();
    if (updatePassword) {
        updatedProfile.passwordHash = PasswordUtils::hashPassword(newPassword);
    }

    if (!profileRepository.update(updatedProfile, updatePassword, errorMessage)) {
        database.rollback();
        return false;
    }

    if (!database.commit(errorMessage)) {
        database.rollback();
        return false;
    }

    session.client = updatedClient;
    session.profile = updatedProfile;
    return true;
}
