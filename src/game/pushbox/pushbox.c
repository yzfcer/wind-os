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

typedef struct __map_s
{
	int rows,cols;
    int hole_cnt;
	char **va_arr;
}map_s;

static int alloc_map(map_s *map,int rows,int cols)
{
	int i;
    if(rows < 5)
        rows = 5;
    if(cols < 5)
        cols = 5;
    
	map->va_arr = malloc(rows * sizeof(char*));
	for(i = 0;i < rows;i ++)
		map->va_arr[i] = malloc(cols * sizeof(char));
    map->rows = rows;
    map->cols = cols;
    map->hole_cnt = 0;
	return 0;
}

static void free_map(map_s *map)
{
	int i;
	for(i = 0;i < map->rows;i ++)
		free(map->va_arr[i]);
	free(map->va_arr); 
    map->rows = 0;
    map->cols = 0;
}

static int arc4random_uniform(int n)
{
    int a;
    a=rand()%n;
    return a;
}

void init_map(map_s *map)
{
	int i,j;
    for (i = 0; i < map->rows; i++)
    {
        for (j = 0; j < map->cols; j++)
        {
            map->va_arr[i][j] = 1; //填充所有的墙
        }
    }
}

void set_map_hole(map_s *map)
{
	int i,j;
    int rand;
    for (i = 1; i < map->rows - 1; i++)
    {
        for (j = 1; j < map->cols - 1; j++)
        {
            rand = arc4random_uniform(4);
            if(rand == 1)
                continue;
            map->va_arr[i][j] = 0;
            map->hole_cnt ++;
        }
    }
}

int set_map_box_idx(map_s *map,int index)
{
	int i,j;
    int idx = 0;
    for (i = 1; i < map->rows - 1; i++)
    {
        for (j = 1; j < map->cols - 1; j++)
        {
            if(map->va_arr[i][j] != 1)
                idx ++;
            if(idx >= index)
            {
                if(map->va_arr[i][j] == 0)
                {
                    map->va_arr[i][j] = 3;
                    return 1;
                }
                return 0;
            }
        }
    }
    return 0;
}

void set_map_box(map_s *map)
{
    int i,j;
    int idx;
    int boxnum = map->cols * map->cols / 30;
    for (i = 1; i < map->rows - 1; i++)
    {
        for (j = 1; j < map->cols - 1; j++)
        {
            idx = arc4random_uniform(map->hole_cnt);
            if(set_map_box_idx(map,idx))
                boxnum --;
            if(boxnum <= 0)
                return;
        }
    }    
}




void setMap (map_s *map)//初始化随机地图
{
	int i,j;
	int dir,set;
	int rows,cols;
    int nowR = 1,temp=1; //当前座标点位置，及存储座标的临时变量
	rows = map->rows;
	cols = map->cols;
    init_map(map);
    set_map_hole(map);
    set_map_box(map);

#if 0
    for (i = 1; i < rows /2 ;i+=2)
    {
        char *pRwo = map->va_arr[i]; //指针指向第一行数组

    loop:
        dir = arc4random_uniform(2)+1; //获取左右随机方向
        set = arc4random_uniform(cols-2-2)+2;//获取随机步进值

        switch (dir)
        {
            case 1:  //向左挖坑移动
                if (nowR-2 < set)
                {
                    goto loop;
                }
                else
                {
                    for (j = nowR; j > nowR-set; j--)
                    {
                    *(pRwo+j) = 0;
                    map->va_arr[i+1][j] = 0;
                        temp = j;
                    }
                    nowR = temp;
                    break;
                }
            case 2: //向右挖坑移动
                if (set > cols -2 - nowR)
                {
                    goto loop;
                }
                else
                {
                    for (j = nowR; j < nowR+set; j++)
                    {
                         *(pRwo+j) = 0;
                        map->va_arr[i+1][j] = 0;
                        temp =j;
                    }
                    nowR = temp;
                    break;
                }

        }
        map->va_arr[i+2][nowR] = 0; //向下挖坑移动
        map->va_arr[i+2][nowR+1] = 0;
        map->va_arr[i+2][nowR-1] = 0;
        map->va_arr[i+3][nowR] = 0; //向下挖坑移动
        map->va_arr[i+3][nowR+1] = 0;
        map->va_arr[i+3][nowR-1] = 0;

    }

    for (i = rows-2; i > rows /2 ;i-=2) //由下往上挖坑，到中间值截止
    {
        char *pRwo = map->va_arr[i]; //指针指向第一行数组

    loop2:
        printf ("");
        dir = arc4random_uniform(2)+1; //左右随机方向
        set = arc4random_uniform(cols-2-2)+2;

        switch (dir)
        {
            case 1:  //向左挖坑移动
                if (nowR-2 < set)
                {
                    goto loop2;
                }
                else
                {
                    for (j = nowR; j > nowR-set; j--)
                    {
                        *(pRwo+j) = 0;
                        map->va_arr[i-1][j] = 0;
                        temp = j;
                    }
                    nowR = temp;
                    break;
                }
            case 2: //向右挖坑移动
                if (set > cols -2 - nowR)
                {
                    goto loop2;
                }
                else
                {
                    for (j = nowR; j < nowR+set; j++)
                    {
                        *(pRwo+j) = 0;
                        map->va_arr[i-1][j] = 0;
                        temp =j;
                    }
                    nowR = temp;
                    break;
                }

        }
        map->va_arr[i-2][nowR] = 0; //向下移动
        map->va_arr[i-2][nowR+1] = 0;
        map->va_arr[i-2][nowR-1] = 0;
        map->va_arr[i-3][nowR] = 0; //向下移动
        map->va_arr[i-3][nowR+1] = 0;
        map->va_arr[i-3][nowR-1] = 0;
    }
    for (i = 1; i < cols-1; i++ ) //为末行、首先和中间行挖全坑
    {
        map->va_arr[rows-2][i]= 0;
        map->va_arr[rows-3][i]= 0;
        map->va_arr[rows/2][i]= 0;
        map->va_arr[1][i]= 0;
        map->va_arr[2][i]= 0;
    }
    map->va_arr[rows-1][cols-2] = 0; //初始化终点、小人、箱子位置
    map->va_arr[1][1] = 3;
    map->va_arr[2][2] = 4;
#endif
}

void printMap (map_s *map)//打印地图
{
	int i,j;
    for (i = 0; i < map->rows; i++ )
    {
        for (j = 0; j < map->cols; j++)
        {
            switch(map->va_arr[i][j])
   	   	   {
   	   	   	    case 0:
   	   	   	    	printf("  "); //空白的地方
   	   	   	    	break;
   	   	   	    case 1:
   	   	   	    	printf("■"); //墙 
   	   	   	    	break;
   	   	   	    case 2:
   	   	   	    	printf("♀"); //人 
				    break;
				case 3:
					printf("☆"); //箱子 
				    break;
			    case 4:
			    	printf("◎"); //终点地方 
				     break; 
				case 6:
					printf("♂");//人加终点位置 
					break;
			    case 7: 
				    printf("★") ;//箱子加终点位置
					break;
			 }
        }
        printf ("\n");
    }
}


void map_test(void)
{
    map_s tmap;
    alloc_map(&tmap,25,25);
    setMap(&tmap);
    printMap(&tmap);
    free_map(&tmap);
}

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
 

//把图形刻画出来
int draw_map()
{	
    int i,j;
    check_result();//调用输赢的函数 
	for(i=0;i<9;i++)
	{
	   for(j=0;j<11;j++)
	   	   {
	   	   	   switch(map[i][j])
	   	   	   {
	   	   	   	    case 0:
	   	   	   	    	printf("  "); //空白的地方
	   	   	   	    	break;
	   	   	   	    case 1:
	   	   	   	    	printf("■"); //墙 
	   	   	   	    	break;
	   	   	   	    case 2:
	   	   	   	    	printf("♀"); //人 
					    break;
					case 3:
						printf("☆"); //箱子 
					    break;
				    case 4:
				    	printf("◎"); //终点地方 
					     break; 
					case 6:
						printf("♂");//人加终点位置 
						break;
				    case 7: 
					    printf("★") ;//箱子加终点位置
						break;
					 }
			  }
	   printf("\n");
	}	   
} 
 
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
	push=getch();//与getchar()有区别的是：getchar()输入一个字符后需要回车来进行下一个字符的输入，
	                //比较麻烦 ，getch()则不需要回车就能连续输入多个字符。 
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
}

int pushbox_main(int argc,char **argv)//主函数 
{
    if(argc == 2)
    {
        map_test();
        return 0;
            
    }
	while(1)
    {
    	 system("cls");//对其进行清屏 
         draw_map();
         push_box();
       
    }
    printf("shuchu \n");
	return 0;
 } 

