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
# 批量重命名图片，生成markdown文档，移动文件
```cpp
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
void Createfile(wchar_t* FileName, wchar_t* PicNameArr);
void RemoveFile(wchar_t* SourceDirectory, wchar_t* Distination);

wchar_t* mdFileNmame = L"1.电路模型.md";
wchar_t DistinationDirectory[256];
wchar_t SourceDirectory[256];
int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);  // 设置控制台输出为 UTF-16 以显示中文

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(L"./images/*.*", &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        //wprintf(L"Failed to open directory.\n");
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
            swprintf(newName, sizeof(newName) / sizeof(wchar_t), L"./images/1-2电路模型-图片-%d.jpg", count++);
            // 使用 rename 函数重命名文件（宽字符版本 wrename）
            if (_wrename(oldName, newName) != 0) {
                wprintf(L"Failed to rename file: %s\n", findFileData.cFileName);
            } else {
                wprintf(L"Renamed %s to %s\n", oldName, newName);
            }
            Createfile(mdFileNmame,newName);
            swprintf(SourceDirectory, sizeof(SourceDirectory), L"D:\\\\rename\\\\images\\\\%s",newName+9);
            swprintf(DistinationDirectory, sizeof(DistinationDirectory), L"D:\\\\rename\\\\%s",newName+9);

                //wprintf(L"%ls",DistinationDirectory);

            RemoveFile(SourceDirectory,DistinationDirectory);
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);
    return 0;
}

void Createfile(wchar_t* FileName, wchar_t* PicNameArr){
    // 设置控制台输出为 UTF-16 以支持中文
    _setmode(_fileno(stdout), _O_U16TEXT);

    // 中文文件名
    wchar_t* filename = mdFileNmame;

    // 使用 _wfopen 打开文件 
    FILE *file = _wfopen(filename, L"a, ccs=UTF-8"); // 以 UTF-8 编码写入
    if (file == NULL) {
        //perror("打开文件失败");
        exit(1);
    }

    // 向文件写入内容
        fwprintf(file, L"<div><img src = \"%ls\"></div>\n",PicNameArr);
    // 关闭文件
    fclose(file);
}

void RemoveFile(wchar_t* SourceDirectory, wchar_t* Distination){
    //wprintf(L"%ls\n",SourceDirectory);
    //wprintf(L"%ls\n",Distination);
    // 移动文件
    if (_wrename(SourceDirectory, Distination) == 0) {
        printf("文件移动成功！\n");
    } else {
        perror("文件移动失败");
    }
}


```
