CppHttpServer

基于 Linux 平台使用 C++ 从零实现的轻量级 HTTP WebServer，支持 HTTP GET/POST 请求解析、静态资源访问以及 CGI 动态请求处理。

服务器基于 Socket 编程实现 TCP 通信，采用多线程方式处理客户端连接，完成了基础 HTTP 协议解析与表单提交功能，适合作为 Linux 网络编程与 WebServer 入门实践项目。

项目特性
基于 Linux Socket 实现 TCP 服务端
支持 HTTP/1.0 GET / POST 请求
支持静态网页资源访问
支持 CGI 动态脚本执行
支持 HTML 表单提交
使用 std::thread 实现多线程并发处理
支持 Bash CGI 脚本处理 POST 数据
支持基础错误处理（404 Not Found）
使用 Makefile 管理项目编译

项目结构
CppHttpServer/
├── httpdocs/            
│   ├── post.html         
│   └── post.cgi          
├── image/
│   └── result.png        
├── httpd.cpp           
├── Makefile            
├── .gitignore
└── README.md

技术栈
C++
Linux
Socket 网络编程
HTTP/1.0
CGI
多线程（std::thread）
Bash Shell
Makefile
核心功能
HTTP 请求解析

服务器支持：
GET 请求
POST 请求
Header 读取
Content-Length 解析
Body 数据读取
CGI 动态处理

通过 fork + pipe + execl 实现 CGI 动态脚本执行：

创建子进程执行 CGI
使用管道完成父子进程通信
通过环境变量传递请求信息
支持 POST 数据输入
多线程并发

服务器采用：

thread(handle_client, client).detach();

实现客户端连接并发处理。

编译运行
环境要求
Linux / Ubuntu
g++
make
编译项目
make

生成可执行文件：

myhttp
启动服务器
./myhttp

运行后：

HTTP Server running on port 6379
浏览器访问

打开浏览器访问：

http://127.0.0.1:6379
CGI 使用说明

项目默认首页：

/post.html

用户提交表单后：

浏览器发送 POST 请求
服务端解析 HTTP Body
CGI 脚本读取 POST 数据
动态生成 HTML 页面返回结果
项目截图
表单提交页面

HTTP 请求处理流程
客户端请求
    ↓
Socket 接收连接
    ↓
解析 HTTP 请求
    ↓
判断静态资源 / CGI
    ↓
执行 CGI 或返回文件
    ↓
响应客户端
已实现内容
 HTTP GET 请求
 HTTP POST 请求
 静态资源访问
 CGI 动态处理
 表单提交
 多线程连接处理
 Linux Socket 通信
 基础错误处理
后续优化方向
MIME 类型支持
HTTP 状态码完善
Keep-Alive 长连接
epoll IO 多路复用
线程池
Reactor 模型
日志系统
配置文件支持
文件下载功能
非阻塞 IO
学习内容

通过该项目学习并实践了：

Linux 网络编程
Socket API 使用
HTTP 协议基础
CGI 工作原理
多线程并发编程
进程间通信（pipe）
Linux 系统调用
Git/GitHub 项目管理
License

This project is for learning and personal practice only.
