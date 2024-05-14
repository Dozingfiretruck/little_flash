**English** | [中文](./README_zh.md) 

 # little_flash



   ## **Introduction**

   The goal is to make an SPI FLASH universal driver

   **Since the library is relatively new, it is inevitable that there may be problems, if you encounter it, please give positive feedback**

   The framework part is compatible with [SFUD](https://gitee.com/Armink/SFUD) projects for migration, and if you only need to drive NOR FLASH, it will be more stable to use SFUD repositories first

   need: c11

   ## Support  flash type

   - [x] SFDP
   - [x] NOR FLASH
   - [x] NAND FLASH

   ## Support  driver type

   - [x] SPI
   - [ ] QSPI


   ## PORT

   You only need to pay attention to the port two files `little_flash_port.c`，`little_flash_config.h`

   1， `little_flash_port.c`：Migrate file and connect them to your own driver interfaces

   2，`little_flash_config.h`： config file

   ## USAGE

   

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

   You can use the little_flash API to perform flash operations directly, but NAND FLASH is not recommended to use the file system

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
           .read_size = flash.chip_info.read_size,
           .prog_size = flash.chip_info.prog_size,
           .block_size = lf_flash.chip_info.erase_size,
           .block_count = lf_flash.chip_info.capacity / lf_flash.chip_info.erase_size,
           .cache_size = flash.chip_info.prog_size,
           .lookahead_size = flash.chip_info.prog_size,
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

   
