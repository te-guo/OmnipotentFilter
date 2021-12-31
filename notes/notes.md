## todo-list

- 实现静态LDCF
- fix load factor，绘制bpk-fpr图像



## log

### 2021/12/23

#### result:

local:

<img src="2021-12-23-local.png" alt="2021-12-23-local" style="zoom:8%;" /> 

server:

<img src="2021-12-23-server.png" alt="2021-12-23-server" style="zoom:8%;" /> 

#### Issues:





### 2021/12/9

#### result:

local:

<img src="2021-12-9-local-size_25_with_remove.png" alt="2021-12-9-local-size_25_with_remove" style="zoom:6%;" /> <img src="2021-12-9-local-size_25_without_remove.png" alt="2021-12-9-local-size_25_without_remove" style="zoom: 6%;" />

server:

<img src="2021-12-9-server-size_25_with_remove.png" alt="2021-12-9-server-size_25_with_remove" style="zoom:6%;" /><img src="2021-12-9-server-size_25_without_remove.png" alt="2021-12-9-server-size_25_without_remove" style="zoom:6%;" />

#### Issues:

- throughput 在 server 上振动
- vqf 如果 remove 就挂
- 需要 fix load factor，绘制 bpk-fpr 图像
- 需要实现多次测试取average performance
- 需要实现静态LDCF
