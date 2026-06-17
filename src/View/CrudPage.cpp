#include "View/CrudPage.h"

#include <QAbstractItemView>
#include <QDate>
#include <QHeaderView>
#include <QIcon>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QVBoxLayout>
#include <utility>

namespace
{
void setButtonIcon(QPushButton* button, const QString& themeIcon, QStyle::StandardPixmap fallback)
{
    QIcon icon = QIcon::fromTheme(themeIcon);
    if (icon.isNull()) {
        icon = button->style()->standardIcon(fallback);
    }
    button->setIcon(icon);
}
}

CrudPage::CrudPage(QString title,
                   QVector<FieldMeta> fields,
                   Loader loader,
                   Saver saver,
                   Remover remover,
                   bool canWrite,
                   QWidget* parent)
    : QWidget(parent)
    , m_title(std::move(title))
    , m_fields(std::move(fields))
    , m_loader(std::move(loader))
    , m_saver(std::move(saver))
    , m_remover(std::move(remover))
    , m_canWrite(canWrite)
{
    auto* layout = new QVBoxLayout(this);
    auto* toolbar = new QToolBar(this);
    toolbar->setMovable(false);

    auto* refreshButton = new QPushButton(QStringLiteral("Обновить"), this);
    setButtonIcon(refreshButton, QStringLiteral("view-refresh"), QStyle::SP_BrowserReload);
    connect(refreshButton, &QPushButton::clicked, this, &CrudPage::refresh);
    toolbar->addWidget(refreshButton);

    if (m_canWrite) {
        m_addButton = new QPushButton(QStringLiteral("Добавить"), this);
        m_editButton = new QPushButton(QStringLiteral("Изменить"), this);
        m_deleteButton = new QPushButton(QStringLiteral("Удалить"), this);
        setButtonIcon(m_addButton, QStringLiteral("list-add"), QStyle::SP_FileDialogNewFolder);
        setButtonIcon(m_editButton, QStringLiteral("document-edit"), QStyle::SP_FileDialogDetailedView);
        setButtonIcon(m_deleteButton, QStringLiteral("edit-delete"), QStyle::SP_DialogDiscardButton);
        connect(m_addButton, &QPushButton::clicked, this, &CrudPage::addRecord);
        connect(m_editButton, &QPushButton::clicked, this, &CrudPage::editRecord);
        connect(m_deleteButton, &QPushButton::clicked, this, &CrudPage::deleteRecord);
        toolbar->addSeparator();
        toolbar->addWidget(m_addButton);
        toolbar->addWidget(m_editButton);
        toolbar->addWidget(m_deleteButton);
    }

    m_table = new QTableWidget(this);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    connect(m_table, &QTableWidget::itemDoubleClicked, this, [this]() {
        if (m_canWrite) {
            editRecord();
        }
    });

    layout->addWidget(toolbar);
    layout->addWidget(m_table);
    refresh();
}

void CrudPage::refresh()
{
    QString error;
    m_records = m_loader ? m_loader(&error) : QVector<QVariantMap>();
    if (!error.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Ошибка загрузки"), error);
    }

    const QVector<FieldMeta> columns = visibleFields();
    m_table->clear();
    m_table->setColumnCount(columns.size());
    m_table->setRowCount(m_records.size());

    QStringList headers;
    for (const FieldMeta& field : columns) {
        headers << field.label;
    }
    m_table->setHorizontalHeaderLabels(headers);

    for (int row = 0; row < m_records.size(); ++row) {
        for (int column = 0; column < columns.size(); ++column) {
            const FieldMeta& field = columns[column];
            auto* item = new QTableWidgetItem(displayValue(field, m_records[row].value(field.key)));
            item->setData(Qt::UserRole, row);
            m_table->setItem(row, column, item);
        }
    }

    m_table->resizeColumnsToContents();
}

void CrudPage::addRecord()
{
    QVariantMap values;
    for (const FieldMeta& field : m_fields) {
        if (field.type == FieldType::Choice && !field.choices.isEmpty()) {
            values[field.key] = field.choices.first();
        } else if (field.type == FieldType::Date && field.required) {
            values[field.key] = QDate::currentDate();
        } else if (field.type == FieldType::Integer || field.type == FieldType::Decimal) {
            values[field.key] = field.minimum;
        }
    }
    values[QStringLiteral("id")] = 0;

    RecordDialog dialog(QStringLiteral("Добавить: %1").arg(m_title), m_fields, values, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QVariantMap newValues = dialog.values();
    QString error;
    if (!m_saver || !m_saver(newValues, &error)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка сохранения"), error);
        return;
    }

    refresh();
}

void CrudPage::editRecord()
{
    const int index = selectedRecordIndex();
    if (index < 0) {
        QMessageBox::information(this, QStringLiteral("Выбор записи"), QStringLiteral("Выберите запись в таблице."));
        return;
    }

    RecordDialog dialog(QStringLiteral("Изменить: %1").arg(m_title), m_fields, m_records[index], this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QVariantMap values = dialog.values();
    QString error;
    if (!m_saver || !m_saver(values, &error)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка сохранения"), error);
        return;
    }

    refresh();
}

void CrudPage::deleteRecord()
{
    const int index = selectedRecordIndex();
    if (index < 0) {
        QMessageBox::information(this, QStringLiteral("Выбор записи"), QStringLiteral("Выберите запись в таблице."));
        return;
    }

    const int id = m_records[index].value(QStringLiteral("id")).toInt();
    const auto answer = QMessageBox::question(
        this,
        QStringLiteral("Удаление"),
        QStringLiteral("Удалить запись #%1 из раздела \"%2\"?").arg(id).arg(m_title));
    if (answer != QMessageBox::Yes) {
        return;
    }

    QString error;
    if (!m_remover || !m_remover(id, &error)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка удаления"), error);
        return;
    }

    refresh();
}

int CrudPage::selectedRecordIndex() const
{
    const QList<QTableWidgetItem*> selectedItems = m_table->selectedItems();
    if (selectedItems.isEmpty()) {
        return -1;
    }
    return selectedItems.first()->data(Qt::UserRole).toInt();
}

QVector<FieldMeta> CrudPage::visibleFields() const
{
    QVector<FieldMeta> result;
    for (const FieldMeta& field : m_fields) {
        if (!field.hiddenInTable) {
            result.append(field);
        }
    }
    return result;
}

QString CrudPage::displayValue(const FieldMeta& field, const QVariant& value) const
{
    if (field.type == FieldType::Date) {
        const QDate date = value.toDate();
        return date.isValid() ? date.toString(QStringLiteral("yyyy-MM-dd")) : QString();
    }

    if (field.type == FieldType::Decimal) {
        return QString::number(value.toDouble(), 'f', 2);
    }

    if (field.type == FieldType::Password) {
        return QString();
    }

    return value.toString();
}
