#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>
//�ź����� empty[�����ź���] \full[�����ź���]\mutex[������]

//��������ѭ������

typedef int buffer_item;

#define RAND_MAXX 50
#define ITEM_SUM 20

HANDLE hMutex;//����������
HANDLE hSemaphore_full;//�����ź���full
HANDLE hSemaphore_empty;//�����ź���empty
HANDLE handle[5];//�����������߳�\�������߳�


buffer_item buffer[ITEM_SUM];//������
int tail = -1;//ѭ�����е�βָ��
int head = 0;//ѭ�����е�ͷָ��




//������������insert_item()[�������߳�]��remove_item()[�������߳�]
//����������Ҫ��ʼ����������ʼ���������mutex���ź���empty��full

int insert_item(buffer_item item)
{

    buffer[(++tail) % ITEM_SUM] = item;
    //������������Ʒ��������
    //�ɹ�return 0������-1
    return 0;
}

int remove_item(buffer_item item)
{
    item = buffer[head % ITEM_SUM];
    head = (head + 1) % ITEM_SUM;
    //���������ѻ���������Ʒ
    //�ɹ�return 0������-1
    return item;
}


//�������������ߵ��߳̿��
DWORD WINAPI produce(void* param)
{
    buffer_item item;
    int* flag = (int*)param;
    while (1)
    {
        //˯��һ�����ʱ��
        int tem = rand() % 100 + 1;
        Sleep(tem * 100);
        //����һ�������
        item = rand() % RAND_MAXX + 1;//����һ����Ʒ

        if (!ReleaseSemaphore(hSemaphore_empty, -1, NULL))//P(empty),ʧ�ܷ���Ϊ0
        {
            WaitForSingleObject(hSemaphore_empty, INFINITE);//���޵ȴ�
            ReleaseSemaphore(hSemaphore_empty, -1, NULL);
        }

        WaitForSingleObject(hMutex, INFINITE);//���ʻ�������ͬʱֻ����һ������
        //��һ����Ʒ���뻺����
        if (insert_item(item))
        {
            printf("report error condition");
        }
        else
        {
            printf("produce %d produce %d   %d\n", *flag, item,(tail+1)%20);
        }

        ReleaseMutex(hMutex);//������ϣ��ͷŻ�����


        if (!ReleaseSemaphore(hSemaphore_full, 1, NULL))//V(full),ʧ�ܷ���Ϊ0
        {
            WaitForSingleObject(hSemaphore_full, INFINITE);//���޵ȴ�
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
        //˯��һ�����ʱ��
        int tem = rand() % 100 + 1;
        Sleep(tem*100);
        //����һ�������

        if (!ReleaseSemaphore(hSemaphore_full, -1, NULL))//P(full),ʧ�ܷ���Ϊ0
        {
            WaitForSingleObject(hSemaphore_full, INFINITE);//���޵ȴ�
            ReleaseSemaphore(hSemaphore_full, -1, NULL);
        }

        WaitForSingleObject(hMutex, INFINITE);//���ʻ�������ͬʱֻ����һ������

        item = remove_item(item);
        printf("                                    consumer %d cosumed %d %d\n", *flag, item,head);


        ReleaseMutex(hMutex);//������ϣ��ͷŻ�����

        if (!ReleaseSemaphore(hSemaphore_empty, 1, NULL))//V(empty),ʧ�ܷ���Ϊ0
        {
            WaitForSingleObject(hSemaphore_empty, INFINITE);//���޵ȴ�
            ReleaseSemaphore(hSemaphore_empty, 1, NULL);
        }
    }
}



int main()
{
    srand((unsigned int)(time(NULL)));
    //�����в���
    //��ʼ��������
    hMutex = CreateMutex(NULL, FALSE, NULL);//����������
    hSemaphore_full = CreateSemaphore(NULL, 0, ITEM_SUM, NULL);//�����ź���
    hSemaphore_empty = CreateSemaphore(NULL, ITEM_SUM, ITEM_SUM, NULL);//�����ź���

    //�����������߳�
    int produce_num[3] = { 1,2,3 };
    handle[0] = CreateThread(NULL, 0, produce, &produce_num[0], 0, NULL);
    handle[1] = CreateThread(NULL, 0, produce, &produce_num[1], 0, NULL);
    handle[2] = CreateThread(NULL, 0, produce, &produce_num[2], 0, NULL);
    //�����������߳�
    int consume_num[2] = { 1,2 };
    handle[3] = CreateThread(NULL, 0, consume, &consume_num[0], 0, NULL);
    handle[4] = CreateThread(NULL, 0, consume, &consume_num[1], 0, NULL);
    //Sleep


    WaitForMultipleObjects(5, handle, TRUE, INFINITE);

    //����
    CloseHandle(hMutex);//�رջ�����
    CloseHandle(hSemaphore_full);//�ر��ź���full
    CloseHandle(hSemaphore_empty);//�ر��ź���empty

    Sleep(1000);

    system("PAUSE");
    return 0;
}