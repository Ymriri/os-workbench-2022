//
// Created by Ymymym on 2024/3/25.
//

#include <stdio.h>
#include <assert.h>
#include <dirent.h> // file
#include <string.h>
#include <stdlib.h>

/***
 * 打开/proc文件，找到所有pid的status
 * PPid 就是他们的父进程，能够渲染成一个树
 *
 */
typedef struct Procfile
{
    char file_name[100]; // name
    int pid;             // pid
    int root[100];       // root
    int count;
    int ppid;
} Procfile;

int compare(const void *a, const void *b)
{
    return (((Procfile *)a)->pid - ((Procfile *)b)->pid);
}

DIR *openDir();                               // openDir
int initTree();                               // 读取所有的进程保存到map
int string2int(char *temp_string, int begin); // string2int
void Print();                                 // print
int findRootTree();                           // 构建系统树
void dfs_create_tree(int root);               // dfs 查找节点
int findPid(int pid);                         // findPid
void PrintTree(Procfile *root, int deep);     // printTree
int PrintPid(int pid, char *name);            // printPid
// fileRoot
Procfile fileTree;
// 进程map
Procfile fileMap[1000];
// 进程map记录
int fileMap_count = 0;
// 前导输出
int empty_string_count = 0;

int emptyString[100];

int need_sort = 0;

int show_pid = 0;
int main(int argc, char *argv[])
{
    // skip fileName
    for (int i = 1; i < argc; i++)
    {
        assert(argv[i]); // C 标准保证
        if (strcmp("-V", argv[i]) == 0 || strcmp("--version", argv[i]) == 0)
        {
            printf("Ymri-pstree 0.0.1\n");
            printf("pCopyright (C)2024 Ymri \n");
            return 0;
        }
        if (strcmp("-n", argv[i]) == 0 || strcmp(" --numeric-sort", argv[i]) == 0)
        {
            need_sort = 1;
        }
        if (strcmp("-p", argv[i]) == 0 || strcmp("--show-pids", argv[i]) == 0)
        {
            // show pids
            show_pid = 1;
        }

        // printf("argv[%d] = %s\n", i, argv[i]);
    }
    assert(!argv[argc]); // C 标准保证
    if (initTree())
    {
        return -1;
    }
    if (need_sort)
    {
        qsort(fileMap, fileMap_count, sizeof(Procfile), compare);
    }
    // Print();
    int root = findRootTree();
    empty_string_count = 0;
    printf("-%s ", fileMap[root].file_name);
    // 记录root的长度
    emptyString[empty_string_count++] = strlen(fileMap[root].file_name) + 1;

    PrintTree(&(fileMap[root]), 1);
    return 0;
}

DIR *readDir()
{

    DIR *dir;
    dir = opendir("/proc");
    if (dir == NULL)
    {
        printf("!Error: Cant't open dir.\n");
        return NULL;
    }
    return dir;
}
/**
 * 字符串变成int，从begin开始到最后
 */
int string2int(char *temp_string, int begin)
{
    // 未检查越界...
    int ret = 0;
    for (int i = begin; temp_string[i] != '\0'; i++)
    {
        if (temp_string[i] == '\n')
        {
            return ret / 10;
        }
        ret += (int)temp_string[i] - 48;
        ret *= 10;
    }
    return -1;
}

void Print()
{
    printf("Pid PPid Name\n");
    for (int i = 0; i < fileMap_count; i++)
    {
        printf("%d %d %s\n", fileMap[i].pid, fileMap[i].ppid, fileMap[i].file_name);
    }
    printf("\n");
}

void PrintTree(Procfile *root, int deep)
{
    if (root == NULL)
        return;
    for (int i = 0; i < root->count; i++)
    {
        if (i == 0)
        {
            // printf
            printf("- ");
            // printf("%s ", fileMap[root->root[i]].file_name);

            int temp_count = PrintPid(fileMap[root->root[i]].pid, fileMap[root->root[i]].file_name);
            // has brother?
            if (root->count == 1)
            {
                // no brother
                emptyString[empty_string_count++] = -temp_count;
            }
            else
            {
                emptyString[empty_string_count++] = temp_count;
            }
            PrintTree(&(fileMap[root->root[i]]), deep + 1);
            empty_string_count--;
        }
        else
        {

            for (int j = 0; j < empty_string_count; j++)
            {

                int temp_count = emptyString[j] > 0 ? emptyString[j] : -emptyString[j];
                for (int k = 0; k < temp_count; k++)
                {
                    printf(" ");
                }
                if (j < (empty_string_count - 1) && (emptyString[j + 1] > 0))
                {
                    printf("|");
                }
                else if (j != empty_string_count - 1)
                {
                    printf(" ");
                }
            }
            printf("|");
            printf("- ");
            int temp_count = PrintPid(fileMap[root->root[i]].pid, fileMap[root->root[i]].file_name);
            // int temp_count = strlen(fileMap[root->root[i]].file_name) + 2;
            if (root->count == 1 || i == root->count - 1)
            {
                emptyString[empty_string_count++] = -temp_count;
            }
            else
            {
                emptyString[empty_string_count++] = temp_count;
            }
            PrintTree(&(fileMap[root->root[i]]), deep + 1);
            empty_string_count--;
        }
    }
    if (root->count == 0)
    {
        // find the end
        printf("\n");
    }
}

// 读取所有的进程，保存到map
int initTree()
{
    DIR *dir = readDir();
    if (dir == NULL)
    {
        return -1;
    }
    fileTree.count = 0;
    struct dirent *temp_dirent = NULL;
    char file_name[256] = "/proc/";
    int count = 5;
    char file_context[110];
    int file_name_count = 0;
    while ((temp_dirent = readdir(dir)) != NULL)
    {
        if (temp_dirent != NULL)
        {
            // printf("%s\n", temp_dirent->d_name);
            //
            if (temp_dirent->d_name[0] >= 48 && temp_dirent->d_name[0] <= 58)
            {
                count = 6;
                while ( (temp_dirent->d_name[count - 6]) != '\0')
                {
                    file_name[count] = temp_dirent->d_name[count - 6];
                    count++;
                }
                file_name[count++] = '/';
                file_name[count++] = 's';
                file_name[count++] = 't';
                file_name[count++] = 'a';
                file_name[count++] = 't';
                file_name[count++] = 'u';
                file_name[count++] = 's';
                file_name[count] = '\0';
                // 构造成/proc/pid/status文件路径
                FILE *fp = fopen(file_name, "r");
                if (fp)
                {
                    Procfile tempPid;
                    tempPid.count = 0;

                    for (int i = 0; i < 7; i++)
                    {
                        fgets(file_context, 100, fp);
                        switch (i)
                        {
                            case 0:
                                // copy name
                                file_name_count = 0;
                                for (int j = 0; file_context[j] != '\0'; j++)
                                {
                                    if (j < 6)
                                    {
                                        continue;
                                    }
                                    tempPid.file_name[file_name_count++] = file_context[j];
                                }
                                tempPid.file_name[file_name_count - 1] = '\0';
                                break;
                            case 5:
                                tempPid.pid = string2int(file_context, 5);
                                break;
                            case 6:
                                tempPid.ppid = string2int(file_context, 6);
                            default:

                                break;
                        }
                    }
                    fileMap[fileMap_count++] = tempPid;
                    fclose(fp);
                }
                else
                {
                    printf("读取异常！\n");
                }
            }
        }
    }
    closedir(dir);

    return 0;
}

int findRootTree()
{
    assert(fileMap_count != 0);
    // 系统从1开始
    int root = findPid(1);
    assert(root != -1);
    fileTree.count = 1;
    fileTree.root[0] = root;
    // printf("%d %s\n", root->pid, root->file_name);
    dfs_create_tree(root);
    return root;
}

int PrintPid(int pid, char *name)
{
    if (show_pid)
    {
        printf("%s(%d) ", name, pid);
        int count = strlen(name) + 4;
        while (pid > 0)
        {
            count++;
            pid /= 10;
        }
        return count;
    }
    else
        printf("%s ", name);
    return strlen(name) + 2;
}
void dfs_create_tree(int root)
{
    if (root < 0)
        return;
    Procfile *temp_procfile = &fileMap[root];
    temp_procfile->count = 0;
    // max set 100
    for (int i = 0; i < fileMap_count; i++)
    {
        // find a child
        // 自己不能是自己的父进程
        if (fileMap[i].pid != 0 && fileMap[i].ppid == temp_procfile->pid)
        {
            temp_procfile->root[temp_procfile->count++] = i;
            // i
            dfs_create_tree(i);
        }
    }
}

int findPid(int pid)
{
    for (int i = 0; i < fileMap_count; i++)
    {
        if (fileMap[i].pid == pid)
        {
            return i;
        }
    }
    return -1;
}