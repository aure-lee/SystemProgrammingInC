# 1.se

## seq-mr.c

`map_phase`和`reduce_phase`函数是MapReduce范式的核心部分。以下是它们通常如何工作的简要概述：

1. **map_phase**：此函数接收输入数据，处理数据，并输出键值对列表。对MapReduce作业提供的每个输入文件都应用map函数。

2. **reduce_phase**：此函数接收map阶段的输出，即键值对，并将具有相同键的值结合起来。reduce函数应用于与同一键相关联的所有值，并为每个键输出最终结果。

您提供的代码为顺序MapReduce作业设置了框架，包括从共享库加载map和reduce函数，散列（hashing），文件读取和MapReduce作业的主驱动程序。

要实现`map_phase`和`reduce_phase`函数，需要执行以下操作：

对于`map_phase`:
- 迭代每个文件。
- 读取文件内容。
- 将内容传递给`mr_map`函数以获取键值对列表。
- 散列每个键值对的键，并通过`nOutputs`确定输出文件。
- 将键值对写入相应的输出文件。

对于`reduce_phase`:
- 收集map阶段每个输出文件的所有键值对。
- 按键对键值对进行排序或分组。
- 将键和关联值的列表传递给`mr_reduce`函数。
- 将`mr_reduce`函数的结果写入最终输出文件。

以下是`map_phase`和`reduce_phase`函数的简单实现：

```c
void map_phase(fnode *node, int nInput, int nOutputs)
{
    for (int i = 0; i < nInput; i++)
    {
        char *file_content = read_file(node[i].file_name);
        list_t key_value_list = mr_map(file_content);
        free(file_content); // 读取文件后不要忘记释放内存

        // 迭代键值列表并写入输出文件
        list_iterator_start(&key_value_list);
        while (list_iterator_hasnext(&key_value_list))
        {
            kvp *pair = list_iterator_next(&key_value_list);
            unsigned int hash = djb_hash(pair->key) % nOutputs;
            // 写入对应输出文件的代码在这里
        }
        list_iterator_stop(&key_value_list);
    }
}

void reduce_phase(fnode *node, int nInput, int nOutputs)
{
    for (int i = 0; i < nOutputs; i++)
    {
        // 假设有一个函数可以获取给定输出文件的所有键值对
        list_t key_value_list = get_all_pairs_for_output_file(i);
        
        // 按键排序或分组
        // 在这里对键值列表进行排序或分组的代码

        // 迭代分组的键值对并应用reduce
        // 假设有一个函数可以获取相同键的下一组值
        while (has_next_key_group(key_value_list))
        {
            char *key;
            list_t values;
            get_next_key_group(&key_value_list, &key, &values);

            char *result = mr_reduce(key, &values);
            // 将结果写入最终输出文件的代码在这里

            free(result); // 不要忘记释放mr_reduce分配的内存
        }
    }
}
```

请注意，实际的实现细节，例如如何写入文件，如何对键值对进行分组，以及如何管理内存，将取决于您的代码库的其余部分以及`list`和`key-value`库提供的接口。上述代码是概念上的指导，并需要根据您的具体应用程序和库API进行调整。

要实现`map_phase`函数，我们需要根据提供的`mr_map`函数处理每个输入文件。具体来说，`map_phase`函数将对每个文件执行以下步骤：

1. 读取文件内容。
2. 使用`mr_map`函数将文件内容分割成单词，并将每个单词封装成键值对，其中键是单词，值是字符串"1"。
3. 将这些键值对的列表（`list_t`类型）写入中间文件，这些中间文件将用于后续的`reduce_phase`。通常，这涉及到基于键的哈希来决定写入哪个中间文件，以便后续归约阶段可以聚合相同键的值。

这里假设`fnode`结构体包含了输入文件信息，`inter_res`结构体用于存储中间结果，包括文件路径数组。以下是`map_phase`函数的一个可能的实现：

```c
void map_phase(fnode *node, int nInput, int nOutputs) {
    for (int i = 0; i < nInput; i++) {
        // 读取每个输入文件的内容
        char *file_content = read_file(node->files[i].fname);
        
        // 使用mr_map函数处理文件内容，获取键值对列表
        list_t kvs = mr_map(file_content);
        
        // 遍历键值对列表，将它们写入中间文件
        for (int j = 0; j < kvs->last; j++) {
            kv_t *kv = list_nth(kvs, j);
            
            // 计算键的哈希值，并决定输出到哪个中间文件
            unsigned int hash = djb_hash(kv->key) % nOutputs;
            char *intermediate_file_path = node->files[i].inter->file_paths[hash];
            
            // 写入中间文件的代码在这里
            // 示例: append_to_file(intermediate_file_path, kv);
        }

        // 清理：释放文件内容和键值对列表的内存
        free(file_content);
        list_free(kvs); // 假设存在一个释放list_t及其内容的函数
    }
}
```

请注意，上面的代码片段是一个简化的示例。实际实现可能需要考虑错误处理、文件写入的具体细节以及内存管理。特别是`append_to_file`函数（在示例中用作占位符）需要被实现以将键值对追加到正确的中间文件中。另外，`list_free`函数也需要实现，用于释放键值对列表及其内容所占用的内存。这些函数的具体实现将依赖于其他代码和数据结构的细节。

为了实现`reduce_phase`函数，我们需要对`map_phase`生成的中间结果进行归约操作。`reduce_phase`函数的任务是处理每个输出文件，这些文件包含了`map_phase`阶段生成的键值对。对于每个键，`reduce_phase`将调用`mr_reduce`函数来合并（或归约）所有对应的值。

以下是`reduce_phase`函数的一个可能实现，假设您已经有了以下几点：

- `fnode`结构体中有一个指向中间结果文件路径数组的指针。
- `mr_reduce`函数可以接收一个键和一个包含所有值的`list_t`，然后返回归约结果。

```c
void reduce_phase(fnode *node, int nInput, int nOutputs) {
    // 遍历每个输出分区
    for (int i = 0; i < nOutputs; i++) {
        // 创建一个字典（或哈希表）来收集相同键的所有值
        dict_t *dict = dict_new();

        // 遍历每个输入文件对应的中间结果
        for (int j = 0; j < nInput; j++) {
            // 获取当前中间结果文件的路径
            char *intermediate_file_path = node->files[j].inter->file_paths[i];

            // 读取中间文件并将键值对添加到字典中
            // 这里假设有一个函数可以处理这个任务
            process_intermediate_file(intermediate_file_path, dict);
        }

        // 现在所有的中间结果已经被收集到字典中，我们可以对每个键进行归约操作
        for (dict_iterator_t it = dict_begin(dict); it != dict_end(dict); it = dict_next(it)) {
            // 获取当前键和对应的值列表
            char *key = dict_key(it);
            list_t values = dict_value(it);

            // 对这些值进行归约操作
            char *reduced_value = mr_reduce(key, values);

            // 将结果写入最终的输出文件
            // 这里假设我们有一个函数来处理写入操作
            write_reduced_value_to_output(key, reduced_value, node->output_dir);

            // 清理归约后的值
            free(reduced_value);
        }

        // 清理字典资源
        dict_free(dict);
    }
}
```

在这个示例中，我们假设存在以下几个函数：

- `dict_new`：创建一个新的字典（或哈希表）。
- `process_intermediate_file`：读取一个中间文件，并将其内容（键值对）添加到字典中。
- `dict_begin`, `dict_end`, `dict_next`：字典迭代器，用于遍历字典中的所有条目。
- `dict_key`, `dict_value`：从字典迭代器中获取键和值列表。
- `write_reduced_value_to_output`：将归约后的值写入最终的输出文件。
- `dict_free`：释放字典资源。

请注意，上述代码是一个示例，它没有提供具体的字典操作函数，这些函数的具体实现取决于您选择的数据结构和库。您需要实现或使用现成的字典（哈希表）库来完成上述假设的函数。此外，您还需要实现文件读取和写入逻辑以及任何必要的错误处理和内存管理。

这两行代码是在C语言中定义函数指针的声明，它们用于MapReduce框架中的两个关键步骤：映射（Map）和归约（Reduce）。

1. `list_t (*mr_map)(char *);`
   - `mr_map` 是一个函数指针，它指向一个函数，这个函数接受一个`char *`类型的参数（通常指向一个字符串或字符数组），并返回一个`list_t`类型的值，其中`list_t`是一个可能在其他地方定义的结构体类型，通常用于表示一系列键值对。
   - 这个函数指针用于Map阶段，它负责处理输入数据（如一个文件的内容），并将其转换为一系列中间键值对。

2. `char *(*mr_reduce)(char *, list_t);`
   - `mr_reduce` 也是一个函数指针，它指向的函数接受两个参数：一个`char *`类型的键和一个`list_t`类型的值列表，并返回一个`char *`类型的值。
   - 这个函数指针用于Reduce阶段，它负责接收Map阶段输出的键和与该键相关的所有值的列表，然后合并这些值以形成一个最终的汇总结果。

在MapReduce程序中，`mr_map`和`mr_reduce`通常会在程序启动时从动态链接库（Dynamic Link Library，DLL）加载，这样可以使MapReduce框架不依赖于特定的映射和归约逻辑，而是可以动态地加载不同的逻辑来处理不同的数据和任务。这也是为什么你会看到`dlopen`和`dlsym`这样的动态加载库的函数调用，用于在运行时加载并获取这些函数指针的值。

## list.c

这些函数实现了一个动态数组（也常被称为“可变大小数组”或“向量”），它可以在运行时动态地增加其容量以存储更多元素。下面是各个函数的作用：

1. `list_new`:
   - 创建一个新的列表。
   - 为列表结构体和其内部的容器分配内存。初始容量为`INIT_SIZE`（8）。
   - 初始化列表的大小（`size`）和最后一个元素的索引（`last`）。
   - 返回新创建的列表。

2. `list_append`:
   - 将一个新元素追加到列表的末尾。
   - 如果列表已满（即`last`索引等于`size`），则重新分配内存以增加列表的容量。新容量是当前容量的`SCALE_FACTOR`倍（在这个案例中是原来的2倍）。
   - 将新元素添加到容器中，并递增`last`索引。
   - 断言用于确保重新分配后的内存不是`NULL`。

3. `list_nth`:
   - 返回列表中第`n`个元素的指针。
   - 这里没有检查索引`n`是否超出范围，调用者需要保证传入的索引是有效的。

4. `list_print`:
   - 遍历列表中的每个元素并对其应用提供的函数`f`。
   - `f`是一个函数指针，指向一个接受`void*`参数的函数，用于打印或以其他方式处理列表中的元素。

## counter-mr.c

这两个函数是MapReduce程序的具体实现示例，分别是映射（Map）和归约（Reduce）的逻辑。

1. `mr_map` 函数：
   - 该函数接收一个字符串`s`（可能是文件内容或其他文本数据）。
   - 它使用空格字符作为分隔符来分割字符串`s`，将其拆分为多个单词（token）。
   - 对于每个分割出的单词，它创建一个键值对（`kv_t`），键是单词本身，值是字符串"1"。
   - 所有这些键值对被追加到`list_t`类型的列表`kvs`中。
   - 最终，它返回含有所有键值对的列表。

这是一个典型的单词计数Map函数，它将文本分割成单词并映射每个单词到数值1，以便后续计算每个单词的出现次数。

2. `mr_reduce` 函数：
   - 该函数接收一个键（`key`）和一个列表（`values`），列表中包含了与该键相关联的所有值。
   - 它分配一个缓冲区`buf`，用于存储最终的归约结果。
   - 使用`sprintf`将与键相关的值的数量（即列表`values`中元素的数量，这里通过`last`属性获取）格式化为字符串，并写入`buf`中。
   - 函数返回这个缓冲区的地址，该地址包含了与特定键相关联的值的数量。

这是一个典型的单词计数Reduce函数，它聚合相同键（单词）的值（在这个案例中是"1"的出现次数），并计算这个键在输入数据中出现的总次数。

## file-struct.c

这个函数`init_fnode`是用于初始化一个`fnode`结构体的，它用于设置MapReduce操作中需要处理的文件节点。这个结构体表示了一系列输入文件以及它们的中间结果和输出。这个函数的主要任务是：

1. 分配内存给一个`fnode`结构体指针。
2. 为`fnode`结构中表示文件列表的`files`成员分配内存，大小基于输入文件的数量（`nInput`）。
3. 初始化`fnode`结构的成员变量，包括输入文件的数量（`num_files`）、临时目录路径（`tmp_dir`）和输出目录路径（`output_dir`）。
4. 遍历输入文件的列表：
   - 为每个输入文件分配一个`file_list`结构体，并复制文件名（使用`strdup`，这通常意味着在堆上分配一个新的字符串，并复制给定字符串的内容）。
   - 为每个文件的中间结果`inter`分配内存。
   - 为`inter`结构中的`file_paths`数组分配内存，这个数组将存储指向中间结果文件路径的指针，其大小基于输出文件的数量（`nOutputs`）。
   - 调用`create_files`函数（这个函数没有在提供的代码中定义）为每个输入文件创建中间结果文件路径。

这个函数的目的是准备好`fnode`结构，以便MapReduce框架可以使用它来处理输入文件、生成中间结果和最终输出。这个初始化过程是MapReduce作业之前准备阶段的一部分，确保了所有必要的数据结构和资源在实际映射（Map）和归约（Reduce）操作开始之前就已经就绪。
