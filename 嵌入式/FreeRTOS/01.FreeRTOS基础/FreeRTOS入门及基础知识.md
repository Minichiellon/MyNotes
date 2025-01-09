# 裸机与FreeRTOS介绍
　　对于裸机系统，只能按顺序执行每个任务。
```cpp
while(1) 
{ 
    打游戏();
    回复信息(); 
}

```
　　对于RTOS系统，可以创建两个任务，然后 $\color{red}{以很短的时间间隔交替执行}$ 两个任务，使得 $\color{red}{看起来两个任务在同时执行}$ 。
```cpp
void main(void) 
{ 
    /* 创建打游戏任务 */
    xTaskCreate（打游戏）
    /* 创建回复信息任务 */
    xTaskCreate(回复信息)
}

```
