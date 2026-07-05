#include "writefile.h"
extern "C"{
    #include <stdio.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
}
writefile::writefile(QObject *parent) : QObject(parent)
{

}
int writefile::write_file_data_int32(char *FileName,int *data)
{
    int fd = 0;
    int ret = 0;
    fd = open(FileName,O_WRONLY);
    if(fd < 0){
        printf("open %s false\n",FileName);
        return -1;
    }
    ret = write(fd,(int *)data,sizeof(data));
    if(ret < 0){
        printf("write %s false\n",FileName);
        close(fd);
        return -1;
    }
    close(fd);
}
void writefile::fan_on()
{
    write_val = 1;
    int fd;
    fd = open("/dev/fan",O_WRONLY);
    if(fd < 0){
        printf("open fan false\n");
        return;
    }
    int ret;
    ret = write(fd,&write_val,1);
    if(ret < 0){
        printf("write fan false\n");
        close(fd);
        return;
    }
    close(fd);
}
void writefile::fan_off()
{
    write_val = 0;
    int fd;
    fd = open("/dev/fan",O_WRONLY);
    if(fd < 0){
        printf("open fan false\n");
        return;
    }
    int ret;
    ret = write(fd,&write_val,1);
    if(ret < 0){
        printf("write fan false\n");
        close(fd);
        return;
    }
    close(fd);
}
void writefile::irr_on()
{
    write_val = 1;
    int fd;
    fd = open("/dev/irr",O_WRONLY);
    if(fd < 0){
        printf("open irr false\n");
        return;
    }
    int ret;
    ret = write(fd,&write_val,1);
    if(ret < 0){
        printf("write irr false\n");
        close(fd);
        return;
    }
    close(fd);
}
void writefile::irr_off()
{
    write_val = 0;
    int fd;
    fd = open("/dev/irr",O_WRONLY);
    if(fd < 0){
        printf("open irr false\n");
        return;
    }
    int ret;
    ret = write(fd,&write_val,1);
    if(ret < 0){
        printf("write irr false\n");
        close(fd);
        return;
    }
    close(fd);
}
