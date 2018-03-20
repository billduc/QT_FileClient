#ifndef SF_FILE_H
#define SF_FILE_H

#include <QObject>
#include <QAbstractListModel>
#include <QFile>


class SF_File
{
public:
    SF_File();

private:
    QString     _filePatch;
    long        _fileSize();

};


class SF_ListFiles_Model : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ListFilesRole {
        _statusRole,
        _fileNameRole,
        _filePatch
    };
};
#endif // SF_FILE_H
