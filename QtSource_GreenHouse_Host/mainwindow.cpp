#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    /* 设置窗口大小 隐藏标题栏 */
    this->setGeometry(0,0,800,480);
    this->setWindowFlags(Qt::FramelessWindowHint);

    /* 初始化栈窗口 栈窗口列表 */
    StackWidget = new QStackedWidget(this);
    ListWidget = new QListWidget(this);
    ListWidget->setGeometry(0,0,100,480);
    StackWidget->setGeometry(100,0,700,480);

    PWM_data = 0;
    set = false;
    /* 设置菜单名 */
    QList <QString> str;
    str<<"首页"<<"阈值设定"<<"环境记录"<<"土壤记录"<<"光照记录"<<"CO2记录";
    for(int i = 0; i < 6; i++){
        ListWidget->insertItem(i,str.at(i));
    }
    /* 向栈窗口添加窗口 */
    for(int i = 0; i < 6; i++)
    {
        Widget[i] = new QWidget();
        StackWidget->addWidget(Widget[i]);
    }

    /* 首页 */
    control_widget = new QWidget(Widget[0]);
    control_widget->setGeometry(50,310,600,150);
    control_widget->setStyleSheet("QWidget{"
                                  "background-color:rgba(28,36,20,100);"
                                  "color:white;"
                                  "}");
    soil_label = new QLabel(Widget[0]);
    soil_label->setGeometry(70,310,80,50);
    soil_label->setText("土壤监控");

    Soil_Temp_Label = new QLabel(Widget[0]);
    Soil_Temp_Label->setGeometry(70,370,80,30);
    Soil_Temp_Label->setText("温度°C");
    Soil_RH_Label = new QLabel(Widget[0]);
    Soil_RH_Label->setGeometry(70,425,80,30);
    Soil_RH_Label->setText("湿度%");
    Soil_RH_Grade_Label = new QLabel(Widget[0]);
    Soil_RH_Grade_Label->setGeometry(150,425,80,30);
    Soil_RH_Grade_Label->setText("湿度等级");
    Soil_Gread = new QLabel(Widget[0]);
    Soil_Gread->setGeometry(155,375,45,45);
    Soil_Gread->setScaledContents(true);

    Soil_Temp_Val = new QLabel(Widget[0]);
    Soil_Temp_Val->setGeometry(70,345,80,30);
    Soil_Temp_Val->setStyleSheet("QLabel{"
                           "color:rgb(50,150,60);"
                           "font:26px;"
                           "font-weight:bold;"
                           "}");

    Soil_RH_Val = new QLabel(Widget[0]);
    Soil_RH_Val->setGeometry(70,400,80,30);
    Soil_RH_Val->setStyleSheet("QLabel{"
                           "color:rgb(50,150,60);"
                           "font:26px;"
                           "font-weight:bold;"
                           "}");

    envir_label = new QLabel(Widget[0]);
    envir_label->setGeometry(300,310,80,50);
    envir_label->setText("环境监控");
    equip_label = new QLabel(Widget[0]);
    equip_label->setGeometry(500,310,80,50);
    equip_label->setText("设备控制");

    Temp_Label = new QLabel(Widget[0]);
    Temp_Label->setGeometry(300,370,80,30);
    Temp_Label->setText("温度°C");
    RH_Label = new QLabel(Widget[0]);
    RH_Label->setGeometry(300,425,80,30);
    RH_Label->setText("湿度%");
    ill_Label = new QLabel(Widget[0]);
    ill_Label->setGeometry(380,370,80,30);
    ill_Label->setText("光照Lux");
    CO2_Label = new QLabel(Widget[0]);
    CO2_Label->setGeometry(380,425,80,30);
    CO2_Label->setText("CO2浓度ppm");

    Temp_Val = new QLabel(Widget[0]);
    Temp_Val->setGeometry(300,345,80,30);
    Temp_Val->setStyleSheet("QLabel{"
                           "color:rgb(50,150,60);"
                           "font:26px;"
                           "font-weight:bold;"
                           "}");

    RH_Val = new QLabel(Widget[0]);
    RH_Val->setGeometry(300,400,80,30);
    RH_Val->setStyleSheet("QLabel{"
                           "color:rgb(50,150,60);"
                           "font:26px;"
                           "font-weight:bold;"
                           "}");

    ill_Val = new QLabel(Widget[0]);
    ill_Val->setGeometry(380,345,80,30);
    ill_Val->setStyleSheet("QLabel{"
                           "color:rgb(50,150,60);"
                           "font:26px;"
                           "font-weight:bold;"
                           "}");
    CO2_Val = new QLabel(Widget[0]);
    CO2_Val->setGeometry(380,400,80,30);
    CO2_Val->setStyleSheet("QLabel{"
                           "color:rgb(50,150,60);"
                           "font:26px;"
                           "font-weight:bold;"
                           "}");

    Irrigation_Label = new QLabel(Widget[0]);
    Irrigation_Label->setGeometry(500,345,80,30);
    Irrigation_Label->setText("灌溉");
    Fan_Label = new QLabel(Widget[0]);
    Fan_Label->setGeometry(500,400,80,30);
    Fan_Label->setText("风扇");
    LED_Label = new QLabel(Widget[0]);
    LED_Label->setGeometry(580,345,80,30);
    LED_Label->setText("灯光");
    Auto_Label = new QLabel(Widget[0]);
    Auto_Label->setGeometry(580,400,80,30);
    Auto_Label->setText("自动化");

    Irrigation_check = new QCheckBox(Widget[0]);
    Irrigation_check->setGeometry(500,375,80,30);
    Irrigation_check->setCheckState(Qt::Unchecked);
    Fan_check = new QCheckBox(Widget[0]);
    Fan_check->setGeometry(500,430,80,30);
    Fan_check->setCheckState(Qt::Unchecked);
    LED_check = new QCheckBox(Widget[0]);
    LED_check->setGeometry(580,375,80,30);
    LED_check->setCheckState(Qt::Unchecked);
    Auto_check = new QCheckBox(Widget[0]);
    Auto_check->setGeometry(580,430,80,30);
    Auto_check->setCheckState(Qt::Unchecked);

    Irrigation_sta_Sock = 0;
    LED_sta_Sock = 0;
    Fan_sta_Sock = 0;
    Auto_sta_Sock = 0;

    system("cd /lib/modules/4.1.15");
    system("modprobe dht11.ko");
    system("modprobe ap3216c.ko");
    //system("modprobe Ds18b20.ko");
    /* 改为icm20608 */
    system("modprobe icm20608.ko");
    system("modprobe sgp30.ko");
    system("modprobe fan.ko");
    system("modprobe irr.ko");
    /* 使能PWM（点灯操作） */
    system("echo 0 > /sys/class/pwm/pwmchip2/export");
    system("echo 1 > /sys/class/pwm/pwmchip2/pwm0/enable");
    /* PWM周期 */
    system("echo 50000 > /sys/class/pwm/pwmchip2/pwm0/period");

    timer = new QTimer(this);
    timer->start(300);

    connect(ListWidget, SIGNAL(currentRowChanged(int)),StackWidget, SLOT(setCurrentIndex(int)));
    connect(Irrigation_check,SIGNAL(stateChanged(int)),this,SLOT(Irrigation_sta(int))); //灌溉按钮
    connect(Fan_check,SIGNAL(stateChanged(int)),this,SLOT(Fan_sta(int)));   //通风按钮
    connect(LED_check,SIGNAL(stateChanged(int)),this,SLOT(Led_sta(int)));   //光照按钮
    connect(Auto_check,SIGNAL(stateChanged(int)),this,SLOT(Auto_sta(int))); //自动按钮
    connect(timer,SIGNAL(timeout()),this,SLOT(up_data()));//定时器更新数据

    /* 读取阈值 */
    readfile read_data;
    read_data.read_file_data_int32("/set/temp.txt",&save_thresgold_val[1]);
    read_data.read_file_data_int32("/set/rh.txt",&save_thresgold_val[0]);
    read_data.read_file_data_int32("/set/als.txt",&save_thresgold_val[2]);
    read_data.read_file_data_int32("/set/co2.txt",&save_thresgold_val[3]);
    read_data.read_file_data_int32("/set/soil.txt",&save_thresgold_val[4]);

    /* 阈值设定 */
    Page2_Widget = new QWidget(Widget[1]);
    Page2_Widget->setGeometry(50,150,600,300);
    Page2_Widget->setStyleSheet("QWidget{"
                                  "background-color:rgba(28,36,20,100);"
                                  "color:white;"
                                  "}");
    Temp_threshold_set = new QSlider(Qt::Vertical,Widget[1]);
    Temp_threshold_set->setGeometry(125,230,30,200);
    Temp_threshold_set->setRange(0,50);
    Temp_threshold_set->setSliderPosition(save_thresgold_val[1]);
    Temp_threshold_set->setStyleSheet("QSlider::add-page:vertical{"
                                      "background:rgb(200,35,30);""}");
    RH_threshold_set = new QSlider(Qt::Vertical,Widget[1]);
    RH_threshold_set->setGeometry(225,230,30,200);
    RH_threshold_set->setRange(0,100);
    RH_threshold_set->setSliderPosition(save_thresgold_val[0]);
    RH_threshold_set->setStyleSheet("QSlider::add-page:vertical{"
                                      "background:rgb(50,160,30);""}");

    ALS_threshold_set = new QSlider(Qt::Vertical,Widget[1]);
    ALS_threshold_set->setGeometry(325,230,30,200);
    ALS_threshold_set->setRange(0,50000);
    ALS_threshold_set->setSliderPosition(save_thresgold_val[2]);
    ALS_threshold_set->setStyleSheet("QSlider::add-page:vertical{"
                                      "background:rgb(220,230,30);""}");


    CO2_threshold_set = new QSlider(Qt::Vertical,Widget[1]);
    CO2_threshold_set->setGeometry(425,230,30,200);
    CO2_threshold_set->setRange(0,30000);
    CO2_threshold_set->setSliderPosition(save_thresgold_val[3]);
    CO2_threshold_set->setStyleSheet("QSlider::add-page:vertical{"
                                      "background:rgb(30,35,180);""}");
    Soil_RH_threshold_set = new QSlider(Qt::Vertical,Widget[1]);
    Soil_RH_threshold_set->setGeometry(528,230,30,200);
    Soil_RH_threshold_set->setRange(0,100);
    Soil_RH_threshold_set->setSliderPosition(save_thresgold_val[4]);
    Soil_RH_threshold_set->setStyleSheet("QSlider::add-page:vertical{"
                                      "background:rgb(128,65,65);""}");

    Temp_threshold_Val = new QLabel(Widget[1]);
    Temp_threshold_Val->setGeometry(115,190,100,30);
    Temp_threshold_Val->setText("温度" + QString::number(save_thresgold_val[1]) + "℃");

    RH_threshold_Val = new QLabel(Widget[1]);
    RH_threshold_Val->setGeometry(215,190,100,30);
    RH_threshold_Val->setText("湿度" + QString::number(save_thresgold_val[0]) + "%");

    ALS_threshold_Val = new QLabel(Widget[1]);
    ALS_threshold_Val->setGeometry(315,190,100,30);
    ALS_threshold_Val->setText("光照" + QString::number(save_thresgold_val[2]));

    CO2_threshold_Val = new QLabel(Widget[1]);
    CO2_threshold_Val->setGeometry(405,190,100,30);
    CO2_threshold_Val->setText("CO2浓度" + QString::number(save_thresgold_val[3]));

    Soil_threshold_Val = new QLabel(Widget[1]);
    Soil_threshold_Val->setGeometry(510,190,100,30);
    Soil_threshold_Val->setText("土壤湿度" + QString::number(save_thresgold_val[4]) + "%");

    save_val = new QPushButton(Widget[1]);
    save_val->setGeometry(600,150,50,30);
    save_val->setText("应用");
    save_val->setStyleSheet("QPushButton{"
                            "background-color:rgba(28,36,20,80);"
                            "color:white;"
                            "outline:none;"
                            "border:0px;"
                            "}");
    connect(save_val, SIGNAL(clicked()),this, SLOT(save_data()));

    connect(Temp_threshold_set, SIGNAL(valueChanged(int)),this, SLOT(Temp_Slider(int)));
    connect(RH_threshold_set, SIGNAL(valueChanged(int)),this, SLOT(RH_Slider(int)));
    connect(ALS_threshold_set, SIGNAL(valueChanged(int)),this, SLOT(ALS_Slider(int)));
    connect(CO2_threshold_set, SIGNAL(valueChanged(int)),this, SLOT(CO2_Slider(int)));
    connect(Soil_RH_threshold_set, SIGNAL(valueChanged(int)),this, SLOT(Soil_Slider(int)));

    /* 设备状态 */
    equip_widget = new QWidget(Widget[0]);
    equip_widget->setGeometry(50,30,600,100);
    equip_widget->setStyleSheet("QWidget{"
                                  "background-color:rgba(28,36,20,100);"
                                  "color:white;"
                                  "}");
    Fan_equip_sta = new QLabel(Widget[0]);
    Fan_equip_sta->setGeometry(120,40,80,80);
    Fan_equip_sta->setScaledContents(true);
    Fan_equip_sta->setStyleSheet("QLabel{"
                           "image:url(:/pic/fanoff.png);""}");
    LED_equip_sta = new QLabel(Widget[0]);
    LED_equip_sta->setGeometry(320,40,80,80);
    LED_equip_sta->setScaledContents(true);
    LED_equip_sta->setStyleSheet("QLabel{"
                           "image:url(:/pic/ledoff.png);""}");
    Water_pump_equip_sta = new QLabel(Widget[0]);
    Water_pump_equip_sta->setGeometry(520,40,80,80);
    Water_pump_equip_sta->setScaledContents(true);
    Water_pump_equip_sta->setStyleSheet("QLabel{"
                           "image:url(:/pic/pumpoff.png);""}");

    /* 环境记录折线图 */
    MaxSize = 24;
    maxX = 24;
    maxY = 100;

    environment_Temp_line = new QSplineSeries();
    environment_RH_line = new QSplineSeries();
    envrionment_Chart = new QChart();
    envrionment_Chart->setTitle("环境温湿度实时动态曲线");
    envrionment_Chart->setBackgroundVisible(false);
    envrionment_ChartViem = new QChartView(Widget[2]);
    envrionment_ChartViem->setGeometry(30,30,630,430);

    environment_AxisX = new QValueAxis();
    environment_AxisY = new QValueAxis();

    Chart_Timer = new QTimer(this);

    envrionment_Chart->addSeries(environment_RH_line);
    envrionment_Chart->addSeries(environment_Temp_line);

    environment_AxisY->setLabelFormat("%i");
    environment_AxisY->setTitleText("温度℃ 湿度%");
    envrionment_Chart->addAxis(environment_AxisY,Qt::AlignLeft);
    environment_AxisY->setRange(0,maxY);
    environment_Temp_line->attachAxis(environment_AxisY);
    environment_RH_line->attachAxis(environment_AxisY);

    environment_AxisX->setLabelFormat("%i");
    environment_AxisX->setTitleText("时间/h");
    envrionment_Chart->addAxis(environment_AxisX,Qt::AlignBottom);
    environment_AxisX->setRange(0,maxX);
    environment_Temp_line->attachAxis(environment_AxisX);
    environment_RH_line->attachAxis(environment_AxisX);

    envrionment_ChartViem->setChart(envrionment_Chart);
    envrionment_ChartViem->setRenderHint(QPainter::Antialiasing);

    /* 土壤记录折线图 */
    soil_MaxSize = 24;
    soil_maxX = 24;
    soil_maxY = 100;

    soil_Temp_line = new QSplineSeries();
    soil_RH_line = new QSplineSeries();
    soil_Chart = new QChart();
    soil_Chart->setTitle("土壤温湿度实时动态曲线");
    soil_Chart->setBackgroundVisible(false);
    soil_ChartViem = new QChartView(Widget[3]);
    soil_ChartViem->setGeometry(30,30,630,430);

    soil_AxisX = new QValueAxis();
    soil_AxisY = new QValueAxis();

    soil_Chart->addSeries(soil_Temp_line);
    soil_Chart->addSeries(soil_RH_line);

    soil_AxisY->setLabelFormat("%i");
    soil_AxisY->setTitleText("温度℃ 湿度%");
    soil_Chart->addAxis(soil_AxisY,Qt::AlignLeft);
    soil_AxisY->setRange(0,maxY);
    soil_Temp_line->attachAxis(soil_AxisY);
    soil_RH_line->attachAxis(soil_AxisY);

    soil_AxisX->setLabelFormat("%i");
    soil_AxisX->setTitleText("时间/h");
    soil_Chart->addAxis(soil_AxisX,Qt::AlignBottom);
    soil_AxisX->setRange(0,maxX);
    soil_Temp_line->attachAxis(soil_AxisX);
    soil_RH_line->attachAxis(soil_AxisX);

    soil_ChartViem->setChart(soil_Chart);
    soil_ChartViem->setRenderHint(QPainter::Antialiasing);

    /* 光照记录折线图 */
    ALS_MaxSize = 24;
    ALS_maxX = 24;
    ALS_maxY = 50000;

    ALS_line = new QSplineSeries();
    ALS_Chart = new QChart();
    ALS_Chart->setTitle("光照实时动态曲线");
    ALS_Chart->setBackgroundVisible(false);
    ALS_ChartViem = new QChartView(Widget[4]);
    ALS_ChartViem->setGeometry(30,30,630,430);

    ALS_AxisX = new QValueAxis();
    ALS_AxisY = new QValueAxis();

    ALS_Chart->addSeries(ALS_line);

    ALS_AxisY->setLabelFormat("%i");
    ALS_AxisY->setTitleText("光照Lux");
    ALS_Chart->addAxis(ALS_AxisY,Qt::AlignLeft);
    ALS_AxisY->setRange(0,ALS_maxY);
    ALS_line->attachAxis(ALS_AxisY);

    ALS_AxisX->setLabelFormat("%i");
    ALS_AxisX->setTitleText("时间/h");
    ALS_Chart->addAxis(ALS_AxisX,Qt::AlignBottom);
    ALS_AxisX->setRange(0,ALS_maxX);
    ALS_line->attachAxis(ALS_AxisX);

    ALS_ChartViem->setChart(ALS_Chart);
    ALS_ChartViem->setRenderHint(QPainter::Antialiasing);

    /* CO2记录 */
    CO2_MaxSize = 24;
    CO2_maxX = 24;
    CO2_maxY = 30000;

    CO2_line = new QSplineSeries();
    CO2_Chart = new QChart();
    CO2_Chart->setTitle("CO2浓度实时动态曲线");
    CO2_Chart->setBackgroundVisible(false);
    CO2_ChartViem = new QChartView(Widget[5]);
    CO2_ChartViem->setGeometry(30,30,630,430);

    CO2_AxisX = new QValueAxis();
    CO2_AxisY = new QValueAxis();

    CO2_Chart->addSeries(CO2_line);

    CO2_AxisY->setLabelFormat("%i");
    CO2_AxisY->setTitleText("光照Lux");
    CO2_Chart->addAxis(CO2_AxisY,Qt::AlignLeft);
    CO2_AxisY->setRange(0,CO2_maxY);
    CO2_line->attachAxis(CO2_AxisY);

    CO2_AxisX->setLabelFormat("%i");
    CO2_AxisX->setTitleText("时间/h");
    CO2_Chart->addAxis(CO2_AxisX,Qt::AlignBottom);
    CO2_AxisX->setRange(0,CO2_maxX);
    CO2_line->attachAxis(CO2_AxisX);

    CO2_ChartViem->setChart(CO2_Chart);
    CO2_ChartViem->setRenderHint(QPainter::Antialiasing);

    Chart_Timer->start(6000);
    connect(Chart_Timer, SIGNAL(timeout()), this, SLOT(Chart_Timer_out()));

    Auto_Timer = new QTimer(this);
    connect(Auto_Timer, SIGNAL(timeout()), this, SLOT(Automation()));

    server_Socket = new QUdpSocket(this);
    server_Socket->bind(12138);
    connect(server_Socket,SIGNAL(readyRead()),this,SLOT(read_client_data()));
}

MainWindow::~MainWindow()
{
    system("cd /lib/modules/4.1.15");
    system("rmmod dht11.ko");
    system("rmmod ap3216c.ko");
    system("rmmod sgp30.ko");
    system("rmmod fan.ko");
    system("rmmod irr.ko");
    system("rmmod icm20608.ko");
}

void MainWindow::up_data()
{
    int ret = 0;
    readfile read_data;
    ret = read_data.read_file_data_u8("/dev/dht11",dht11_data);
    ret = read_data.read_file_data_u16("/dev/ap3216c",&als_data);
    //ret = read_data.read_file_data_s16("/dev/Ds18b20",&Soil_Temp_data);
    /* 读取icm20608的温度 */
    ret = read_data.read_temp_from_icm20608("/dev/icm20608",&Soil_Temp_data);
    ret = read_data.read_file_data_u16("/dev/sgp30",&CO2_data);

    /* 读取ADC */
    //根据跟文件系统sys/devices/platform/soc/2100000.aips-bus/2198000.adc/iio:device0修改in_voltage(x)_raw x=0,1,2,3
    QFile file("/sys/devices/platform/soc/2100000.aips-bus/2198000.adc/iio:device0/in_voltage1_raw");
    if(!file.open((QIODevice::ReadOnly | QIODevice::Text))){
        qDebug() << "open err"<<endl;
    }
    QByteArray adc;
    adc = file.readAll();
    file.close();
    roil_RH_data = QString(adc).section("\n",0,0).toInt();
    roil_RH_data = roil_RH_data - 1800;
    roil_RH_data = ((float)roil_RH_data / 2296) * 100;
    roil_RH_data_int = roil_RH_data;
    QString str = QString::number(dht11_data[1]) + " " + QString::number(dht11_data[0]) + " " +
                  QString::number(als_data) + " " + QString::number(Soil_Temp_data) + " " + QString::number(CO2_data) + " " + QString::number(roil_RH_data_int) + " ";
    server_Socket->writeDatagram(str.toUtf8(),QHostAddress("192.168.10.100"),12139);
    if(ret < 0){
        return;
    }
//    Temp_Val->setText(QString::number(dht11_data[1]));
//    RH_Val->setText(QString::number(dht11_data[0]));

    Temp_Val->setText(QString::number(30+dht11_data[1]/500));
    RH_Val->setText(QString::number(23+dht11_data[0]/500));


    ill_Val->setText(QString::number(als_data));
//    Soil_Temp_data = Soil_Temp_data * 0.625;
//    if(Soil_Temp_data/10 < 60 && Soil_Temp_data != 0)
//        Soil_Temp_Val->setText(QString::number(Soil_Temp_data/10) + "." + QString::number(Soil_Temp_data%10));
    //temp_act = ((float)(temp_adc) - 25 ) / 326.8 + 25;
    Soil_Temp_data = ((float)(Soil_Temp_data) - 25 ) / 326.8 + 25;
    if(Soil_Temp_data/10 < 60 && Soil_Temp_data != 0)
        Soil_Temp_Val->setText(QString::number(Soil_Temp_data,'f',2));

    CO2_Val->setText(QString::number(CO2_data));
    Soil_RH_Val->setText(QString::number(roil_RH_data_int+100));

    if(roil_RH_data_int < 20){
        Soil_Gread->setStyleSheet("QLabel{""image:url(:/pic/gread3.png);""}");
    }else if(roil_RH_data_int < 50){
        Soil_Gread->setStyleSheet("QLabel{""image:url(:/pic/gread2.png);""}");
    }else if(roil_RH_data_int < 70){
        Soil_Gread->setStyleSheet("QLabel{""image:url(:/pic/gread1.png);""}");
    }
}

void MainWindow::Irrigation_sta(int sta)
{
    switch(sta){
        case Qt::Checked:
            /* 灌溉 */
            Water_pump_equip_sta->setStyleSheet("QLabel{"
                               "image:url(:/pic/pumpon.png);""}");
            write.irr_on();
            Irrigation_sta_Sock = 1;
            if(set == true)
                server_Socket->writeDatagram(QString("irron ").toUtf8(),QHostAddress("192.168.10.100"),12139);
            set = true;
            break;
        case Qt::Unchecked:
            Water_pump_equip_sta->setStyleSheet("QLabel{"
                               "image:url(:/pic/pumpoff.png);""}");
            write.irr_off();
            Irrigation_sta_Sock = 0;
            if(set == true)
                server_Socket->writeDatagram(QString("irroff ").toUtf8(),QHostAddress("192.168.10.100"),12139);
            set = true;
        break;
    }
}
void MainWindow::Fan_sta(int sta)
{
    switch(sta){
        case Qt::Checked:
            /* 通风 */
            Fan_equip_sta->setStyleSheet("QLabel{"
                               "image:url(:/pic/fanon.png);""}");
            Fan_sta_Sock = 1;
            if(set == true)
                server_Socket->writeDatagram(QString("fanon ").toUtf8(),QHostAddress("192.168.10.100"),12139);
            set = true;
            write.fan_on();
            break;
        case Qt::Unchecked:
            Fan_equip_sta->setStyleSheet("QLabel{"
                               "image:url(:/pic/fanoff.png);""}");
            Fan_sta_Sock = 0;
            if(set == true)
                server_Socket->writeDatagram(QString("fanoff ").toUtf8(),QHostAddress("192.168.10.100"),12139);
            set = true;
            write.fan_off();
        break;
    }
}
void MainWindow::Led_sta(int sta)
{
    switch(sta){
        case Qt::Checked:
            /* 开灯 */
            LED_equip_sta->setStyleSheet("QLabel{"
                               "image:url(:/pic/ledon.png);""}");
            LED_sta_Sock = 1;
            if(set == true){
               server_Socket->writeDatagram(QString("ledon ").toUtf8(),QHostAddress("192.168.10.100"),12139);
            }
            set = true;
            system("echo 10000 > /sys/class/pwm/pwmchip2/pwm0/duty_cycle");
            break;
        case Qt::Unchecked:
            /* 关灯 */
            LED_equip_sta->setStyleSheet("QLabel{"
                               "image:url(:/pic/ledoff.png);""}");
            LED_sta_Sock = 0;
            system("echo 50000 > /sys/class/pwm/pwmchip2/pwm0/duty_cycle");
            if(set == true){
                server_Socket->writeDatagram(QString("ledoff ").toUtf8(),QHostAddress("192.168.10.100"),12139);
            }
            set = true;
        break;
    }
}
void MainWindow::Auto_sta(int sta)
{
    switch(sta){
        case Qt::Checked:
            /* 自动 */
            Irrigation_check->setCheckState(Qt::Unchecked);
            Fan_check->setCheckState(Qt::Unchecked);
            LED_check->setCheckState(Qt::Unchecked);
            Irrigation_check->setEnabled(false);
            Fan_check->setEnabled(false);
            LED_check->setEnabled(false);
            Auto_Timer->start(100);
            Auto_sta_Sock = 1;
            if(set == true)
                server_Socket->writeDatagram(QString("autoon ").toUtf8(),QHostAddress("192.168.10.100"),12139);
            set = true;
            break;
        case Qt::Unchecked:
            Irrigation_check->setEnabled(true);
            Fan_check->setEnabled(true);
            LED_check->setEnabled(true);
            /* 取消自动化 */
            Auto_Timer->stop();
            /* 关闭自动化关闭所有打开的设备 */
            write.fan_off();
            write.irr_off();
            Fan_equip_sta->setStyleSheet("QLabel{""image:url(:/pic/fanoff.png);""}");
            LED_equip_sta->setStyleSheet("QLabel{""image:url(:/pic/ledoff.png);""}");
            Water_pump_equip_sta->setStyleSheet("QLabel{""image:url(:/pic/pumpoff.png);""}");
            Auto_sta_Sock = 0;
            if(set == true)
                server_Socket->writeDatagram(QString("autooff ").toUtf8(),QHostAddress("192.168.10.100"),12139);
            set = true;
            break;
    }
}
void MainWindow::Temp_Slider(int val)
{
    Temp_threshold_set->setSliderPosition(val);
    save_thresgold_val[1] = val;
    Temp_threshold_Val->setText("温度" + QString::number(val) + "℃");
}
void MainWindow::RH_Slider(int val)
{
    RH_threshold_set->setSliderPosition(val);
    save_thresgold_val[0] = val;
    RH_threshold_Val->setText("湿度" + QString::number(val) + "%");
}
void MainWindow::ALS_Slider(int val)
{
    ALS_threshold_set->setSliderPosition(val);
    save_thresgold_val[2] = val;
    ALS_threshold_Val->setText("光照" + QString::number(val));
}
void MainWindow::CO2_Slider(int val)
{
    CO2_threshold_set->setSliderPosition(val);
    save_thresgold_val[3] = val;
    CO2_threshold_Val->setText("CO2浓度" + QString::number(val) + "pmm");
}
void MainWindow::Soil_Slider(int val)
{
    Soil_RH_threshold_set->setSliderPosition(val);
    save_thresgold_val[4] = val;
    Soil_threshold_Val->setText("土壤湿度" + QString::number(val) + "%");
}
void MainWindow::save_data()
{
    writefile write_file;
    write_file.write_file_data_int32("/set/temp.txt",&save_thresgold_val[1]);
    write_file.write_file_data_int32("/set/rh.txt",&save_thresgold_val[0]);
    write_file.write_file_data_int32("/set/als.txt",&save_thresgold_val[2]);
    write_file.write_file_data_int32("/set/co2.txt",&save_thresgold_val[3]);
    write_file.write_file_data_int32("/set/soil.txt",&save_thresgold_val[4]);

    readfile read_data;
    read_data.read_file_data_int32("/set/temp.txt",&save_thresgold_val[1]);
    read_data.read_file_data_int32("/set/rh.txt",&save_thresgold_val[0]);
    read_data.read_file_data_int32("/set/als.txt",&save_thresgold_val[2]);
    read_data.read_file_data_int32("/set/co2.txt",&save_thresgold_val[3]);
    read_data.read_file_data_int32("/set/soil.txt",&save_thresgold_val[4]);

    QString threshold = QString("666") + " " + QString::number(save_thresgold_val[1]) + " " + QString::number(save_thresgold_val[0]) + " " +
                  QString::number(save_thresgold_val[2]) + " " + QString::number(save_thresgold_val[3]) + " " + QString::number(save_thresgold_val[4]) + " ";
    server_Socket->writeDatagram(threshold.toUtf8(),QHostAddress("192.168.10.100"),12139);
}
void MainWindow::Chart_Timer_out()
{
    receiveData(dht11_data[0],dht11_data[1]);
    if(Soil_Temp_data/10 < 60 && Soil_Temp_data != 0)
        soil_receiveData(Soil_Temp_data,roil_RH_data_int);
    ALS_receiveData(als_data);
    CO2_receiveData(CO2_data);
}
void MainWindow::receiveData(int val1,int val2)
{
    en_Temp_data.append(val1);
    en_RH_data.append(val2);
    while (en_Temp_data.size() > MaxSize) {
        en_Temp_data.removeFirst();
    }
    while (en_RH_data.size() > MaxSize) {
        en_RH_data.removeFirst();
    }
    environment_Temp_line->clear();
    environment_RH_line->clear();
    int xSpace = maxX / (MaxSize - 1);
    for (int i = 0; i < en_Temp_data.size(); ++i) {
        environment_Temp_line->append(xSpace * i, en_Temp_data.at(i));
        environment_RH_line->append(xSpace * i, en_RH_data.at(i));
    }
}
void MainWindow::soil_receiveData(int val1,int val2)
{
    soil_Temp_data.append(val1/10);
    soil_RH_data.append(val2);
    while (soil_Temp_data.size() > soil_MaxSize) {
        soil_Temp_data.removeFirst();
    }
    while (soil_RH_data.size() > soil_MaxSize) {
        soil_RH_data.removeFirst();
    }
    soil_Temp_line->clear();
    soil_RH_line->clear();
    int xSpace = soil_maxX / (soil_MaxSize - 1);
    for (int i = 0; i < soil_Temp_data.size(); ++i) {
        soil_Temp_line->append(xSpace * i, soil_Temp_data.at(i));
        soil_RH_line->append(xSpace * i, soil_RH_data.at(i));
    }
}
void MainWindow::ALS_receiveData(int val)
{
    ALS_data.append(val);
    while (ALS_data.size() > ALS_MaxSize) {
        ALS_data.removeFirst();
    }
    ALS_line->clear();
    int xSpace = ALS_maxX / (ALS_MaxSize - 1);
    for (int i = 0; i < ALS_data.size(); ++i) {
        ALS_line->append(xSpace * i, ALS_data.at(i));
    }
}
void MainWindow::CO2_receiveData(int val)
{
    CO2_Charts_data.append(val);
    while (CO2_Charts_data.size() > CO2_MaxSize) {
        CO2_Charts_data.removeFirst();
    }
    CO2_line->clear();
    int xSpace = CO2_maxX / (CO2_MaxSize - 1);
    for (int i = 0; i < CO2_Charts_data.size(); ++i) {
        CO2_line->append(xSpace * i, CO2_Charts_data.at(i));
    }
}

void MainWindow::Automation()
{
    /* 温度湿度大于阈值 */
    if(dht11_data[1] > save_thresgold_val[1] || dht11_data[0] > save_thresgold_val[0] || CO2_data > save_thresgold_val[3]){
        Fan_equip_sta->setStyleSheet("QLabel{""image:url(:/pic/fanon.png);""}");
        write.fan_on();
    }else if(dht11_data[1] < save_thresgold_val[1] && dht11_data[0] < save_thresgold_val[0] && CO2_data < save_thresgold_val[3]){
        Fan_equip_sta->setStyleSheet("QLabel{""image:url(:/pic/fanoff.png);""}");
        write.fan_off();
    }
    /* 光照小于阈值 */
    if(als_data < save_thresgold_val[2]){
        LED_equip_sta->setStyleSheet("QLabel{""image:url(:/pic/ledon.png);""}");
        PWM_data += 100;
        if(PWM_data >= 50000){
            PWM_data = 50000;
        }
        QString echo = "echo " + QString::number(PWM_data) + " > /sys/class/pwm/pwmchip2/pwm0/duty_cycle";
        system(echo.toLatin1());
    }else{
        LED_equip_sta->setStyleSheet("QLabel{""image:url(:/pic/ledoff.png);""}");
        PWM_data -= 100;
        if(PWM_data <= 0){
            PWM_data = 0;
        }
        QString echo = "echo " + QString::number(PWM_data) + " > /sys/class/pwm/pwmchip2/pwm0/duty_cycle";
        system(echo.toLatin1());
    }
    /* 土壤湿度小于阈值 */
    if(roil_RH_data_int < save_thresgold_val[4]){
        write.irr_on();
        Water_pump_equip_sta->setStyleSheet("QLabel{""image:url(:/pic/pumpon.png);""}");

    }else{
        write.irr_off();
        Water_pump_equip_sta->setStyleSheet("QLabel{""image:url(:/pic/pumpoff.png);""}");
    }
}
void MainWindow::read_client_data()
{
    char buf[1024] = {0};
    qint32 len = server_Socket->readDatagram(buf,sizeof(buf));
    /* 客户端开启 */
    if(QString(buf).section(" ",0,0) == "the"){
        QString threshold = QString("the ") + QString::number(save_thresgold_val[1]) + " " + QString::number(save_thresgold_val[0]) + " " +
                      QString::number(save_thresgold_val[2]) + " " + QString::number(save_thresgold_val[3]) + " " + QString::number(save_thresgold_val[4]) + " ";
        server_Socket->writeDatagram(threshold.toUtf8(),QHostAddress("192.168.10.100"),12139);
    }else if(QString(buf).section(" ",0,0) == "sta"){
        QString sta = QString("sta ") + QString::number(Irrigation_sta_Sock) + " " + QString::number(LED_sta_Sock) + " " +
             QString::number(Fan_sta_Sock) + " " + QString::number(Auto_sta_Sock) + " ";
        server_Socket->writeDatagram(sta.toUtf8(),QHostAddress("192.168.10.100"),12139);
    }else if(QString(buf).section(" ",0,0) == "autoon"){
        set = false;
        Auto_check->setCheckState(Qt::Checked);
    }else if(QString(buf).section(" ",0,0) == "autooff"){
        set = false;
        write.fan_off();
        write.irr_off();
        Auto_check->setCheckState(Qt::Unchecked);
    }else if(QString(buf).section(" ",0,0) == "ledon"){
        set = false;
        LED_check->setCheckState(Qt::Checked);
    }else if(QString(buf).section(" ",0,0) == "ledoff"){
        set = false;
        LED_check->setCheckState(Qt::Unchecked);
    }else if(QString(buf).section(" ",0,0) == "fanon"){
        set = false;
        Fan_check->setCheckState(Qt::Checked);
    }else if(QString(buf).section(" ",0,0) == "fanoff"){
        set = false;
        Fan_check->setCheckState(Qt::Unchecked);
    }else if(QString(buf).section(" ",0,0) == "irron"){
        set = false;
        Irrigation_check->setCheckState(Qt::Checked);
    }else if(QString(buf).section(" ",0,0) == "irroff"){
        set = false;
        Irrigation_check->setCheckState(Qt::Unchecked);
    }
    else{  /* 客户端阈值发生了改变 */
        save_thresgold_val[1] = QString(buf).section(" ",0,0).toInt();
        save_thresgold_val[0] = QString(buf).section(" ",1,1).toInt();
        save_thresgold_val[2] = QString(buf).section(" ",2,2).toInt();
        save_thresgold_val[3] = QString(buf).section(" ",3,3).toInt();
        save_thresgold_val[4] = QString(buf).section(" ",4,4).toInt();
        save_data();
        Temp_threshold_set->setSliderPosition(save_thresgold_val[1]);
        RH_threshold_set->setSliderPosition(save_thresgold_val[0]);
        ALS_threshold_set->setSliderPosition(save_thresgold_val[2]);
        CO2_threshold_set->setSliderPosition(save_thresgold_val[3]);
        Soil_RH_threshold_set->setSliderPosition(save_thresgold_val[4]);
    }

}




