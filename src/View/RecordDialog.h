#pragma once

#include <QDialog>
#include <QMap>
#include <QStringList>
#include <QVariantMap>
#include <QVector>

class QCheckBox;
class QWidget;

enum class FieldType
{
    Integer,
    Text,
    MultilineText,
    Decimal,
    Date,
    Choice,
    Password
};

struct FieldMeta
{
    QString key;
    QString label;
    FieldType type = FieldType::Text;
    bool required = true;
    bool readOnly = false;
    bool hiddenInTable = false;
    bool hiddenInForm = false;
    QStringList choices;
    double minimum = 0.0;
    double maximum = 1000000000.0;
};

class RecordDialog : public QDialog
{
public:
    RecordDialog(const QString& title,
                 QVector<FieldMeta> fields,
                 QVariantMap values,
                 QWidget* parent = nullptr);

    QVariantMap values() const;

protected:
    void accept() override;

private:
    QWidget* createEditor(const FieldMeta& field, const QVariant& value);
    QVariant editorValue(const FieldMeta& field) const;

    QVector<FieldMeta> m_fields;
    QVariantMap m_initialValues;
    QMap<QString, QWidget*> m_editors;
    QMap<QString, QCheckBox*> m_optionalDateChecks;
};
