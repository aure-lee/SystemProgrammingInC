# Lab5 MapReduce

## 概述

分布式系统是指其组件位于不同的网络计算机上，通过传递消息进行通信和协调行动的系统。一个良好设计的分布式系统不仅提供效率和可扩展性，还提供容错性和经济性。因此，分布式系统在构建今天的云计算和大数据系统中变得越来越重要。

在这个实验室中，你将使用C语言实现分布式计算中的一些重要基础设施，从而更深入地理解分布式计算系统设计和实现的原理。首先，你将学习什么是远程过程调用（RPC），并使用它们构建一些有趣的服务。其次，你将实现MapReduce，这是最有影响力的分布式计算基础设施之一。第三，你将实现一个分片键值存储，类似的系统支持许多流行的网站。

你应该下载这个代码来开始这个实验室。

## PartA:RPC

远程过程调用（Remote Procedure Call，简称RPC）是一种通信协议，允许程序在网络上调用另一台计算机上的程序或服务。它使我们能够在一台机器上执行一个函数，而该函数实际上在另一台机器上运行并返回结果。这种机制特别适用于分布式系统，允许不同系统和语言之间进行交互。

### 1.Software Setup

gorpc是一个使用Go语言编写的RPC框架，提供了一个简单的API，使得将RPC功能集成到Go项目中变得容易。`gorpc`利用高效的序列化和反序列化方法，以及优化的网络通信协议，提供高性能的RPC服务。此外，`gorpc`支持多种传输协议和序列化格式。

要使用`GoPRPC`，首先需要在您的环境中安装`Go`语言：

```text
css-lab@tiger:~$ sudo apt-get install golang
css-lab@tiger:~$go version
go version go1.18.1 linux/amd64
```

如果您看到上述信息，则安装成功。

### 2.Services ans RPCs

在这部分中，我们将使用RPC创建一个求平方的服务器。客户端将发送一个数字，服务器将对该数字进行平方运算，然后将结果返回给客户端。

#### 练习1

阅读`partA/square`文件夹中的代码，并填写`square-client.c`和`square-server.c`中缺失的代码。

接下来，我们将实现一个功能来统计字符串中的单词数。字符串客户端将发送`"hello, world"`到字符串服务，字符串服务器将统计单词的数量并将结果返回。

#### 练习2

阅读分配给你的代码`string-service.c`，并完成`string-count`函数的实现。

同样地，我们也可以实现基于RPC的云数据结构。例如，你可以在服务器端设计一个栈以及一些栈的操作，并尝试在客户端上调用它。

#### 练习3

完成`stack-server.h`和`stack-server.c`中的代码。确保你的代码在`stack-client.c`中通过测试。

### 3.Concurrency ans Locks

在分布式场景中，竞态条件是指多个分布式组件同时尝试访问和修改共享资源的情况，导致结果不可预测。为了避免这种情况，可以使用锁来确保只有一个组件可以在任何给定时间访问或修改资源。通过这种方式，锁提供了一种机制，确保对共享资源的操作是原子的，从而消除潜在的竞态条件。

当多个线程同时尝试修改共享计数器时，如果没有适当的同步机制，可能会导致数据不一致。具体来说，两个或多个线程可能同时读取计数器的相同值，然后根据该值执行增加或减少操作。这样，它们的修改可能互相覆盖，导致最终的计数值不是预期的总和。这种情况通常被称为竞态条件。

#### 练习3.1

编译并运行`partA/count`文件夹中的代码。您的输出是什么？预期的输出是什么？这个输出是如何生成的？

#### 练习3.2

修改`partA/count`文件夹中的代码，使用互斥锁保护共享变量。编译并运行您的代码，输出是什么？

当有多个客户端或多个线程访问服务器时，RPC实现中的堆栈也会出现数据竞争，因此我们需要使用一些方法来确保数据的安全性。

#### 练习3.3

改进`stack-server.h`和`stack-server.c`中的代码。确保您的代码通过`constack-client.c`中的测试。

## PartB: MapReduce

在这部分中，您需要构建一个分布式计算的`MapReduce`库，就像Google的`MapReduce`论文所描述的那样。为了使任务可管理，您将逐步构建三个版本的`MapReduce`：

1. 顺序`MapReduce`：只有一个线程执行所有的Map任务，然后执行Reduce任务；
2. 并发`MapReduce`：N个工作线程执行Map任务，M个工作线程执行Reduce任务，还有一个主节点协调所有任务，它们都在同一台服务器上运行；
3. 分布式`MapReduce`：工作线程可能在不同的服务器上运行。

### 1.Sequential MapReduce

请查看`file-struct.h`文件以获取函数声明的详细信息。并结合下面的两个图表，理解单线程`MapReduce`的数据结构。

#### 练习1

在`seq-mr.c`文件中完成`map_phase`和`reduce_phase`函数，实现单线程`MapReduce`的功能。

提示：您可能需要一个用于`MapReduce`的数据集。您可以通过运行以下命令在`Utils`目录中生成原始数据：
`css-lab@tiger:~$ python3 generate-words.py 10 1M`

### 2.Concurrent MapReduce

顺序`MapReduce`使用单个线程按顺序执行`Map`和`Reduce`函数。在这部分中，您将使用多线程模型实现`Map`和`Reduce`，并需要设计适当的数据结构来解决并发处理中的数据同步挑战。

#### 练习2

设计您的数据结构并实现线程通信和同步，以在多线程单机环境下完成`MapReduce`。

### 3.Distributed MapReduce

在实际环境中，负责`MapReduce`处理的线程通常分布在多台服务器上，也被称为工作节点。工作节点向主节点发起连接请求以请求任务，主节点将`Map`或`Reduce`任务分配给工作节点，从而完成分布式`MapReduce`。

#### 练习3

使用`gorpc`远程过程调用实现分布式`MapReduce`。您需要设计适当的数据结构来减轻分布式环境中常见的冲突和同步问题。

## PartC: Sharded KV Stores

在这部分中，您将构建一个分布式分片键值（KV）存储系统。键值存储系统是一个从键到相应值的概念映射。然而，如今的热门网站（例如微博）或应用程序（例如微信）可能拥有超过数十亿的用户，因此无法通过单个服务器存储所有用户数据或为所有请求提供服务。为此，一个自然的想法是将所有用户数据分割并存储在一组服务器上，这通常被称为分片。存储数据的服务器被称为分片KV存储。

### 1.分片KV存储

为了利用KV分片存储，客户端应该维护一个配置C，告诉客户端应该与哪个分片KV服务器进行通信，以访问与指定键相关联的值。更具体地说，假设我们有n个具有地址`addr_i(1<=i<=n)`的服务器，那么我们可以在每个客户端上维护以下配置C：

`C: [(addr_1, [l_1, h_1]), (addr_2, [l_2, h_2]), ..., (addr_n, [l_n, h_n])]`

记录分片服务器的地址以及哈希值范围。因此，对于任何键"k"，如果其哈希值`hash(k)`在范围`[l_i, h_i]`内，则我们知道其数据驻留在分片服务器`addr_i`上。

为了更好地理解这个想法，让我们来看一个示例。假设我们有四个分片服务器，地址从`addr_0`到`addr_3`，并且有以下配置：

```text
C: [(addr_0, [0, 249]),
    (addr_1, [250, 499]),
    (addr_2, [500, 749]),
    (addr_3, [750, 999])]
```

因此，对于一个示例用户名`"ZhangSan"`，如果我们有
`hash("ZhangSan") % 1000 == 347`

那么我们应该与地址为addr_1的服务器通信，以访问其值。

#### 练习1

实现这个配置。

一个分片的键值存储（KV存储）S 将一个键 "k" 映射到与该键关联的值的列表 v_1、...、v_n：

S：k -> [v_1, v_2, ..., v_n]
键 k 和值 v_i（其中 1<=i<=n）都是字符串。例如，当 k 是像微信这样的社交媒体上的用户名时，值 v_1 到 v_n 都是她在"朋友圈"发布的所有帖子。

一个分片的键值存储支持以下API服务：

```text
Get(k)      : 检索与键 "k" 关联的值
Put(k, v)   : 将 "k" 关联到 "v"（将清除 "k" 的现有值）
Append(k, v): 将值 "v" 追加到 "k" 的值列表；如果 k 不存在，则充当 Put(k, v)
Delete(k)   : 移除键 "k" 及其关联的所有值
```

#### 练习2

实现分片KV存储。

### 2.Sharding Master

虽然您已经构建了一个在多个服务器上运行的分片键值存储（KV存储），但它存在一个关键限制，即该存储是静态的，因此不可扩展。具体而言，无法添加或删除服务器。在这个实验的这一部分，您将实现一个分片主服务器，该主服务器将管理所有分片服务器，支持添加或删除服务器。与此同时，每个分片服务器上的键值对应该在服务器之间进行迁移，以反映服务器配置的更改。例如，当移除服务器 S 时，服务器 S 上的所有键值对应该在 S 被移除之前迁移到其他服务器，因此尽管移除了服务器，数据仍然可用。

在这个实验的这一部分，您应该集中精力在分片主服务器的实现上，但不需要担心分片服务器之间的数据迁移。您将在实验的下一部分中实现数据迁移。

分片主服务器维护了上述的服务器配置 C：
    C: [(addr_1, [l_1, h_1]), (addr_2, [l_2, h_2]), ..., (addr_n, [l_n, h_n])]

与此同时，分片主服务器运行一个支持以下API的服务：
    GetConfig()       : 检索当前配置
    AddServer(addr)   : 添加一个具有地址 "addr" 的新服务器
    RemoveServer(addr): 移除具有地址 "addr" 的服务器

值得注意的是，这些API会更改配置 C。例如，给定一个配置：
    C: [(addr_0, [0, 249]),
        (addr_1, [250, 499]),
        (addr_2, [500, 749]),
        (addr_3, [750, 999])]

对 RPC 调用 AddServer(addr_4) 将导致以下配置：
    C: [(addr_0, [0, 199]),
        (addr_1, [200, 399]),
        (addr_2, [400, 599]),
        (addr_3, [600, 799]),
        (addr_4, [800, 999])]

接下来，对 RemoveServer(addr_2) 的调用将导致以下配置：
    C: [(addr_0, [0, 249]),
        (addr_1, [250, 499]),
        (addr_3, [500, 749]),
        (addr_4, [750, 999])]

#### 练习3

实现主配置的代码以及主API。

配置 C 本质上是一个全局属性，不仅分片主服务器，还每个分片服务器 S 都应该维护一份副本。与此同时，配置 C 在不断变化，因为分片服务器的添加或移除。因此，每个分片服务器 S 应该定期调用分片主服务器的`GetConfig()`RPC 来使其本地配置副本保持最新。

#### 练习4

实现配置的更新。

### 3.KV Shuffling

在这个实验的这一部分，您将实现在分片服务器之间进行键-值（KV）重新分配，以移动KV对。具体而言，当将新服务器 S 添加到配置中时，其他服务器上的KV对可能会根据新配置移动到这个新服务器 S。另外，当现有服务器 S 从配置中移除时，应在服务器 S 离开之前将服务器 S 上的KV对移动到其他分片服务器。

#### 练习5

实现KV重新分配。
