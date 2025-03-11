# C语言输入输出函数详解

C语言中的输入输出（I/O）函数主要定义在标准库 `<stdio.h>` 中，涵盖从标准输入输出到文件操作等多种功能。以下是各类函数的详细介绍及示例：

---

## 一、标准输出函数

### 1. `printf`
- **功能**：格式化输出到标准输出（`stdout`）。
- **语法**：`int printf(const char *format, ...);`
- **占位符**：`%d`（整型）、`%f`（浮点）、`%s`（字符串）、`%c`（字符）等。
- **示例**：
  ```c
  int age = 25;
  printf("Age: %d\n", age); // 输出：Age: 25
  ```

### 2. `puts`
- **功能**：输出字符串并自动换行。
- **语法**：`int puts(const char *str);`
- **示例**：
  ```c
  puts("Hello World"); // 输出：Hello World\n
  ```

### 3. `putchar`
- **功能**：输出单个字符，不换行。
- **语法**：`int putchar(int char);`
- **示例**：
  ```c
  putchar('A'); // 输出：A
  ```

---

## 二、标准输入函数

### 1. `scanf`
- **功能**：从标准输入（`stdin`）读取格式化输入。
- **语法**：`int scanf(const char *format, ...);`
- **注意**：需用 `&` 取变量地址，字符串输入需指定长度（如 `%10s`）。
- **示例**：
  ```c
  int num;
  scanf("%d", &num); // 输入数字存入num
  ```

### 2. `fgets`（推荐替代 `gets`）
- **功能**：安全读取字符串（含换行符），指定最大长度。
- **语法**：`char *fgets(char *str, int n, FILE *stream);`
- **示例**：
  ```c
  char buffer[100];
  fgets(buffer, sizeof(buffer), stdin); // 从stdin读取一行
  ```

### 3. `getchar`
- **功能**：从 `stdin` 读取单个字符。
- **语法**：`int getchar(void);`
- **示例**：
  ```c
  char c = getchar(); // 读取一个字符
  ```

---

## 三、文件操作函数

### 1. `fopen` / `fclose`
- **功能**：打开/关闭文件。
- **模式**：`"r"`（读）、`"w"`（写）、`"a"`（追加）。
- **示例**：
  ```c
  FILE *file = fopen("test.txt", "w");
  if (file != NULL) {
      fputs("Hello File", file);
      fclose(file);
  }
  ```

### 2. `fprintf` / `fscanf`
- **功能**：格式化文件写入/读取。
- **示例**：
  ```c
  fprintf(file, "Value: %d", 42); // 写入文件
  fscanf(file, "%d", &num);       // 从文件读取
  ```

### 3. `fgets` / `fputs`
- **功能**：逐行读写文件字符串。
- **示例**：
  ```c
  fputs("Line of text", file);    // 写入字符串（无自动换行）
  fgets(buffer, 100, file);       // 读取一行（含换行符）
  ```

---

## 四、字符串格式化函数

### 1. `sprintf` / `snprintf`
- **功能**：将数据格式化为字符串。
- **注意**：`snprintf` 可限制长度，防止溢出。
- **示例**：
  ```c
  char buffer[50];
  sprintf(buffer, "Value: %d", 100); // 输出到字符串
  ```

### 2. `sscanf`
- **功能**：从字符串解析数据。
- **示例**：
  ```c
  int val;
  sscanf("Age: 30", "Age: %d", &val); // val = 30
  ```

---

## 五、二进制文件操作

### `fread` / `fwrite`
- **功能**：读写二进制数据（如结构体）。
- **示例**：
  ```c
  struct Data data;
  fwrite(&data, sizeof(data), 1, file); // 写入结构体
  fread(&data, sizeof(data), 1, file);  // 读取结构体
  ```

---

## 六、错误处理

### `perror`
- **功能**：输出错误信息（基于 `errno`）。
- **示例**：
  ```c
  FILE *file = fopen("nonexistent.txt", "r");
  if (file == NULL) {
      perror("Error"); // 输出：Error: No such file or directory
  }
  ```

---

## 注意事项
1. **安全性**：避免使用 `gets`（已废弃），优先用 `fgets` 替代。
2. **缓冲区问题**：`scanf` 可能遗留换行符，建议用 `fgets` 读取后解析。
3. **返回值检查**：始终检查函数返回值（如 `scanf` 返回成功读取的项数）。
```