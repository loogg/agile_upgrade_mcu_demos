# Agile Upgrade

## 1、介绍

Agile Upgrade: 用于快速构建 bootloader 的中间件。

- `example` 文件夹提供 PC 上的示例

- MCU 上的示例查看 [mcu_demos](https://github.com/loogg/agile_upgrade_mcu_demos)

### 1.1、特性

1. 适配 `RT-Thread` 官方固件打包工具 (图形化工具及命令行工具)
2. 使用纯 C 开发，不涉及任何硬件接口，可在任何形式的硬件上直接使用
3. 加密、压缩支持如下：
   - [x] AES256
   - [x] fastlz
   - [x] quicklz
   - [ ] gzip
4. 原生适配 `file` 及 `fal` 操作接口
5. 移植简单，实现自定义的后端只需适配几个操作接口
6. 使用简单，几行代码即可实现固件升级
7. 全过程日志输出
8. 提供过程回调，可将过程及进度显示在自定义硬件上

### 1.2、目录结构

| 名称 | 说明 |
| ---- | ---- |
| adapter | 已适配的后端 |
| doc | 文档 |
| example | 例子 |
| figures | 素材 |
| inc  | 头文件 |
| porting | 移植文件 |
| src  | 源代码 |
| tools  | 固件打包工具 |

### 1.3、许可证

Agile Upgrade 遵循 `Apache-2.0` 许可，详见 `LICENSE` 文件。

## 2、移植

### 2.1、配置文件

Agile Upgrade 依赖 `agile_upgrade_config.h` 头文件，修改该头文件中的配置可以设置库的基本行为，禁用未使用的模块和特性，在编译时调整内存缓冲区的大小等等。

`porting` 文件夹下提供了配置文件模板 `agile_upgrade_conf_template.h`，复制它并重命名为 `agile_upgrade_config.h` 并在工程中包含路径即可。

#### 基本配置

1. 设置 `CRC` 表定义位置，开启则定义在 `Flash`，否则定义在 `RAM`

    ```C
    #define AGILE_UPGRADE_USING_CRC_TAB_CONST
    ```

2. 使能版本号比较,开启则比较固件版本，固件版本一致不进行升级

    ```C
    #define AGILE_UPGRADE_ENABLE_VERSION_COMPARE
    ```

#### 调试功能

1. 使能日志输出

    ```C
    #define AGILE_UPGRADE_ENABLE_LOG
    ```

2. 日志输出调用的接口

    ```C
    #define AGILE_UPGRADE_LOG_PRINTF printf
    ```

#### 加密算法

1. 使能 AES256

    ```C
    #define AGILE_UPGRADE_ENABLE_AES
    ```

2. AES256 初始化向量，16 个字符

    ```C
    #define AGILE_UPGRADE_AES_IV  "0123456789ABCDEF"
    ```

3. AES256 密钥，32 个字符

    ```C
    #define AGILE_UPGRADE_AES_KEY "0123456789ABCDEF0123456789ABCDEF"
    ```

#### 压缩算法

1. 使能 fastlz

    ```C
    #define AGILE_UPGRADE_ENABLE_FASTLZ
    ```

2. 使能 quicklz

    ```C
    #define AGILE_UPGRADE_ENABLE_QUICKLZ
    ```

#### 使能后端

1. 使能 fal 后端

    ```C
    #define AGILE_UPGRADE_ENABLE_FAL
    ```

2. 使能 file 后端

    ```C
    #define AGILE_UPGRADE_ENABLE_FILE
    ```

### 2.2、后端适配

原生适配 `file` 及 `fal` 操作接口，自定义后端只需要实现 `agile_upgrade_ops` 接口即可。可以参考 [adapter](./adapter) 中后端的实现。

```C
struct agile_upgrade_ops {
    int (*init)(agile_upgrade_t *agu);
    int (*read)(agile_upgrade_t *agu, int offset, uint8_t *buf, int size);
    int (*write)(agile_upgrade_t *agu, int offset, const uint8_t *buf, int size);
    int (*erase)(agile_upgrade_t *agu, int offset, int size);
    int (*deinit)(agile_upgrade_t *agu);
};
```

1. `init`

    初始化后端，可以使用应用层传递的 `agu->user_data` 参数，并将生成的句柄传递给 `agu->ops_data`。不需要可不实现。

2. `read`

    从后端读取数据，`offset` 为负数时表示从尾部读入。可以使用 `agu->ops_data` 参数。

3. `write`

    向后端写入数据，`offset` 为负数时表示向尾部写入。可以使用 `agu->ops_data` 参数。

4. `erase`

    擦除后端数据。可以使用 `agu->ops_data` 参数。不需要可不实现。

5. `deinit`

    反初始化后端。可以使用 `agu->ops_data` 参数。不需要可不实现。

### 2.3、RT-Thread 软件包支持

在 `RT-Thread` 的包管理器中选中即可，不需要自行移植配置文件。

```C

RT-Thread online packages
    system packages   --->
        [*] agile_upgrade: Middleware for fast building bootloader.  --->
            [*]   Enable debug log output
            [ ]   Enable crc32 table defined in flash
            [ ]   Enable AES decrypt
            [ ]   Enable fastlz decompress
            [*]   Enable quicklz decompress
            [ ]   Enable fal adapter
            [ ]   Enable file adapter
            [*]   Enable version compare

```

## 3、使用 Agile Upgrade

### 3.1、快速开始

移植操作完成后，几行代码即可实现固件升级。

```C
#include <agile_upgrade.h>

extern const struct agile_upgrade_ops agile_upgrade_file_ops;

int main(void) {
    agile_upgrade_t src_agu = {0};
    src_agu.name = "src";
    src_agu.user_data = "./src.rbl";
    src_agu.ops = &agile_upgrade_file_ops;

    agile_upgrade_t dst_agu = {0};
    dst_agu.name = "dst";
    dst_agu.user_data = "./dst.bin";
    dst_agu.ops = &agile_upgrade_file_ops;

    agile_upgrade_release(&src_agu, &dst_agu, 0);

    return 0;
}
```

- `agile_upgrade`

  ```C
  struct agile_upgrade {
    const char *name;
    int len;
    const void *user_data;
    const struct agile_upgrade_ops *ops;
    const void *ops_data;
  };
  ```

  - `name`

    后端名称，日志打印中会使用。如果为 `NULL`，日志中会输出为 `UNKNOWN`。

  - `len`

    后端能操作空间大小(Byte)，校验固件和搬运时进行判断。如果为 0 则不进行判断。

  - `user_data`

    后端私有数据，一般用于 `agu->ops->init` 中作为初始化的参数。

  - `ops`

    后端操作接口。

  - `ops_data`

    后端操作接口私有数据。

### 3.2、API 介绍

1. 固件校验

    ```C
    int agile_upgrade_verify(agile_upgrade_t *agu, agile_upgrade_fw_info_t *fw_info, uint8_t is_tail);
    ```

    | 参数 | 描述 |
    | ---- | ---- |
    | agu | 操作对象 |
    | fw_info | 固件信息 |
    | is_tail | 0：固件信息在头部<br/>1：固件信息在尾部|

2. 固件升级

    ```C
    int agile_upgrade_release(agile_upgrade_t *src_agu, agile_upgrade_t *dst_agu, uint8_t is_erase);
    ```

    | 参数 | 描述 |
    | ---- | ---- |
    | src_agu | 源操作对象 |
    | dst_agu | 目标操作对象 |
    | is_erase | 升级完成后是否需要擦除源操作对象 |

3. 设置钩子函数

    固件解析按步骤执行，每一步执行前会调用步骤钩子函数；异常时会调用异常钩子函数；每解析一包数据会调用进度钩子函数。

    - 步骤枚举

      ```C
      typedef enum {
      AGILE_UPGRADE_STEP_INIT = 0,
      AGILE_UPGRADE_STEP_VERIFY,
      AGILE_UPGRADE_STEP_ALGO,
      AGILE_UPGRADE_STEP_UPGRADE,
      AGILE_UPGRADE_STEP_FINISH
      } agile_upgrade_step_t;
      ```

    - 错误码枚举

      ```C
      typedef enum {
      AGILE_UPGRADE_EOK = 0,
      AGILE_UPGRADE_ERR = -1,
      AGILE_UPGRADE_EVERIFY = -2,
      AGILE_UPGRADE_EFULL = -3,
      AGILE_UPGRADE_EREAD = -4,
      AGILE_UPGRADE_EWRITE = -5,
      AGILE_UPGRADE_EERASE = -6,
      AGILE_UPGRADE_EINVAL = -7
      } agile_upgrade_err_t;
      ```

    - 设置步骤钩子函数

      ```C
      void agile_upgrade_set_step_hook(agile_upgrade_step_callback_t hook);
      typedef void (*agile_upgrade_step_callback_t)(int step);
      ```

    - 设置进度钩子函数

      ```C
      void agile_upgrade_set_progress_hook(agile_upgrade_progress_callback_t hook);
      typedef void (*agile_upgrade_progress_callback_t)(uint32_t cur_size, uint32_t total_size);
      ```

    - 设置错误钩子函数

      ```C
      void agile_upgrade_set_error_hook(agile_upgrade_error_callback_t hook);
      typedef void (*agile_upgrade_error_callback_t)(int step, int code);
      ```

## 4、支持

![zanshang](./figures/zanshang.jpg)

如果 Agile Upgrade 解决了你的问题，不妨扫描上面二维码请我 **喝杯咖啡** ~

## 5、联系方式 & 感谢

- 维护：马龙伟
- 主页：<https://github.com/loogg/agile_upgrade>
- 邮箱：<2544047213@qq.com>
