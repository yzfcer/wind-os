#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>

#define U 1
#define D 2
#define L 3 
#define R 4       //�ߵ�״̬��U���� ��D���£�L:�� R����

typedef struct SNAKE //�����һ���ڵ�
{
    int x;
    int y;
    struct SNAKE *next;
}snake;

typedef struct __snake_ctx_s
{
    int score; 
    int add;//�ܵ÷���ÿ�γ�ʳ��÷֡�
    int status;
    int movetime;  //����ʱ��
    snake *head;//��ͷָ��
    snake *food;//ʳ��ָ��
    snake *q;   //�����ߵ�ʱ���õ���ָ��
    int end_status; //��Ϸ�����������1��ײ��ǽ��2��ҧ���Լ���3�������˳���Ϸ��
}snake_ctx_s;

snake_ctx_s ctx;


//����ȫ������//
static void param_init(void);
static void Pos();
static void creatMap();
static void initSnake();
static int biteSelf();
static void createFood();
static int cantCrossWall();
static int snakeMove();
static void pause();
static int runGame();
static void initGame();
static int endGame();
static void gameStart();

static void param_init(void)
{
    ctx.score = 0;
    ctx.add = 10;//�ܵ÷���ÿ�γ�ʳ��÷֡�
    ctx.status = 0;
    ctx.head = NULL;//��ͷָ��
    ctx.food = NULL;//ʳ��ָ��
    ctx.movetime = 500;
    ctx.q = NULL;//�����ߵ�ʱ���õ���ָ��
    ctx.end_status = 0; //��Ϸ�����������1��ײ��ǽ��2��ҧ���Լ���3�������˳���Ϸ��
}

static void Pos(int x, int y)//���ù��λ��
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);//���ر�׼�����롢����������豸�ľ����Ҳ���ǻ�����롢���/�������Ļ�������ľ��
    SetConsoleCursorPosition(hOutput, pos);
}

static void creatMap()//������ͼ
{
    int i;
    for (i = 0; i<58; i += 2)//��ӡ���±߿�
    {
        Pos(i, 0);
        printf("��");//һ������ռ����λ��
        Pos(i, 26);
        printf("��");
    }
    for (i = 1; i<26; i++)//��ӡ���ұ߿�
    {
        Pos(0, i);
        printf("��");
        Pos(56, i);
        printf("��");
    }
}

static void initSnake()//��ʼ������
{
    snake *tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//����β��ʼ��ͷ�巨����x,y�趨��ʼ��λ��//
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)//��ʼ����Ϊ4
    {
        ctx.head = (snake*)malloc(sizeof(snake));
        ctx.head->next = tail;
        ctx.head->x = 24 + 2 * i;
        ctx.head->y = 5;
        tail = ctx.head;
    }
    while (tail != NULL)//��ͷ��Ϊ���������
    {
        Pos(tail->x, tail->y);
        printf("��");
        tail = tail->next;
    }
}
//??
static int biteSelf()//�ж��Ƿ�ҧ�����Լ�
{
    snake *self;
    self = ctx.head->next;
    while (self != NULL)
    {
        if (self->x == ctx.head->x && self->y == ctx.head->y)
        {
            return 1;
        } 
        self = self->next;
    }
    return 0;
}

static void createFood()//�������ʳ��
{
    snake *food_1;
    srand((unsigned)time(NULL));//Ϊ�˷�ֹÿ�β������������ͬ����������Ϊtime
    food_1 = (snake*)malloc(sizeof(snake));
    while ((food_1->x % 2) != 0)    //��֤��Ϊż����ʹ��ʳ��������ͷ����
    {
        food_1->x = rand() % 52 + 2;
    }
    food_1->y = rand() % 24 + 1;
    ctx.q = ctx.head;
    while (ctx.q->next == NULL)
    {
        if (ctx.q->x == food_1->x && ctx.q->y == food_1->y) //�ж������Ƿ���ʳ���غ�
        {
            free(food_1);
            createFood();
        }
        ctx.q = ctx.q->next;
    }
    Pos(food_1->x, food_1->y);
    ctx.food = food_1;
    printf("��");
}

static int cantCrossWall()//���ܴ�ǽ
{
    if (ctx.head->x == 0 || ctx.head->x == 56 || ctx.head->y == 0 || ctx.head->y == 26)
    {
        ctx.end_status = 1;
        //return endGame();
        return -1;
    }
    return 0;
}

static int snakeMove()//��ǰ��,��U,��D,��L,��R
{
    int res;
    snake * nexthead;
    res = cantCrossWall();
    if(res != 0)
        return res;

    nexthead = (snake*)malloc(sizeof(snake));
    if (ctx.status == U)
    {
        nexthead->x = ctx.head->x;
        nexthead->y = ctx.head->y - 1;
        if (nexthead->x == ctx.food->x && nexthead->y == ctx.food->y)//�����һ����ʳ��//
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("��");
                ctx.q = ctx.q->next;
            }
            ctx.score = ctx.score + ctx.add;
            createFood();
        }
        else                                               //���û��ʳ��//
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q->next->next != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("��");
                ctx.q = ctx.q->next;
            }
            Pos(ctx.q->next->x, ctx.q->next->y);
            printf("  ");
            free(ctx.q->next);
            ctx.q->next = NULL;
        }
    }
    if (ctx.status == D)
    {
        nexthead->x = ctx.head->x;
        nexthead->y = ctx.head->y + 1;
        if (nexthead->x == ctx.food->x && nexthead->y == ctx.food->y)  //��ʳ��
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("��");
                ctx.q = ctx.q->next;
            }
            ctx.score = ctx.score + ctx.add;
            createFood();
        }
        else                               //û��ʳ��
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q->next->next != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("��");
                ctx.q = ctx.q->next;
            }
            Pos(ctx.q->next->x, ctx.q->next->y);
            printf("  ");
            free(ctx.q->next);
            ctx.q->next = NULL;
        }
    }
    if (ctx.status == L)
    {
        nexthead->x = ctx.head->x - 2;
        nexthead->y = ctx.head->y;
        if (nexthead->x == ctx.food->x && nexthead->y == ctx.food->y)//��ʳ��
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("��");
                ctx.q = ctx.q->next;
            }
            ctx.score = ctx.score + ctx.add;
            createFood();
        }
        else                                //û��ʳ��
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q->next->next != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("��");
                ctx.q = ctx.q->next;
            }
            Pos(ctx.q->next->x, ctx.q->next->y);
            printf("  ");
            free(ctx.q->next);
            ctx.q->next = NULL;
        }
    }
    if (ctx.status == R)
    {
        nexthead->x = ctx.head->x + 2;
        nexthead->y = ctx.head->y;
        if (nexthead->x == ctx.food->x && nexthead->y == ctx.food->y)//��ʳ��
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("��");
                ctx.q = ctx.q->next;
            }
            ctx.score = ctx.score + ctx.add;
            createFood();
        }
        else                                         //û��ʳ��
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q->next->next != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("��");
                ctx.q = ctx.q->next;
            }
            Pos(ctx.q->next->x, ctx.q->next->y);
            printf("  ");
            free(ctx.q->next);
            ctx.q->next = NULL;
        }
    }
    if (biteSelf() == 1)       //�ж��Ƿ��ҧ���Լ�
    {
        ctx.end_status = 2;
        //endGame();
        return -1;
    }
    return 0;
}

static void pause()//��ͣ
{
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            break;
        }

    }
}

static void readKey(void)
{
    if (GetAsyncKeyState( 0x1B )& 0x8000)
    {
        ctx.status = 0x1B;
    }
    else if (GetAsyncKeyState(VK_UP) && ctx.status != D)
    {
        ctx.status = U;
    }
    else if (GetAsyncKeyState(VK_DOWN) && ctx.status != U)
    {
        ctx.status = D;
    }
    else if (GetAsyncKeyState(VK_LEFT) && ctx.status != R)
    {
        ctx.status = L;
    }
    else if (GetAsyncKeyState(VK_RIGHT) && ctx.status != L)
    {
        ctx.status = R;
    }
    else if (GetAsyncKeyState(VK_SPACE))
    {
        pause();
    }
    else if (GetAsyncKeyState(VK_ESCAPE))
    {
        ctx.end_status = 3;
        ctx.status = 0x1B;
    }
    else if (GetAsyncKeyState(VK_F1))
    {
        if (ctx.movetime >= 50)
        {
            ctx.movetime = ctx.movetime - 30;
            ctx.add = ctx.add + 2;
            if (ctx.movetime == 320)
            {
                ctx.add = 2;//��ֹ����1֮���ټӻ����д�
            }
            Sleep(500);
        }
        
    }
    else if (GetAsyncKeyState(VK_F2))
    {
        if (ctx.movetime<350)
        {
            ctx.movetime = ctx.movetime + 30;
            ctx.add = ctx.add - 2;
            if (ctx.movetime == 350)
            {
                ctx.add = 1;  //��֤��ͷ�Ϊ1
            }
            Sleep(500);
        }
    }
}


static int runGame()//������Ϸ        
{
    int res;
    int time = 0;
    Pos(64, 15);
    printf("���ܴ�ǽ������ҧ���Լ�\n");
    Pos(64, 16);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ�.");
    Pos(64, 17);
    printf("F1 Ϊ���٣�F2 Ϊ����\n");
    Pos(64, 18);
    printf("ESC ���˳���Ϸ.space����ͣ��Ϸ.");
    Pos(64, 20);
    printf("C�����о����� www.clang.cc");
    ctx.status = R;
    while (1)
    {
        Pos(64, 10);
        printf("�÷֣�%d  ", ctx.score);
        Pos(64, 11);
        printf("ÿ��ʳ��÷֣�%d��", ctx.add);

        readKey();
        if(ctx.status == 0x1B)
            return -1;
        Sleep(10);
        time += 10;
        if(time >= ctx.movetime)
        {
            time = 0;
            res = snakeMove();
            if(res != 0)
                return 0;
        }
            
        
    }
    return 0;
}

static void initGame()//��ʼ����
{
    Pos(40, 12);

    system("title C�����о�����   www.clang.cc");
    printf("��ӭ����̰ʳ����Ϸ��");
    Pos(40, 25);
    printf("              C�����о�����  www.clang.cc.\n");
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ��� F1 Ϊ���٣�2 Ϊ����\n");
    Pos(25, 13);
    printf("���ٽ��ܵõ����ߵķ�����\n");
    system("pause");
    system("cls");
}

static int endGame()//������Ϸ
{
    system("cls");
    Pos(24, 12);
    if (ctx.end_status == 1)
    {
        printf("�Բ�����ײ��ǽ�ˡ���Ϸ����.");
    }
    else if (ctx.end_status == 2)
    {
        printf("�Բ�����ҧ���Լ��ˡ���Ϸ����.");
    }
    else if (ctx.end_status == 3)
    {
        printf("�����Ѿ���������Ϸ��");
    }
    Pos(24, 13);
    printf("���ĵ÷���%d\n", ctx.score);
	system("pause");
    //printf("����һ��?[y]");
    //if (getchar() != 'y')
    //    return -1;
    return 0;
}

static void gameStart()//��Ϸ��ʼ��
{
    system("mode con cols=100 lines=30");
    initGame();
    creatMap();
    initSnake();
    createFood();
}

int snake_main(int argc,char **argv)
{
    int res;
    while(1)
    {
        param_init();
        gameStart();
        res = runGame();
        if(res != 0)
            break;
        endGame();
    }
    system("mode con cols=100 lines=50");//���ô��ڴ�С
    return 0;
}
