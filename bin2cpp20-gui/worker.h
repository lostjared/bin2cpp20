#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QString>

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(const QString &inputFile,
           const QString &variableName,
           bool cExpr,
           bool asString,
           bool unicode,
           bool sorted,
           QObject *parent = nullptr);

public slots:
    void doWork();

signals:
    void finished(const QString &result, const QString &error);

private:
    QString m_inputFile;
    QString m_variableName;
    bool m_cExpr;
    bool m_asString;
    bool m_unicode;
    bool m_sorted;
};

#endif
