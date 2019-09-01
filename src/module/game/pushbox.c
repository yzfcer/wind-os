/***********************************************
文件名称：main.c
文件描述：C语言写一个简单的小游戏 
编译环境：DEV C++ 
初写时间：2018年8月3日 
修改时间：2018年8月3日  
***********************************************/
#include<stdio.h>
#include<stdlib.h> 
#include<conio.h> 
#include<windows.h>



//原始的图表，五行六列，其中 0 代表着空白的地方； 1 代表着墙；2 代表着人；
//3 代表着箱子；4 代表着箱子的终点位置。 
//图的变化要靠自己来编写数组，通过数字来进行图的构造。
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
        printf("  "); //空白的地方
    else if(point == 1)
        printf("■"); //墙 
    else if(point == 2)
        printf("♀"); //人 
    else if(point == 3)
        printf("☆"); //箱子 
    else if(point == 4)
        printf("◎"); //终点地方 
    else if(point == 5)
        printf("♂");//人加终点位置 
    else if(point == 6)
        printf("★") ;//箱子加终点位置
}

//把图形刻画出来
int draw_map()
{	
    int i,j;
    check_result();//调用输赢的函数 
    for(i=0;i<9;i++)
    {
        for(j=0;j<11;j++)
        {
            print_map_point(map[i][j]);
        }
        printf("\n");
    }
    return 0;
} 

/*进行小人的上下左右的移动
移动的情况：
1.人的前面是空地；
2.人的前面是终点位置；
3.人的前面是箱子
3.1.箱子的前面是空地；
3.2.箱子的前面是终点位置。
不移动的情况：
1.人的前面是墙；
2.人的前面是箱子；
2.1.箱子的前面是墙 ； 
2.2.箱子的前面是箱子；
*/
//分析后，要进行确定人的位置以及胜利的条件。 

//进行小人的移动，整个移动的过程就是数组变化的过程 
int push_box()
{
    int i,j,count,caw;//行和列 
    int push;
    for(i=0;i<9;i++)//确定人的位置 
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
    push=_getch();//与getchar()有区别的是：getchar()输入一个字符后需要回车来进行下一个字符的输入，
    //比较麻烦 ，_getch()则不需要回车就能连续输入多个字符。 
    if(push == 0x1B)
        return 0;
    switch(push)
    {//上
    case 'W':
    case 72:
    // 1.人的前面是空地；
    // 2.人的前面是终点位置；
    // 3.人的前面是箱子
    //3.1.箱子的前面是空地；
    //3.2.箱子的前面是终点位置。
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

    /* 移动的情况：
    位置：
    人   map[count][caw]
    人的前面是空地   map[count-1][caw]
    人的前面是终点位置   map[count-1][caw]
    箱子的前面是空地或终点位置  map[count-2][caw]*/ 

    //下 
    case 'S':
    case 80://键值 
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
    //左 
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
    //右 
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

//整个游戏的输赢
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
        printf("恭喜你，你赢了！\n");
    return 0;
}

int pushbox_main(int argc,char **argv)//主函数 
{
    int res;
    while(1)
    {
        system("cls");//对其进行清屏 
        draw_map();
        res = push_box();
        if(res == 0)
            break;
    }
    return 0;
} 

