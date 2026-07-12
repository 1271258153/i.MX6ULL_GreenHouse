# 问题记录

## 1. U-Boot 到内核启动，中间改过哪些参数？

本项目主要改的是 U-Boot 环境变量 **`bootargs`** 和 **`bootcmd`**（见 [README §6](./README.md#6-u-boot移植)）。

### 网络启动内核 + eMMC 根文件系统

```shell
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw'
setenv bootcmd 'tftp 80800000 zImage; tftp 83000000 imx6ull-alientek-emmc.dtb; bootz 80800000 - 83000000'
saveenv
```

| 参数 | 含义 |
|------|------|
| `console=ttymxc0,115200` | 串口控制台 |
| `root=/dev/mmcblk1p2` | 根文件系统分区 |
| `rootwait` | 等 mmc 就绪再挂载 |
| `rw` | 根文件系统可读写 |
| `tftp 80800000 zImage` | 内核下载到 DRAM `0x80800000` |
| `tftp 83000000 *.dtb` | 设备树下载到 `0x83000000` |
| `bootz …` | 跳转启动 zImage |

### NFS 挂载根文件系统

```shell
setenv bootargs 'console=ttymxc0,115200 root=/dev/nfs nfsroot=192.168.1.250:/home/alientek/linux/nfs/rootfs,proto=tcp rw ip=192.168.1.251:192.168.1.250:192.168.1.1:255.255.255.0::eth0:off'
saveenv
```

---

## 2. 驱动加载失败怎么排查？

结合本仓库驱动（`linux-source/drivers/.../Green_House/`）的常用步骤：

1. **看内核日志**  
   `dmesg | tail -50`  
   本项目驱动里大量用了 `printk`（如 `register chrdev false`、`init SGP30 false`、`gpio request false`），优先看 probe / init 是否打印失败。

2. **核对设备树与驱动匹配字符串**  
   - 设备树：[`imx6ull-14x14-evk.dts`](linux-source/arch/arm/boot/dts/imx6ull-14x14-evk.dts)  
   - 驱动：`of_match_table` 里的 `compatible` 必须与 DT **完全一致**  
   - 例：AHT20 驱动为 `"alientek,aht20"`，DT 同为 `compatible = "alientek,aht20"`，`reg = <0x38>`  

3. **`of_device_id` 必须有哨兵**  
   表尾要有 `{}`，否则 OF 匹配可能越界（见第 4 节）。

4. **确认总线与地址**  
   ```bash
   i2cdetect -y 0    # 或 1，看 0x38 / 0x58 / 0x1e 是否在线
   ls /sys/bus/i2c/devices/
   ls /dev/aht20 /dev/sgp30 /dev/ap3216c /dev/fan /dev/irr
   ```

5. **风扇 / 灌溉（platform 路径写法）**  
   [`fan.c`](linux-source/drivers/platform/Green_House/fan/fan.c) / [`irr.c`](linux-source/drivers/platform/Green_House/irr/irr.c) 用 `of_find_node_by_path("/imx_fan")` 等，需确认 DT 里有对应节点和 `gpio_*` 属性，并注意 GPIO 是否被占用。

6. **硬件冲突**  
   DT 注释写明：AHT20 与 SGP30 调试时引脚可能冲突，需二选一或用杜邦线/面包板中转。

7. **编译与内核版本**  
   外编模块的 `KERNELDIR`、`ARCH`、`CROSS_COMPILE` 必须对应当前运行的内核，否则 `insmod` 可能版本不匹配。

---

## 3. 触摸芯片 GT9147 与 Tslib 设备节点

触摸芯片为 **GT9147**，原先的 FT5426 驱动无法使用。

排查过程：

1. 修改驱动 `Makefile` 的 `KERNELDIR`，指向当前内核源码，编译出 `.ko`
2. 将 `.ko` 拷贝到 `lib/modules/4.1.15/`，手动加载后运行 Qt **无法交互**
3. 用 `hexdump /dev/input/event2` 测试，确认触摸驱动本身有效
4. 检查 `/etc/profile`，发现 `TSLIB_TSDEVICE=/dev/input/event1`，而手动加载生成的是 **event2**
5. 改成正确的 event 节点后，Qt 即可正常交互

**编进内核后无法启动的补充：**  
把驱动放进 `drivers/input/touchscreen/` 并 `obj-y += gt9147.o` 时，需在内核顶层 Makefile 设好 `ARCH` / `CROSS_COMPILE`；能进内核但仍起不来时，检查设备树中 **OV5640 摄像头与触摸引脚是否冲突**（本板 DT 中有 `goodix,gt9147` 与 `ovti,ov5640` 节点）。

---

## 4. `of_device_id` 缺少哨兵节点

设备树匹配表 `of_device_id` 末尾没有加哨兵节点 `{ /* sentinel */ }`（即空的 `{}`）。

加载模块时报错；用 `printk` 调试时现象更怪。原因是内核匹配 Device Tree 时**越界访问**——原本越界读到的“随机地址”可能刚好是 `NULL`，内核靠 `.compatible == NULL` 结束遍历；加了 `printk` 后栈/内存布局变化，越界读到的值不再是 `NULL`，表现为崩溃或异常。

**正确写法（以 AHT20 / SGP30 为例）：**

```c
struct of_device_id aht20_of_match[] = {
    { .compatible = "alientek,aht20" },
    { /* sentinel */ }
};
```

```c
const struct of_device_id sgp30_match_table[] = {
    { .compatible = "sgp30" },
    { /* sentinel */ }
};
```

---

## 5. 驱动框架相关问题（结合本仓库源码）

### 5.1 platform 驱动和 i2c 驱动有什么区别？`probe` 何时调用？

| 类型 | 本项目例子 | 匹配与 probe |
|------|------------|--------------|
| **I2C 驱动** | [`aht20.c`](linux-source/drivers/media/i2c/Green_House/aht20/aht20.c)、[`sgp30.c`](linux-source/drivers/media/i2c/Green_House/sgp30/sgp30.c)、[`ap3216c`](linux-source/drivers/media/i2c/Green_House/ap3216c) | `i2c_add_driver()` 后，总线把 DT 生成的 `i2c_client` 与 `i2c_driver` 的 `of_match_table` / `id_table` 匹配成功 → 调用 `probe(client)` |
| **SPI 驱动** | [`icm20608.c`](linux-source/drivers/media/spi/icm20608/icm20608.c) | `spi_register_driver()` 匹配成功 → `probe(spi)` |
| **GPIO 类“平台设备”** | [`fan.c`](linux-source/drivers/platform/Green_House/fan/fan.c)、[`irr.c`](linux-source/drivers/platform/Green_House/irr/irr.c) | **未注册 `platform_driver`**，在 `module_init` 里直接 `of_find_node_by_path("/imx_fan")` 取 GPIO，**没有标准 probe** |

I2C：设备已在总线上（DT 的 `reg` + 父节点 `&i2c1`），驱动去“认领”设备。  
标准 platform：DT `compatible` + `platform_driver_register` → probe。本项目风扇/灌溉是简化写法。

### 5.2 设备树里一个 i2c 节点如何和驱动匹配？

以 SGP30 为例：

**设备树**（`imx6ull-14x14-evk.dts`）：

```dts
&i2c1 {
    sgp30@58 {
        compatible = "sgp30";
        reg = <0x58>;   /* 7 位 I2C 地址 */
    };
};
```

**驱动**：

```c
const struct of_device_id sgp30_match_table[] = {
    { .compatible = "sgp30" },
    {}
};
/* .driver.of_match_table = sgp30_match_table */
```

流程：内核解析 DT → 在 i2c1 上创建 client（地址 `0x58`）→ 与驱动 `compatible` 字符串比较 → 成功则 `sgp30_probe()`，其中 `client->addr` 即设备地址（**不要用**头文件里的 8 位地址 `0xb0/0xb1`）。

---

## 6. AHT20、SGP30 等 I2C 传感器上电时序与数据读写问题

### 6.1 AHT20（[`aht20.c`](linux-source/drivers/media/i2c/Green_House/aht20/aht20.c)）

**时序：**

| 阶段 | 操作 | 延时 |
|------|------|------|
| 上电稳定 | — | `mdelay(40)` |
| 初始化 | 写 `{INIT, 0x08, 0x00}` | 再 `mdelay(500)`，并读状态 |
| 测量 | 写 `{0xAC, 0x33, 0x00}` | `mdelay(80)` 后再读 |

**数据拼接（20 bit，不是整字节对齐）：**

- 湿度：高字节在 `raw[1]`、`raw[2]`，并与 `raw[3]` 的高 4 位拼成 20 bit  
- 温度：`raw[3]` 低 4 位 + `raw[4]` + `raw[5]` 拼成 20 bit  

**浮点：** 内核里不要做 `float` 换算，驱动只把原始计数值 `copy_to_user`，在 Qt/`readfile` 应用层再算真实温湿度。

### 6.2 SGP30（[`sgp30.c`](linux-source/drivers/media/i2c/Green_House/sgp30/sgp30.c)）

**时序：**

1. 发 `Init_air_quality`（`0x2003` → 字节 `0x20, 0x03`），`mdelay(100)`  
2. 测量时发 `Measure_air_quality`（`0x2008` → `0x20, 0x08`），`mdelay(100)` 后读回 eCO2  

命令拆字节写法：

```c
buf[0] = Init_air_quality >> 8;   /* 高字节 0x20 */
buf[1] = Init_air_quality & 0xff; /* 低字节 0x03 */
```

**地址：** DT `reg = <0x58>` → `client->addr`；头文件 `0xb0/0xb1` 是 8 位读写地址，Linux I2C 不用。

**注意：** 手册测量结果一般为 6 字节（含 CRC）；驱动若只读部分字节，以实际代码为准，建议按手册读满并校验 CRC。

### 6.3 本项目 I2C/SPI 设备一览

| 设备 | 总线 | DT `compatible` / `reg` | 字符设备 |
|------|------|-------------------------|----------|
| AHT20 | I2C1 | `alientek,aht20` / `0x38` | `/dev/aht20` |
| SGP30 | I2C1 | `sgp30` / `0x58` | `/dev/sgp30` |
| AP3216C | I2C1 | `lsc_ap3216c` / `0x1e` | `/dev/ap3216c` |
| ICM20608 | SPI | `alientek,icm20608` | `/dev/icm20608` |
| 风扇 | GPIO | 节点 `/imx_fan` | `/dev/fan` |
| 灌溉 | GPIO | 节点 `/imx_irr` | `/dev/irr` |
