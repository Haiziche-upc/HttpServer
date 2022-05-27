## HttpServer
This is a miniserver implemented in C/C++

## Environment
**Platform**：Ubuntu 20.04.4 LTS

**Language**: C/C++

**Compiler**: g++ 9.4.0


## Instructions for use
1. This server supports GET, POST method, GET method for resource acquisition, such as HTML, image and other files to obtain, POST method can be used together with the backend, if you want to submit data, please use the POST method, instead of GET.

2. The default port number is **9999**, if there is a conflict, please modify the SERVER_PORT in HttpServer.H.

3. The default IP is **127.0.0.1**, if you need to modify it, please modify it in the Server() method in HttpServer .cpp.

4. If you want to process the data using the POST method and return it, please carefully read the comments in the POSTRequest() method.

5. Add -pthread at compile time


## HttpServer
用c/c++实现的微型http服务软件

## 运行环境
**平台**：Ubuntu 20.04.4 LTS

**语言**: C/C++

**编译器**: g++ 9.4.0


## 使用说明
1.本服务器支持GET，POST方法，GET方法用于资源获取，如HTML，image等文件的获取，POST方法可结合后端一起使用，如果你想进行数据提交，请用POST方法，而不是GET。

2.默认端口号为**9999**,如有冲突，请在HttpServer.H中修改SERVER_PORT。

3.默认IP为**127.0.0.1**,如需修改，请在HttpServer.cpp中的Server() 方法中进行修改。

4.如果你想使用POST方法对数据进行处理并返回，请仔细阅读POSTRequest()方法中的注释。

5.编译时请添加 -pthread

File structure:

```
|--main.cpp
|--HttpServer.cpp
|--404.html
|--404
|  |--style.css
|  |--failed.ico
|--Demo1.html
|--HaiZiChe.png
```



