# 操作系统实验报告
```
张一白
161320224
```

## 多进程

### shell1

每次读入一行，以空格隔开解析所有参数，随后执行`fork()`，父线程中运行`waitpid()`等待子线程退出。子线程中运行`execvp()`，将所要执行的程序装入子线程运行。
![](http://7sbxnl.com1.z0.glb.clouddn.com/shell1.png)

### shell2

每次读入一行，解析重定向文件名。使用`dup()`复制保存相应(stdin/stdout)的文件描述符，并将对应文件打开，将文件描述符替换到对应(stdin/stdout)的描述符上。随后以shell1运行程序的方法执行对应程序。
![](http://7sbxnl.com1.z0.glb.clouddn.com/shell2.png)

### shell3
每次读入一行，以`|`解析所有程序命令。对于每条命令，执行`pipe()`函数，将写描述符与上一条命令产生的的读描述符传入重定向解析函数。重定向解析函数中解析重定向指令，并打开相应文件，如存在文件重定向，则使用文件描述符；否则使用管道描述符。将描述如传入`my_system()`函数, `fork()`后在子进程`exec()`之前使用`dup2()`替换对应(stdin/stdout)描述符；父进程中只对最后一条fork启动的子线程等待结束。
![](http://7sbxnl.com1.z0.glb.clouddn.com/shell3.png)

## 多线程

### pi1

计算线程函数传入参数结构体中包含起始项序号，需计算项数，计算结果，进程ID。新建线程从1开始计算count/2项，随后主线程从count/2+1开始计算count/2项。随后等待线程结束。结束后将各进程计算的结果相加
![](http://7sbxnl.com1.z0.glb.clouddn.com/pi1.png)

### pi2

将pi1中的计算线程继续分割，以子线程方式运行。主线程等待所有子线程结束后将结果累加输出。
![](http://7sbxnl.com1.z0.glb.clouddn.com/pi2.png)

### sort
排序线程函数传入参数结构体包含起始项序号，项数。排序线程内使用了系统自带的快速排序函数进行排序。待所有分割的子线程结束排序后，通过归并的方法将排序结果归并到新的数组中并输出。
![](http://7sbxnl.com1.z0.glb.clouddn.com/pi2.png)

### pc1
使用条件变量和is_full的布尔变量dispatch相应数组为满或者为空的事件。

producer在buf1为空的时候生产，随后把buf1_is_full设为true，并激活buf1_change事件，然后等待buf1_change，并且!buf1_is_full。

calculator收到buf1_change后判断是否为buf1_is_full，并进行计算。计算后将buf2_is_full设为true并激活buf2_change事件，然后等待buf2_change，并且!buf2_is_full。在接收到后将buf1_is_full设为false并发送buf1_change事件。

consumer收到buf2_change后判断buf2_is_full，并将buf2输出。随后将buf2_is_full设为false并发送buf2_change事件。

![](http://7sbxnl.com1.z0.glb.clouddn.com/pc1.png)

### pc2
使用信号量buf1_full, buf1_empty, buf2_full, buf2_empty，初始值分别为0, 4, 0, 4。

随后按标准生产者-消费者模型运行即可。此时calculator是produce的消费者，consumer是calculator的消费者。

![](http://7sbxnl.com1.z0.glb.clouddn.com/pc2.png)

### ring
使用信号量cond, 互斥锁mutex, 互斥锁add

使用竞争的方法在进程开启后尝试将自己加入cond的wait列表。首先对add锁加锁，加锁后如果当前需要加入cond的进程编号是自己，则执行cond_wait加入等待列表。随后解锁add。
wait被激活后对一个全局变量n进行加一，随后执行wait将自己再次加入等待列表。

在所有进程加入完成后，使用signal激活cond，便可以环形执行。

![](http://7sbxnl.com1.z0.glb.clouddn.com/ring.png)

