#include "Config/AppConfig.h"
#include "Model/DatabaseManager.h"
#include "View/LoginWindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("PawnshopApp"));
    QApplication::setOrganizationName(QStringLiteral("Coursework"));

    const DatabaseConfig config = AppConfig::loadDatabaseConfig();
    QString error;
    if (!DatabaseManager::instance().connectToDatabase(config, &error)) {
        QMessageBox::critical(
            nullptr,
            QStringLiteral("Ошибка подключения к PostgreSQL"),
            QStringLiteral("Не удалось подключиться к базе данных.\n\n%1\n\nПроверьте config.ini и наличие драйвера QPSQL.").arg(error));
        return 1;
    }

    LoginWindow window;
    window.show();
    return app.exec();
}
