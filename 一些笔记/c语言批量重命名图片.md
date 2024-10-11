# 处理英文字符
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main() {
    struct dirent *entry;
    DIR *dp = opendir("./images"); // 打开包含图片的目录
    if (dp == NULL) {
        perror("opendir");
        return -1;
    }

    int count = 1;
    char oldName[256];
    char newName[256];

    while ((entry = readdir(dp)) != NULL) {
        // 只重命名 jpg 和 png 格式的文件
        if (strstr(entry->d_name, ".jpg") || strstr(entry->d_name, ".png")) {
            snprintf(oldName, sizeof(oldName), "./images/%s", entry->d_name); // 原文件路径
            snprintf(newName, sizeof(newName), "./images/image_%d.jpg", count++); // 新文件名

            if (rename(oldName, newName) != 0) {
                perror("rename");
            } else {
                printf("Renamed %s to %s\n", oldName, newName);
            }
        }
    }

    closedir(dp);
    return 0;
}

```
# 处理中文字符
```cpp
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);  // 设置控制台输出为 UTF-16 以显示中文

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(L"./images/*.*", &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        wprintf(L"Failed to open directory.\n");
        return -1;
    }

    int count = 1;
    wchar_t oldName[256];
    wchar_t newName[256];

    do {
        // 只重命名 jpg 和 png 格式的文件
        if (wcsstr(findFileData.cFileName, L".jpg") || wcsstr(findFileData.cFileName, L".png")) {
            // 生成旧的文件名路径（包含中文）
            swprintf(oldName, sizeof(oldName) / sizeof(wchar_t), L"./images/%s", findFileData.cFileName);
            // 生成新的文件名（中文文件名示例）
            swprintf(newName, sizeof(newName) / sizeof(wchar_t), L"./images/图片_%d.jpg", count++);

            // 使用 rename 函数重命名文件（宽字符版本 wrename）
            if (_wrename(oldName, newName) != 0) {
                wprintf(L"Failed to rename file: %s\n", findFileData.cFileName);
            } else {
                wprintf(L"Renamed %s to %s\n", oldName, newName);
            }
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);
    return 0;
}

```
