#ifndef CPPBRIDGE_H
#define CPPBRIDGE_H

#include <QObject>

class CppBridge : public QObject
{
    Q_OBJECT
public:
    explicit CppBridge(QObject *parent = nullptr);

signals:

};

#endif // CPPBRIDGE_H
