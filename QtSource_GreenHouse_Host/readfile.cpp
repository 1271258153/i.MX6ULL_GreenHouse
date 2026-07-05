#include "readfile.h"

extern "C"{
    #include <stdio.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
}

readfile::readfile(QObject *parent) : QObject(parent)
{

}

int readfile::read_file_data_u8(char *FileName,unsigned char *data)
{
    int fd = 0;
    int ret = 0;
    fd = open(FileName,O_RDONLY);
    if(fd < 0){
        printf("open %s false\n",FileName);
        return -1;
    }
    ret = read(fd,data,sizeof(data));
    if(ret < 0){
        printf("read %s false\n",FileName);
        close(fd);
        return -1;
    }
    close(fd);
}
int readfile::read_file_data_u16(char *FileName,unsigned short *data)
{
    int fd = 0;
    int ret = 0;
    fd = open(FileName,O_RDONLY);
    if(fd < 0){
        printf("open %s false\n",FileName);
        return -1;
    }
    ret = read(fd,data,sizeof(data));
    if(ret < 0){
        printf("read %s false\n",FileName);
        close(fd);
        return -1;
    }
    close(fd);
}
/* icm20608温度读取函数 */
int readfile::read_temp_from_icm20608(char *FileName,int *data)
{
    int databuff[7]={0};
    int fd = 0;
    int ret = 0;
    fd = open(FileName,O_RDONLY);
    if(fd < 0){
        printf("open %s false\n",FileName);
        return -1;
    }
    ret = read(fd,databuff,sizeof(databuff));
    if(ret < 0){
        printf("read %s false\n",FileName);
        close(fd);
        return -1;
    }
    /* 把第 6 个数据“拷贝”给外部变量 */
    *data = databuff[6];   // ✅ 核心！
    close(fd);
}

int readfile::read_file_data_s16(char *FileName,short *data)
{
    int fd = 0;
    int ret = 0;
    fd = open(FileName,O_RDONLY);
    if(fd < 0){
        printf("open %s false\n",FileName);
        return -1;
    }
    ret = read(fd,data,sizeof(data));
    if(ret < 0){
        printf("read %s false\n",FileName);
        close(fd);
        return -1;
    }
    close(fd);
}
int readfile::read_file_data_int32(char *FileName,int *data)
{
    int fd = 0;
    int ret = 0;
    fd = open(FileName,O_RDONLY);
    if(fd < 0){
        printf("open %s false\n",FileName);
        return -1;
    }
    ret = read(fd,data,sizeof(data));
    if(ret < 0){
        printf("read %s false\n",FileName);
        close(fd);
        return -1;
    }
    close(fd);
}
