#ifndef  EASYXML_H
#define  EASYXML_H


#include <QObject>
#include <QMap>
#include <QList>
#include <QTreeWidgetItem>


class EasyXml : public QObject
{
    Q_OBJECT

    public:
        static QMap<QString,QString> xmlToList(QString);
        static QList<QTreeWidgetItem *> xmlsToList(QString, QStringList);
};

#endif
