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
            map->va_arr[i][j] = 1; //������е�ǽ
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




void setMap (map_s *map)//��ʼ�������ͼ
{
	int i,j;
	int dir,set;
	int rows,cols;
    int nowR = 1,temp=1; //��ǰ�����λ�ã����洢�������ʱ����
	rows = map->rows;
	cols = map->cols;
    init_map(map);
    set_map_hole(map);
    set_map_box(map);

#if 0
    for (i = 1; i < rows /2 ;i+=2)
    {
        char *pRwo = map->va_arr[i]; //ָ��ָ���һ������

    loop:
        dir = arc4random_uniform(2)+1; //��ȡ�����������
        set = arc4random_uniform(cols-2-2)+2;//��ȡ�������ֵ

        switch (dir)
        {
            case 1:  //�����ڿ��ƶ�
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
            case 2: //�����ڿ��ƶ�
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
        map->va_arr[i+2][nowR] = 0; //�����ڿ��ƶ�
        map->va_arr[i+2][nowR+1] = 0;
        map->va_arr[i+2][nowR-1] = 0;
        map->va_arr[i+3][nowR] = 0; //�����ڿ��ƶ�
        map->va_arr[i+3][nowR+1] = 0;
        map->va_arr[i+3][nowR-1] = 0;

    }

    for (i = rows-2; i > rows /2 ;i-=2) //���������ڿӣ����м�ֵ��ֹ
    {
        char *pRwo = map->va_arr[i]; //ָ��ָ���һ������

    loop2:
        printf ("");
        dir = arc4random_uniform(2)+1; //�����������
        set = arc4random_uniform(cols-2-2)+2;

        switch (dir)
        {
            case 1:  //�����ڿ��ƶ�
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
            case 2: //�����ڿ��ƶ�
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
        map->va_arr[i-2][nowR] = 0; //�����ƶ�
        map->va_arr[i-2][nowR+1] = 0;
        map->va_arr[i-2][nowR-1] = 0;
        map->va_arr[i-3][nowR] = 0; //�����ƶ�
        map->va_arr[i-3][nowR+1] = 0;
        map->va_arr[i-3][nowR-1] = 0;
    }
    for (i = 1; i < cols-1; i++ ) //Ϊĩ�С����Ⱥ��м�����ȫ��
    {
        map->va_arr[rows-2][i]= 0;
        map->va_arr[rows-3][i]= 0;
        map->va_arr[rows/2][i]= 0;
        map->va_arr[1][i]= 0;
        map->va_arr[2][i]= 0;
    }
    map->va_arr[rows-1][cols-2] = 0; //��ʼ���յ㡢С�ˡ�����λ��
    map->va_arr[1][1] = 3;
    map->va_arr[2][2] = 4;
#endif
}

void printMap (map_s *map)//��ӡ��ͼ
{
	int i,j;
    for (i = 0; i < map->rows; i++ )
    {
        for (j = 0; j < map->cols; j++)
        {
            switch(map->va_arr[i][j])
   	   	   {
   	   	   	    case 0:
   	   	   	    	printf("  "); //�հ׵ĵط�
   	   	   	    	break;
   	   	   	    case 1:
   	   	   	    	printf("��"); //ǽ 
   	   	   	    	break;
   	   	   	    case 2:
   	   	   	    	printf("��"); //�� 
				    break;
				case 3:
					printf("��"); //���� 
				    break;
			    case 4:
			    	printf("��"); //�յ�ط� 
				     break; 
				case 6:
					printf("��");//�˼��յ�λ�� 
					break;
			    case 7: 
				    printf("��") ;//���Ӽ��յ�λ��
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
 

//��ͼ�ο̻�����
int draw_map()
{	
    int i,j;
    check_result();//������Ӯ�ĺ��� 
	for(i=0;i<9;i++)
	{
	   for(j=0;j<11;j++)
	   	   {
	   	   	   switch(map[i][j])
	   	   	   {
	   	   	   	    case 0:
	   	   	   	    	printf("  "); //�հ׵ĵط�
	   	   	   	    	break;
	   	   	   	    case 1:
	   	   	   	    	printf("��"); //ǽ 
	   	   	   	    	break;
	   	   	   	    case 2:
	   	   	   	    	printf("��"); //�� 
					    break;
					case 3:
						printf("��"); //���� 
					    break;
				    case 4:
				    	printf("��"); //�յ�ط� 
					     break; 
					case 6:
						printf("��");//�˼��յ�λ�� 
						break;
				    case 7: 
					    printf("��") ;//���Ӽ��յ�λ��
						break;
					 }
			  }
	   printf("\n");
	}	   
} 
 
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
    if(argc == 2)
    {
        map_test();
        return 0;
            
    }
	while(1)
    {
    	 system("cls");//����������� 
         draw_map();
         push_box();
       
    }
    printf("shuchu \n");
	return 0;
 } 

