#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>

//��ѧ�ҾͲ�����
//���õ���ѧ������ΪNʱ���������N-1����ѧ��ͬʱ�����������������

#define PHI_num 5

HANDLE phi_mutex;//�������͵���ѧ���������Ƶ��ź�������ֵΪn-1
HANDLE chopstick[PHI_num];//���ӵĻ���������ֵ��Ϊ1
HANDLE handle[PHI_num];//������ѧ���߳�

DWORD WINAPI phi_eat(void* param)
{
    int* flag = (int*)param;
    while (1)
    {
        int tem = rand() % 10 + 1;
        Sleep(tem * 100);
        if (!ReleaseSemaphore(phi_mutex, -1, NULL))//P(phi_mutex),ʧ�ܷ���Ϊ0
        {
            WaitForSingleObject(phi_mutex, INFINITE);//���޵ȴ�
            ReleaseSemaphore(phi_mutex, -1, NULL);
        }

        WaitForSingleObject(chopstick[*flag - 1], INFINITE);//���ʻ�������ͬʱֻ����һ������
        printf("��ѧ��%d�����%d������\n", *flag, *flag);
        WaitForSingleObject(chopstick[*flag % 5], INFINITE);//���ʻ�������ͬʱֻ����һ������
        printf("��ѧ��%d�����%d������\n", *flag, *flag % 5 + 1);


        printf("                                                   ��ѧ��%d��ʼ����\n", *flag);
        tem = rand() % 10 + 1;
        Sleep(tem * 100);


        ReleaseMutex(chopstick[*flag - 1]);//������ϣ��ͷŻ�����
        printf("                          ��ѧ��%d���µ�%d������\n", *flag, *flag);
        ReleaseMutex(chopstick[*flag % 5]);
        printf("                          ��ѧ��%d���µ�%d������\n", *flag, (*flag % 5) + 1);

        printf("                                                   ��ѧ��%d��˼��\n", *flag);
        if (!ReleaseSemaphore(phi_mutex, 1, NULL))//V(phi_mutex),ʧ�ܷ���Ϊ0
        {
            WaitForSingleObject(phi_mutex, INFINITE);//���޵ȴ�
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

    phi_mutex = CreateSemaphore(NULL, PHI_num - 1, PHI_num - 1, NULL);//ȡֵ[0,PHI_num-1]
    for (int i = 0; i < PHI_num; i++)
    {
        chopstick[i] = CreateMutex(NULL, FALSE, NULL);//����������
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