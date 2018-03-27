#ifndef SF_USER_H
#define SF_USER_H

#include <QObject>
#include <QAbstractListModel>


class SF_User
{
public:
    SF_User();

private:
    QString     _userName;
    QString     _avatar;
    int         _status;

};


class SF_ListUser_Model : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ListMemberRole{
        _statusRole,
        _userNameRole,
        _avatarRole,
    };
};

#endif // SF_USER_H
