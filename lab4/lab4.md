# Lab4

## PartB

### 3.自旋锁

自旋锁是另一种保证独占执行的同步机制。与互斥锁不同，当一个线程尝试获取一个已被占用的自旋锁时，它会自旋等待锁的可用性。自旋锁在竞争不严重的情况下可能提供更高的效率，因为它避免了线程上下文切换的开销。但在高竞争的场景下，它也可能导致CPU资源的浪费。
在`pthread`库中，自旋锁的操作主要涉及以下关键函数。

```c
#include <pthread.h>  
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_unlock (pthread_spinlock_t *lock);
int pthread_spin_trylock (pthread_spinlock_t *lock);
int pthread_spin_destroy(pthread_spinlock_t *lock);
```

其中，`pthread_spin_init`函数用于初始化自旋锁，接受一个锁指针和一个标志，用于确定锁是否应该在进程之间共享。要获取自旋锁，线程使用`pthread_spin_lock`函数。如果锁已经被另一个线程持有，它将继续尝试获取锁，直到成功为止。

#### time命令

在Linux中，time命令用于测量程序的执行时间和资源使用情况。它的用法如下：

time [选项] 命令
常用的选项包括：

- -p：以可解析的格式输出时间和资源信息。
- -f FORMAT：指定自定义的输出格式。
- -o FILE：将输出写入指定的文件。

time命令的输出包括三部分：

实际时间（real）：指程序从开始到结束的实际运行时间，包括等待时间和执行时间。  
用户时间（user）：指程序在用户态运行的时间，即执行程序代码的时间。  
系统时间（sys）：指程序在内核态运行的时间，即执行系统调用和内核操作的时间。  
除了这些时间信息，time命令还会输出其他一些资源使用情况，例如CPU占用、内存使用量等。

下面是time命令的一个示例输出：

```text
real    0m1.234s
user    0m0.123s
sys     0m0.456s
```

在这个示例中，程序的实际运行时间为1.234秒，用户态运行时间为0.123秒，系统态运行时间为0.456秒。

通过使用time命令，你可以测量程序的执行时间，并进行性能分析和优化。

### 4.条件变量

条件变量是一种用于线程通信的同步机制，它避免了忙等待（busy-waiting）。

在pthread库中，条件变量的操作主要涉及以下关键函数。

```c
#include <pthread.h>  
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_destroy(pthread_cond_t *cond);
```

其中，`pthread_cond_init`函数基于提供的属性`attr`（或默认属性）初始化条件变量`cond`。当一个线程需要等待某个条件`cond`变为真时，它会调用`pthread_cond_wait`函数并在条件变量`cond`上休眠。

通过结合条件变量和互斥锁，我们可以更优雅地解决多生产者多消费者问题，其中多个生产者将数据项放入共享缓冲区，而多个消费者同时从该缓冲区中获取数据项。

## PartC

### 1.原子变量

原子变量是设计为以原子（不可分割）方式操作的变量。这意味着对这些变量的操作将在不受其他线程干扰的情况下完成。原子操作在多线程应用程序中至关重要，因为共享数据可以同时被多个线程访问。`<stdatomic.h>`头文件提供了一组函数，可用于对变量执行原子操作。

这些函数包括：

- `atomic_load`：返回原子变量的值。
- `atomic_store`：设置原子变量的值。
- `atomic_fetch_add`和`atomic_fetch_sub`：分别对原子变量执行加法或减法操作。
- `atomic_exchange`：将原子变量的值与新值交换。

### 2.使用原子变量构建同步原语

原子变量可用于构建互斥锁、信号量和自旋锁等同步原语。

### 3.实现原理

您可能想知道像`stdatomic.h`头文件中的高级原子变量是如何实现的。实际上，这些原子变量是通过在低级`CPU`上使用原子指令来实现的，因此它们依赖于特定的机器。在这部分，我们将探讨与`Intel x86-64 CPU`相关的原子变量实现，而这些关键原理也适用于其他 CPU，如`ARM`。

`Intel CPU`提供原子指令，通过在指令前加上一个特殊的关键字`lock`。

例如，指令

`lock incq (%rax)`

将会原子地将寄存器`%rax`指向的内存地址中的值增加 1。在底层，`CPU`将锁定内存总线以保证对内存的独占访问。特别地，有一个特殊的指令`CMPXCHG`，它可以原子地比较和交换内存值。本质上，CMPXCHG 指令是 Intel 对[著名的 CAS](https://en.wikipedia.org/wiki/Compare-and-swap)（比较并交换）指令的实现，这种指令在 x86 之外的多种 CPU 上也广泛存在。

## PartD 并发数据结构

并发数据结构支持多个并发线程同时访问和修改。在这一部分中，我们将构建两个并发数据结构作为示例，分别是并发栈和并发哈希表。

### 1.并发栈

与顺序栈类似，并发栈支持典型的栈操作，如`push()`、`pop()`和`size()`，但以并发的方式执行。

#### 练习 1

完成`concur-stack.c`中的代码。确保您的代码通过了`test-stack.c`中的测试。

### 2.并发哈希表

并发哈希表是一种支持在并发情况下高效搜索键的数据结构。

#### 练习 2

完成`concur-hash.c`中的代码，使用粗粒度互斥锁来保护整个哈希表。

#### 挑战

虽然粗粒度互斥锁可以有效保护哈希表，但它只允许一个线程一次访问哈希表。一个改进的思路是使用细粒度的锁策略，为每个桶（`bucket`）使用一个互斥锁。实现这个思路。

## PartE 无锁数据结构

无锁数据结构在不使用显式锁的情况下保护共享数据上的并发操作。

虽然各种锁（如互斥锁或自旋锁）可以保证共享数据结构的线程安全性，但它们也会对操作进行序列化，这通常是不希望的。无锁数据结构可以通过使用原子操作而不使用显式锁来进一步提高整体吞吐量。在这部分中，我们将构建两个无锁数据结构：栈和队列。

### 1.无锁栈

构建无锁栈的关键思想是以原子方式比较和交换栈顶。

#### 练习1

在`lf-stack.c`中填写缺失的代码。

### 2.无锁队列

为了构建无锁队列，我们使用两个原子变量分别记录队列的尾部和头部。然后我们使用原子操作来入队或出队项目。

#### 练习2

在`lf-queue.c`中填写缺失的代码。确保你的代码通过测试。
