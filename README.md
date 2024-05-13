# little_flash

## 介绍
目标做成spi flash通用框架

要求: c11

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

## 使用

## LFS

```
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

