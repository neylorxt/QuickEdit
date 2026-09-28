#ifndef CUSTOMFILESYSTEMMODEL_H
#define CUSTOMFILESYSTEMMODEL_H

#include <QFileSystemModel>

class CustomFileSystemModel : public QFileSystemModel
{
public:
    explicit CustomFileSystemModel(QObject *parent = nullptr)
        : QFileSystemModel(parent)
    {
    }

    void setFolderName(const QString &name)
    {
        folderName = name;

        emit headerDataChanged(Qt::Horizontal, 0, 0);
    }

    QVariant headerData(
        int section,
        Qt::Orientation orientation,
        int role = Qt::DisplayRole
        ) const override
    {
        if (orientation == Qt::Horizontal &&
            section == 0 &&
            role == Qt::DisplayRole)
        {
            return folderName;
        }

        return QFileSystemModel::headerData(section, orientation, role);
    }

private:
    QString folderName;
};

#endif // CUSTOMFILESYSTEMMODEL_H
