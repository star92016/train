# 跑火车
----
## 简介
这是一个服务端和多个客户端的程序，多个客户端能够同步配合显示一个火车从左边的窗口跑到右边的窗口。最初的想法来自之前看过的一个视频，一个小人从一个电脑屏幕跑到另一个电脑屏幕。

+ 运行前
![run-before](https://raw.githubusercontent.com/star92016/train/master/run-before.png)
+ 运行中
![runing](https://raw.githubusercontent.com/star92016/train/master/runing.png)

## 编译
进入工作目录执行make
## 运行服务端
+ ./server [-p port][-t time]
+ 可以给程序加监听端口号和延迟时间参数
+ 端口默认是5555
+ 时间默认是30，时间取1-1000，越小运动越快
## 运行客户端
+ ./client [-p port][-h host][-f file][end]
+ 可以加访问端口、服务端地址和文件
+ 端口默认为5555
+ 地址默认值为127.0.0.1，地址目前只支持ip地址
+ 文件默认为当前目录show.txt
+ [end]表示这是最后一个客户端，最后一个客户端执行./client end程序就开始运行，最大客户端数量是100
## show.txt格式
+ 第一行：行数 列数
+ 其余行：图像
+ 文件内容可以保护特殊控制字符，通过控制字符可以给图像加颜色
+ 文件默认内容来自sl程序
## 存在问题
+ 服务端如果提前异常结束程序，TCP连接会进入TIME_WAIT，导致当前端口暂时不可用
+ 部分同学可能会编译出错，这个程序首先安装ncurses库。
