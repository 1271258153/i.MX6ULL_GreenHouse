#ifndef WRITEFILE_H
#define WRITEFILE_H

#include <QObject>

class writefile : public QObject
{
    Q_OBJECT
public:
    explicit writefile(QObject *parent = nullptr);

    int write_file_data_int32(char *FileName,int *data);
    void fan_on();
    void fan_off();
    void irr_on();
    void irr_off();
    char write_val;

signals:

};

#endif // WRITEFILE_H
