# little_flash

## 介绍
目标做成spi flash通用框架



## 支持flash类型

- [ ] nor flash
- [x] nand flash

## 支持驱动方式

- [x] SPI
- [ ] QSPI


## 移植教程

只需要关注port两个文件 `little_flash_port.c`，`little_flash_config.h`

1， `little_flash_port.c`：移植文件，对接自己的驱动接口

2，`little_flash_config.h`： 配置文件

