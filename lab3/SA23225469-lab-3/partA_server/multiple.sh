#!/bin/bash

# 简单的脚本来对服务器执行多个并发请求。

# 服务器的URL或IP地址
SERVER_URL="http://localhost:8080/index.html"

# 要发起的并发请求的数量
NUM_REQUESTS=1000

time1=$(date +%s)
echo $(date)

# 每个进程将执行的函数
# 它仅仅是curl服务器URL，并将响应输出到一个文件
function make_request {
    curl -s "$SERVER_URL" > /dev/null
    # echo "请求完成。"
}

# 导出函数，以便可以由parallel使用
export -f make_request

# 启动多个进程，每个进程都会执行make_request函数
for i in $(seq 1 $NUM_REQUESTS)
do
    make_request &
done

# 等待所有后台进程完成
wait

time2=$(date +%s)
echo $(date)

diff=$((time2 - time1))
echo "$diff"

echo "所有请求已完成。"
