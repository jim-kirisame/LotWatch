# LotWatch

LotWatch是一个自制的蓝牙手表项目。它使用了nRF51822作为核心，mma8452为加速度计。

使用[nRF SDK V10.0](https://developer.nordicsemi.com/nRF5_SDK/nRF51_SDK_v10.x.x/nRF51_SDK_10.0.0_dc26b5e.zip)开发，并在Keil MDK v5下编译通过。

## 文件结构

- app-js：手机端应用程序，使用Cordova编写
- Application：手表的主程序部分
- Bootloader：手表的Bootloader，用于OTA
- Hardware：硬件设计
- Release：编译好的各个程序，可以直接使用。

其余事项见各文件夹下的说明。