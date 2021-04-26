#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>
//信号量： empty[计数信号量] \full[计数信号量]\mutex[互斥锁]

//缓冲区：循环队列

typedef int buffer_item;

#define RAND_MAXX 50
#define ITEM_SUM 20

HANDLE hMutex;//声明互斥量
HANDLE hSemaphore_full;//声明信号量full
HANDLE hSemaphore_empty;//声明信号量empty
HANDLE handle[5];//创建生产者线程\消费者线程


buffer_item buffer[ITEM_SUM];//缓冲区
int tail = -1;//循环队列的尾指针
int head = 0;//循环队列的头指针




//缓冲区操作：insert_item()[生产者线程]、remove_item()[消费者线程]
//缓冲区还需要初始化函数，初始化互斥对象mutex、信号量empty、full

int insert_item(buffer_item item)
{

    buffer[(++tail) % ITEM_SUM] = item;
    //生产者生产物品到缓冲区
    //成功return 0，否则-1
    return 0;
}

int remove_item(buffer_item item)
{
    item = buffer[head % ITEM_SUM];
    head = (head + 1) % ITEM_SUM;
    //消费者消费缓冲区的物品
    //成功return 0，否则-1
    return item;
}


//生产者与消费者的线程框架
DWORD WINAPI produce(void* param)
{
    buffer_item item;
    int* flag = (int*)param;
    while (1)
    {
        //睡眠一段随机时间
        int tem = rand() % 100 + 1;
        Sleep(tem * 100);
        //创建一个随机数
        item = rand() % RAND_MAXX + 1;//生产一个产品

        if (!ReleaseSemaphore(hSemaphore_empty, -1, NULL))//P(empty),失败返回为0
        {
            WaitForSingleObject(hSemaphore_empty, INFINITE);//无限等待
            ReleaseSemaphore(hSemaphore_empty, -1, NULL);
        }

        WaitForSingleObject(hMutex, INFINITE);//访问互斥量，同时只能有一个访问
        //将一个产品送入缓冲区
        if (insert_item(item))
        {
            printf("report error condition");
        }
        else
        {
            printf("produce %d produce %d   %d\n", *flag, item,(tail+1)%20);
        }

        ReleaseMutex(hMutex);//访问完毕，释放互斥量


        if (!ReleaseSemaphore(hSemaphore_full, 1, NULL))//V(full),失败返回为0
        {
            WaitForSingleObject(hSemaphore_full, INFINITE);//无限等待
            ReleaseSemaphore(hSemaphore_full, 1, NULL);
        }
    }
}

DWORD WINAPI consume(void* param)
{
    buffer_item item=0;
    int* flag = (int*)param;
    while (1)
    {
        //睡眠一段随机时间
        int tem = rand() % 100 + 1;
        Sleep(tem*100);
        //创建一个随机数

        if (!ReleaseSemaphore(hSemaphore_full, -1, NULL))//P(full),失败返回为0
        {
            WaitForSingleObject(hSemaphore_full, INFINITE);//无限等待
            ReleaseSemaphore(hSemaphore_full, -1, NULL);
        }

        WaitForSingleObject(hMutex, INFINITE);//访问互斥量，同时只能有一个访问

        item = remove_item(item);
        printf("                                    consumer %d cosumed %d %d\n", *flag, item,head);


        ReleaseMutex(hMutex);//访问完毕，释放互斥量

        if (!ReleaseSemaphore(hSemaphore_empty, 1, NULL))//V(empty),失败返回为0
        {
            WaitForSingleObject(hSemaphore_empty, INFINITE);//无限等待
            ReleaseSemaphore(hSemaphore_empty, 1, NULL);
        }
    }
}



int main()
{
    srand((unsigned int)(time(NULL)));
    //命令行参数
    //初始化缓冲区
    hMutex = CreateMutex(NULL, FALSE, NULL);//创建互斥量
    hSemaphore_full = CreateSemaphore(NULL, 0, ITEM_SUM, NULL);//创建信号量
    hSemaphore_empty = CreateSemaphore(NULL, ITEM_SUM, ITEM_SUM, NULL);//创建信号量

    //创建生产者线程
    int produce_num[3] = { 1,2,3 };
    handle[0] = CreateThread(NULL, 0, produce, &produce_num[0], 0, NULL);
    handle[1] = CreateThread(NULL, 0, produce, &produce_num[1], 0, NULL);
    handle[2] = CreateThread(NULL, 0, produce, &produce_num[2], 0, NULL);
    //创建消费者线程
    int consume_num[2] = { 1,2 };
    handle[3] = CreateThread(NULL, 0, consume, &consume_num[0], 0, NULL);
    handle[4] = CreateThread(NULL, 0, consume, &consume_num[1], 0, NULL);
    //Sleep


    WaitForMultipleObjects(5, handle, TRUE, INFINITE);

    //结束
    CloseHandle(hMutex);//关闭互斥量
    CloseHandle(hSemaphore_full);//关闭信号量full
    CloseHandle(hSemaphore_empty);//关闭信号量empty

    Sleep(1000);

    system("PAUSE");
    return 0;
}