#ifndef READFILE_H
#define READFILE_H

#include <QObject>

class readfile : public QObject
{
    Q_OBJECT
public:
    explicit readfile(QObject *parent = nullptr);

    int read_file_data_u8(char *FileName,unsigned char *data);
    int read_file_data_u16(char *FileName,unsigned short *data);
    int read_temp_from_icm20608(char *FileName,int *data);   //icm20608温度读取函数
    int read_file_data_s16(char *FileName,short *data);
    int read_file_data_int32(char *FileName,int *data);

signals:

};

#endif // READFILE_H
