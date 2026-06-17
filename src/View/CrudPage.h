#pragma once

#include "View/RecordDialog.h"

#include <QVariantMap>
#include <QVector>
#include <QWidget>
#include <functional>

class QPushButton;
class QTableWidget;

class CrudPage : public QWidget
{
public:
    using Loader = std::function<QVector<QVariantMap>(QString*)>;
    using Saver = std::function<bool(QVariantMap&, QString*)>;
    using Remover = std::function<bool(int, QString*)>;

    CrudPage(QString title,
             QVector<FieldMeta> fields,
             Loader loader,
             Saver saver,
             Remover remover,
             bool canWrite,
             QWidget* parent = nullptr);

    void refresh();

private:
    void addRecord();
    void editRecord();
    void deleteRecord();
    int selectedRecordIndex() const;
    QVector<FieldMeta> visibleFields() const;
    QString displayValue(const FieldMeta& field, const QVariant& value) const;

    QString m_title;
    QVector<FieldMeta> m_fields;
    Loader m_loader;
    Saver m_saver;
    Remover m_remover;
    bool m_canWrite = false;
    QVector<QVariantMap> m_records;
    QTableWidget* m_table = nullptr;
    QPushButton* m_addButton = nullptr;
    QPushButton* m_editButton = nullptr;
    QPushButton* m_deleteButton = nullptr;
};
