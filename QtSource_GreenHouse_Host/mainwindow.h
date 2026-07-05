#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QListWidget>
#include <QList>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QTimer>
#include <QCheckBox>
#include <QDebug>
#include <QSlider>
#include <QFile>
#include <QString>
#include <qvalueaxis.h>
#include <QValueAxis>
#include <QSplineSeries>
#include <QChart>
#include <QPushButton>
#include <QList>
#include <QChartView>
#include <QMessageBox>
#include <QUdpSocket>
#include <QFile>
#include "readfile.h"
#include "writefile.h"

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QStackedWidget *StackWidget;    /* 栈窗口 */
    QListWidget *ListWidget;    /* 左侧选项窗口 */
    QWidget *Widget[6];         /* 栈窗口 */
    QWidget *control_widget;    /* 首页实时监控环境数据窗口 */
    QLabel *soil_label; /* 土壤监控 */
    QLabel *Soil_Temp_Label;/* 温度 */
    QLabel *Soil_RH_Label;/* 湿度 */
    QLabel *Soil_Temp_Val;/* 温度 */
    QLabel *Soil_RH_Val;/* 湿度 */
    QLabel *Soil_Gread;
    QLabel *Soil_RH_Grade_Label;/* 湿度 */
    QLabel *envir_label; /* 环境监控 */
    QLabel *equip_label;/* 设备控制 */
    QLabel *Temp_Label;/* 温度 */
    QLabel *RH_Label;/* 湿度 */
    QLabel *ill_Label;/* 光照 */
    QLabel *CO2_Label;/* 二氧化碳浓度 */
    QLabel *Temp_Val;/* 温度值 */
    QLabel *RH_Val;/* 湿度值 */
    QLabel *ill_Val;/* 光照值 */
    QLabel *CO2_Val;/* 二氧化碳浓度值 */

    /* 监控数据 */
    QTimer *timer;
    unsigned char dht11_data[2]; //dht11_data[0]湿度 dht11_data[1]温度
    unsigned short als_data;
    int Soil_Temp_data;
    unsigned short CO2_data;
    int roil_RH_data;
    int roil_RH_data_int;
    int PWM_data;
    /* 设备控制 */
    QLabel *Irrigation_Label;
    QLabel *LED_Label;
    QLabel *Fan_Label;
    QLabel *Auto_Label;
    QCheckBox *Irrigation_check;
    QCheckBox *LED_check;
    QCheckBox *Fan_check;
    QCheckBox *Auto_check;

    int Irrigation_sta_Sock;
    int LED_sta_Sock;
    int Fan_sta_Sock;
    int Auto_sta_Sock;

    /* 阈值设定 */
    QWidget *Page2_Widget;
    QSlider *Temp_threshold_set;
    QSlider *RH_threshold_set;
    QSlider *ALS_threshold_set;
    QSlider *CO2_threshold_set;
    QSlider *Soil_RH_threshold_set;
    QLabel  *Temp_threshold_Val;
    QLabel  *RH_threshold_Val;
    QLabel  *ALS_threshold_Val;
    QLabel  *CO2_threshold_Val;
    QLabel  *Soil_threshold_Val;

    int save_thresgold_val[5]; //0湿度 1温度 2光照 3CO2 4土壤湿度
    QPushButton *save_val;
    /* 设备状态 */
    QWidget *equip_widget;
    QLabel *Fan_equip_sta;
    QLabel *LED_equip_sta;
    QLabel *Water_pump_equip_sta;

    /* 环境记录折线图 */
    void receiveData(int,int); //接收数据函数
    int MaxSize; //最大数据个数
    int maxX; //X轴最大数据
    int maxY; //Y轴最大数据

    QValueAxis *environment_AxisX;  // 环境温湿度X轴
    QValueAxis *environment_AxisY;  // 环境温湿度Y轴

    QList <int> en_Temp_data; //数据容器
    QList <int> en_RH_data; //数据容器
    QSplineSeries *environment_Temp_line; //环境温度折线
    QSplineSeries *environment_RH_line; //环境湿度折线
    QChart *envrionment_Chart;//环境折线图表
    QChartView *envrionment_ChartViem;
    QTimer *Chart_Timer;

    /* 土壤记录折线图 */
    void soil_receiveData(int,int); //接收数据函数
    int soil_MaxSize; //最大数据个数
    int soil_maxX; //X轴最大数据
    int soil_maxY; //Y轴最大数据

    QValueAxis *soil_AxisX;  // 环境温湿度X轴
    QValueAxis *soil_AxisY;  // 环境温湿度Y轴

    QList <int> soil_Temp_data; //数据容器
    QList <int> soil_RH_data; //数据容器
    QSplineSeries *soil_Temp_line; //环境温度折线
    QSplineSeries *soil_RH_line; //环境湿度折线
    QChart *soil_Chart;//环境折线图表
    QChartView *soil_ChartViem;

    /* 光照记录折线图 */
    void ALS_receiveData(int); //接收数据函数
    int ALS_MaxSize; //最大数据个数
    int ALS_maxX; //X轴最大数据
    int ALS_maxY; //Y轴最大数据

    QValueAxis *ALS_AxisX;  // 环境温湿度X轴
    QValueAxis *ALS_AxisY;  // 环境温湿度Y轴

    QList <int> ALS_data; //数据容器
    QSplineSeries *ALS_line; //环境温度折线
    QChart *ALS_Chart;//环境折线图表
    QChartView *ALS_ChartViem;

    /* CO2记录折线图 */
    void CO2_receiveData(int); //接收数据函数
    int CO2_MaxSize; //最大数据个数
    int CO2_maxX; //X轴最大数据
    int CO2_maxY; //Y轴最大数据

    QValueAxis *CO2_AxisX;  // 环境温湿度X轴
    QValueAxis *CO2_AxisY;  // 环境温湿度Y轴

    QList <int> CO2_Charts_data; //数据容器
    QSplineSeries *CO2_line; //环境温度折线
    QChart *CO2_Chart;//环境折线图表
    QChartView *CO2_ChartViem;

    /* 自动化 */
    QTimer *Auto_Timer;

    /* 网络 */
    QUdpSocket *server_Socket;

    /* 读写驱动文件 */
    writefile write;

    bool set;
signals:
    void to_thread_read_data();

public slots:
    void Irrigation_sta(int sta);
    void Fan_sta(int sta);
    void Led_sta(int sta);
    void Auto_sta(int sta);
    void up_data();

    /* 滑条 */
    void Temp_Slider(int val);
    void RH_Slider(int val);
    void ALS_Slider(int val);
    void CO2_Slider(int val);
    void Soil_Slider(int val);

    /* 保存数据 */
    void save_data();
    void Chart_Timer_out();

    /* 自动化 */
    void Automation();

    /* 网络接收数据 */
    void read_client_data();
};
#endif // MAINWINDOW_H
