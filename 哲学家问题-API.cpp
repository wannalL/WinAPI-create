#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>

//哲学家就餐问题
//采用当哲学家数量为N时，最多允许N-1个哲学家同时进餐来解决死锁问题

#define PHI_num 5

HANDLE phi_mutex;//声明进餐的哲学家数量限制的信号量，初值为n-1
HANDLE chopstick[PHI_num];//筷子的互斥量，初值均为1
HANDLE handle[PHI_num];//创建哲学家线程

DWORD WINAPI phi_eat(void* param)
{
    int* flag = (int*)param;
    while (1)
    {
        int tem = rand() % 10 + 1;
        Sleep(tem * 100);
        if (!ReleaseSemaphore(phi_mutex, -1, NULL))//P(phi_mutex),失败返回为0
        {
            WaitForSingleObject(phi_mutex, INFINITE);//无限等待
            ReleaseSemaphore(phi_mutex, -1, NULL);
        }

        WaitForSingleObject(chopstick[*flag - 1], INFINITE);//访问互斥量，同时只能有一个访问
        printf("哲学家%d拿起第%d根筷子\n", *flag, *flag);
        WaitForSingleObject(chopstick[*flag % 5], INFINITE);//访问互斥量，同时只能有一个访问
        printf("哲学家%d拿起第%d根筷子\n", *flag, *flag % 5 + 1);


        printf("                                                   哲学家%d开始进餐\n", *flag);
        tem = rand() % 10 + 1;
        Sleep(tem * 100);


        ReleaseMutex(chopstick[*flag - 1]);//访问完毕，释放互斥量
        printf("                          哲学家%d放下第%d根筷子\n", *flag, *flag);
        ReleaseMutex(chopstick[*flag % 5]);
        printf("                          哲学家%d放下第%d根筷子\n", *flag, (*flag % 5) + 1);

        printf("                                                   哲学家%d在思考\n", *flag);
        if (!ReleaseSemaphore(phi_mutex, 1, NULL))//V(phi_mutex),失败返回为0
        {
            WaitForSingleObject(phi_mutex, INFINITE);//无限等待
            ReleaseSemaphore(phi_mutex, 1, NULL);
        }
       
        tem = rand() % 10 + 1;
        Sleep(tem * 100);



    }

}

int main()
{
    int phi[PHI_num];
    srand((unsigned int)(time(NULL)));

    phi_mutex = CreateSemaphore(NULL, PHI_num - 1, PHI_num - 1, NULL);//取值[0,PHI_num-1]
    for (int i = 0; i < PHI_num; i++)
    {
        chopstick[i] = CreateMutex(NULL, FALSE, NULL);//创建互斥量
        phi[i] = i + 1;
    }

    for (int i = 0; i < PHI_num; i++)
    {
        handle[i] = CreateThread(NULL, 0, phi_eat, &phi[i], 0, NULL);
    }

    WaitForMultipleObjects(PHI_num, handle, TRUE, INFINITE);

    CloseHandle(phi_mutex);
    for (int i = 0; i < PHI_num; i++)
    {
        CloseHandle(chopstick[i]);
    }


    system("PAUSE");
    return 0;
}