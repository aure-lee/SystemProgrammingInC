# Lab4-PartB

## Exercise 1

```text
Expected count: 0 (because half increment and half decrement)
Actual count: 13873
```

在给定的代码中，存在竞争条件，increment和decrement两个线程可能会同时访问共享数据`count`，由于线程之间的竞争关系，可能会导致某些操作被覆盖或丢失，导致结果不对。

## Exercise 2

输出：

```text
Expected count: 0 (because half increment and half decrement)
Actual count: 0
```

## Exericse 3

```text
Expected count: 0 (because half increment and half decrement)
Actual count: 0
```

## Exercise 4

```text
simonli@DESKTOP-UAMU9F2:~/lec/lab4/partB$ time ./mutex.out 
Expected count: 0 (because half increment and half decrement)
Actual count: 0

real    0m0.011s
user    0m0.026s
sys     0m0.024s
simonli@DESKTOP-UAMU9F2:~/lec/lab4/partB$ time ./spinlock.out 
Expected count: 0 (because half increment and half decrement)
Actual count: 0

real    0m0.009s
user    0m0.017s
sys     0m0.000s
```

分析`time`命令输出的内容，`spinlock`的效率更高一些。

在保护共享数据结构时，我们可以根据以下因素来决定使用互斥锁还是自旋锁：

1. 内容竞争的程度：如果预计存在较高的竞争程度，即多个线程频繁争夺锁的情况，那么互斥锁通常更适合。互斥锁能够将等待的线程置于休眠状态，从而减少CPU的利用率，避免资源的浪费。

2. 等待时间的长短：如果预计等待时间很短，并且锁很快就会被释放，那么自旋锁可能更高效。自旋锁避免了线程上下文切换的开销，等待的线程会持续自旋检查锁的可用性，如果锁很快被释放，等待的线程可以快速获取锁而无需进行上下文切换的开销。

3. 系统负载：自旋锁可能会导致CPU资源的浪费，因为等待的线程会持续自旋检查锁的可用性。因此，在系统负载较高的情况下，使用互斥锁可能更合适，以避免浪费CPU资源。
