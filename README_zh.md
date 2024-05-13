[**English**](./README.md)  | **中文**

# little_flash



## 介绍
目标做成SPI FLASH通用框架

**由于库较新，难免可能会有问题，如果遇到请积极反馈**

框架部分兼容[SFUD](https://gitee.com/Armink/SFUD)项目以便迁移，如果只需要驱动NOR FLASH优先使用SFUD更稳定

要求: c11

## 支持flash类型

- [x] SFDP
- [x] NOR FLASH
- [x] NAND FLASH

## 支持驱动方式

- [x] SPI
- [ ] QSPI


## 移植教程

只需要关注port两个文件 `little_flash_port.c`，`little_flash_config.h`

1， `little_flash_port.c`：移植文件，对接自己的驱动接口

2，`little_flash_config.h`： 配置文件

## 使用



```c
little_flash_t lf_flash = {0};

uint8_t date[256] = {0};

little_flash_init();
little_flash_device_init(&lf_flash);
printf("little_flash_device_init OK\n");

little_flash_erase(&lf_flash, 1024x1024, 256);

little_flash_write(&lf_flash, 1024x1024, date, 256);

little_flash_read(&lf_flash, 1024x1024, date, 256);


```

可使用little_flash api直接进行flash操作，但不推荐NAND FLASH直接操作，推荐使用文件系统

## LittleFs

```c
    // configuration of the filesystem is provided by this struct
    struct lfs_config cfg = {

        .context = &lf_flash,
        // block device operations
        .read = lf_block_device_read,
        .prog = lf_block_device_prog,
        .erase = lf_block_device_erase,
        .sync = lf_block_device_sync,
        // block device configuration
        .read_size = 2048,
        .prog_size = 2048,
        .block_size = lf_flash.chip_info.erase_size,
        .block_count = lf_flash.chip_info.capacity / lf_flash.chip_info.erase_size,
        .cache_size = 2048,
        .lookahead_size = 2048,
        .block_cycles = 500,
    };

    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    LUAT_DEBUG_PRINT("boot_count: %d", boot_count);
```

