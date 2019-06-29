/***********************************************
�ļ����ƣ�main.c
�ļ�������C����дһ���򵥵�С��Ϸ 
���뻷����DEV C++ 
��дʱ�䣺2018��8��3�� 
�޸�ʱ�䣺2018��8��3��  
***********************************************/
#include<stdio.h>
#include<stdlib.h> 
#include<conio.h> 
#include<windows.h>



//ԭʼ��ͼ���������У����� 0 �����ſհ׵ĵط��� 1 ������ǽ��2 �������ˣ�
//3 ���������ӣ�4 ���������ӵ��յ�λ�á� 
//ͼ�ı仯Ҫ���Լ�����д���飬ͨ������������ͼ�Ĺ��졣
int map[9][11]={
    {0,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,1,0,0,0,1,0},
    {0,1,0,3,3,3,3,3,0,1,0},
    {0,1,0,3,0,3,0,3,0,1,1},
    {0,1,0,0,0,2,0,0,3,0,1},
    {1,1,0,1,1,1,1,0,3,0,1},
    {1,0,4,4,4,4,4,1,0,0,1},
    {1,0,4,4,4,4,4,0,0,1,1},
    {1,1,1,1,1,1,1,1,1,1,0}
};

int draw_map();
int push_box();
int check_result();

static void print_map_point(int point)
{
    if(point == 0)
        printf("  "); //�հ׵ĵط�
    else if(point == 1)
        printf("��"); //ǽ 
    else if(point == 2)
        printf("��"); //�� 
    else if(point == 3)
        printf("��"); //���� 
    else if(point == 4)
        printf("��"); //�յ�ط� 
    else if(point == 5)
        printf("��");//�˼��յ�λ�� 
    else if(point == 6)
        printf("��") ;//���Ӽ��յ�λ��
}

//��ͼ�ο̻�����
int draw_map()
{	
    int i,j;
    check_result();//������Ӯ�ĺ��� 
    for(i=0;i<9;i++)
    {
        for(j=0;j<11;j++)
        {
            print_map_point(map[i][j]);
        }
        printf("\n");
    }	   
} 

/*����С�˵��������ҵ��ƶ�
�ƶ��������
1.�˵�ǰ���ǿյأ�
2.�˵�ǰ�����յ�λ�ã�
3.�˵�ǰ��������
3.1.���ӵ�ǰ���ǿյأ�
3.2.���ӵ�ǰ�����յ�λ�á�
���ƶ��������
1.�˵�ǰ����ǽ��
2.�˵�ǰ�������ӣ�
2.1.���ӵ�ǰ����ǽ �� 
2.2.���ӵ�ǰ�������ӣ�
*/
//������Ҫ����ȷ���˵�λ���Լ�ʤ���������� 

//����С�˵��ƶ��������ƶ��Ĺ��̾�������仯�Ĺ��� 
int push_box()
{
    int i,j,count,caw;//�к��� 
    int push;
    for(i=0;i<9;i++)//ȷ���˵�λ�� 
    {
        for (j=0;j<11;j++)
        {
            if(map[i][j]==2||map[i][j]==6)
            {
                count=i;
                caw=j;
            }
        }
    } 
    push=getch();//��getchar()��������ǣ�getchar()����һ���ַ�����Ҫ�س���������һ���ַ������룬
    //�Ƚ��鷳 ��getch()����Ҫ�س����������������ַ��� 
    if(push == 0x1B)
        return 0;
    switch(push)
    {//��
    case 'W':
    case 72:
    // 1.�˵�ǰ���ǿյأ�
    // 2.�˵�ǰ�����յ�λ�ã�
    // 3.�˵�ǰ��������
    //3.1.���ӵ�ǰ���ǿյأ�
    //3.2.���ӵ�ǰ�����յ�λ�á�
    if(map[count-1][caw]==0||map[count-1][caw]==4)
    {
        map[count][caw]-=2;
        map[count-1][caw]+=2;
    } 
    else if(map[count-1][caw]==3||map[count-1][caw]==7)
    {
        if(map[count-2][caw]==0||map[count-2][caw]==4)
        {
            map[count][caw]-=2;
            map[count-1][caw]-=1;
            map[count-2][caw]+=3;
        }
    }
    break;

    /* �ƶ��������
    λ�ã�
    ��   map[count][caw]
    �˵�ǰ���ǿյ�   map[count-1][caw]
    �˵�ǰ�����յ�λ��   map[count-1][caw]
    ���ӵ�ǰ���ǿյػ��յ�λ��  map[count-2][caw]*/ 

    //�� 
    case 'S':
    case 80://��ֵ 
    if(map[count+1][caw]==0||map[count+1][caw]==4)
    {
        map[count][caw]-=2;
        map[count+1][caw]+=2;
    }

    else if(map[count+2][caw]==0||map[count+2][caw]==4)
    {
        if(map[count+1][caw]==3||map[count+1][caw]==7)
        {
            map[count][caw]-=2;
            map[count+1][caw]-=1;
            map[count+2][caw]+=3;
        }
    }
    break;
    //�� 
    case 'A':
    case 75:
    if(map[count][caw-1]==0||map[count][caw-1]==4)
    {
        map[count][caw]-=2;
        map[count][caw-1]+=2;
    }

    else if(map[count][caw-2]==0||map[count][caw-2]==4)
    {
        if(map[count][caw-1]==3||map[count][caw-1]==7)
        {
            map[count][caw]-=2;
            map[count][caw-1]-=1;
            map[count][caw-2]+=3;
        }
    }
    break;
    //�� 
    case 'D':
    case 77:
    if(map[count][caw+1]==0||map[count][caw+1]==4)
    {
        map[count][caw]-=2;
        map[count][caw+1]+=2;
    }

    else if(map[count][caw+2]==0||map[count][caw+2]==4)
    {
        if(map[count][caw+1]==3||map[count][caw+1]==7)
        {
            map[count][caw]-=2;
            map[count][caw+1]-=1;
            map[count][caw+2]+=3;
        }
    }
    break;

    } 
    return 1;
}

//������Ϸ����Ӯ
int check_result() 
{
    int i,j,k = 0;
    for(i=0;i<9;i++)
    {
        for (j=0;j<11;j++)
        {
            if(map[i][j]==3)
                k++;
        }
    }
    if(k==0)
        printf("��ϲ�㣬��Ӯ�ˣ�\n");
}

int pushbox_main(int argc,char **argv)//������ 
{
    int res;
    while(1)
    {
        system("cls");//����������� 
        draw_map();
        res = push_box();
        if(res == 0)
            break;
    }
    return 0;
} 

