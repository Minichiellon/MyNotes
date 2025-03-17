# Kadane算法详解

Kadane算法是一种用于高效求解**最大子数组和**问题的动态规划算法。以下是其核心原理与实现细节：

---

## **算法步骤**

1. **初始化**：
   - `current_max`：记录包含当前元素的最大子数组和，初始值为数组第一个元素。
   - `global_max`：记录全局最大子数组和，初始值同`current_max`。

2. **遍历数组**（从第二个元素开始）：
   - **更新`current_max`**：  
     `current_max = max(nums[i], current_max + nums[i])`
   - **更新`global_max`**：  
     `global_max = max(global_max, current_max)`

3. **结果**：遍历完成后，`global_max`即为最大子数组和。

---

## **关键特性**
- **动态规划思想**：每一步选择扩展当前子数组或以当前元素为起点重新开始。
- **处理全负数**：若数组全为负数，算法能正确找到最大的单个负数。
- **时间复杂度**：O(n)，仅需一次遍历。

---

## **示例分析**
数组：`[-2, 1, -3, 4, -1, 2, 1, -5, 4]`  
步骤分解：

| 元素 | `current_max` | `global_max` |
|------|---------------|--------------|
| -2   | -2            | -2           |
| 1    | 1             | 1            |
| -3   | -2            | 1            |
| 4    | 4             | 4            |
| -1   | 3             | 4            |
| 2    | 5             | 5            |
| 1    | 6             | **6**        |
| -5   | 1             | 6            |
| 4    | 5             | 6            |

**最终结果**：`global_max = 6`

---

## **代码实现（C语言）**

```c
#include <stdio.h>
#include <limits.h>

int max(int a, int b) {
    return (a > b) ? a : b;
}

int kadane(int* nums, int size) {
    if (size == 0) return 0;
    int current_max = nums[0];
    int global_max = nums[0];
    for (int i = 1; i < size; i++) {
        //从第二个元素开始遍历，比较"选取当前元素"和"从当前元素重新开始"哪种情况值更大
        current_max = max(nums[i], current_max + nums[i]);
        global_max = max(global_max, current_max);
    }
    return global_max;
}
```
## **应用场景**
- **最大子矩阵和**：将二维矩阵压缩为一维数组后应用Kadane算法。
- **股票买卖**：寻找一段时间内股票价格的最大涨幅。
- **信号处理**：定位信号序列中的最大连续波动。

---
Kadane算法以其简洁性和高效性，成为解决连续子数组问题的经典工具。通过维护两个变量动态更新状态，避免了暴力枚举的低效性。
