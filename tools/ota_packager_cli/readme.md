RBL打包命令行工具

## 命令示例
rt_ota_packaging_tool_cli.exe -f rtthread.bin -v 20200408_165304 -p app -o rtthread.rbl -c gzip -s aes -i 0123456789ABCDEF -k 0123456789ABCDEF0123456789ABCDEF

## 参数解析

OTA file packager v0.1.0
Shanghai Real-Thread Electronic Technology Co.,Ltd

Usage:
rt_ota_packaging_tool_cli -f BIN -v VERSION -p PARTNAME [-o OUTFILE] [-c CMPRS_TYPE] [-s CRYPT_TYPE] [-i IV] [-k KEY] [-h]
-f bin file. 待打包的固件文件，BIN格式。
-v firmware's version. 设定版本号，字符串，建议16字符左右
-p firmware's target part name. RBL目标分区名称
-o output rbl file path.(optional) RBL文件输出路径
-c compress type allow [quicklz|gzip|none](optional) 压缩算法
-s crypt type allow [aes|none](optional) 加密算法
-i iv for aes-256-cbc 密码1
-k key for aes-256-cbc 密码2
-h show this help information 打印帮助
