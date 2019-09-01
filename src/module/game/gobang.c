#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <malloc.h>
#include <windows.h>

struct rcd;//�����ڵ�ṹ
typedef struct rcd* Record;//�ڵ�ָ�����
typedef struct rcd record;//�ڵ����

#define MAXIMUS 15 //�������̴�С

static int p[MAXIMUS][MAXIMUS];//�洢�Ծ���Ϣ
static char buff[MAXIMUS*2+1][MAXIMUS*4+3];//���������
static int Cx,Cy;//��ǰ���λ��
static int Now;//��ǰ���ӵ���ң�1����ڣ�2�����
static int wl,wp;//��ǰд�뻺����������������λ��
static char* showText;//������������ʾ��������Ϣ
static int count;//�غ���
static int Putable;//ָʾ��ǰ�Ƿ��������
static int Exiting;//1Ϊ���������Ӳ���ESCʱѯ���Ƿ��˳������״̬��2Ϊ�Ǵ�״̬
static int ExiRep;//1Ϊ���طŵ����һ�غϲ������ʱѯ���Ƿ��˳��طŵ�״̬��2Ϊ�Ǵ�״̬
static Record RecBeg,RecNow;//��¼�Ŀ�ʼ�ڵ�͵�ǰ�ڵ�

struct rcd//��¼�ڵ�ṹ,˫������ʽ
{
    int X;//�˼�¼�����X����
    int Y;//�˼�¼�����Y����
    Record Next;//ǰһ����¼
    Record Back;//��һ����¼
};

static Record newRecord()//��¼�ڵ㹹�캯��
{
    Record r=(Record)malloc(sizeof(record));//����һ���ڵ����
    r->Next=NULL;//����ǰ��ڵ��ֵNULL
    r->Back=NULL;
    return r;
}

static void Exit()//����˳�����
{
    //int input;
    if(Exiting)//����ǵڶ��ΰ���ESC
    {
        exit(0);
    }
    else//����ǵ�һ�ΰ���ESC��ѯ���Ƿ��˳�����
    {
        showText="�Ƿ��˳����ٴΰ���ESC�˳�������������";
        Exiting=1;//ָʾ�Ѿ����¹�ESC
    }
}

static void ExitRep()//����˳��ط�
{
    //int input;
    if(ExiRep)//����ǵڶ��κ���
    {
        ExiRep=3;
    }
    else//����ǵ�һ�κ�����ѯ���Ƿ��˳��ط�
    {
        showText="�Ƿ��˳����ٴκ����˳��طţ�����������";
        ExiRep=1;//ָʾ�Ѿ����¹�����
    }
}

static void AddRecord()//��Ӽ�¼
{
    RecNow->X=Cx;//��¼����
    RecNow->Y=Cy;
    RecNow->Next=newRecord();//������һ����¼�ڵ�
    RecNow->Next->Back=RecNow;//���˫����
    RecNow=RecNow->Next;//��ǰ��¼������һ����¼�ڵ�
}

static int DelRecord()//ɾ����ǰ��¼�ڵ㣬1Ϊɾ���ɹ���0Ϊɾ��ʧ��
{
    Record b;//��һ���ڵ�
    if(RecNow->Back!=NULL)//Խ����
    {
        b=RecNow->Back;//������һ���ڵ�
        free(RecNow);//�ͷŵ�ǰ�ڵ�
        RecNow=b;//��ǰ��¼������һ����¼�ڵ�
        return 1;
    }
    else
    {
        return 0;//û�нڵ��ɾ��ʱ
    }
}

static void CleanRecord()//�������м�¼
{
    Record n;//��һ���ڵ�
    while(RecBeg->Next!=NULL)//ɾ�����м�¼��ֱ��Խ��ǰΪֹ
    {
        n=RecBeg->Next;//������һ���ڵ�
        free(RecBeg);//�ͷŵ�ǰ�ڵ�
        RecBeg=n;//��ǰ��¼������һ����¼�ڵ�
    }
}

static char* Copy(char* strDest,const char* strSrc)//�޸Ĺ����ַ������ƺ����������ĩ�˵�\0
{
    char* strDestCopy = strDest;
    while (*strSrc!='\0')
    {
        *strDest++=*strSrc++;
    }
    return strDestCopy;
}

static void Initialize()//��ʼ��һ���Ծֺ���
{
    int i,j;//ѭ������
    system("title �Ծ���(����������ƹ�꣬�س����ӣ��ո���)��Esc�˳�");
    showText="";//������ʾ��Ϣ
    count=0;//�غ�������
    RecNow=RecBeg=newRecord();
    Exiting=0;
    for(i=0;i<MAXIMUS;i++)//���öԾ�����
    {
        for(j=0;j<MAXIMUS;j++)
        {
            p[i][j]=0;
        }
    }
    Cx=Cy=MAXIMUS/2;//���ù�굽����
    Now=1;//���õ�ǰΪ�ڷ�
}

static char* getStyle(int i,int j)//���������ָ�����꽻��λ�õ��ַ���ͨ���Ʊ��ƴ������
{
    if(p[i][j]==1)//1Ϊ����
        return "��";
    else if(p[i][j]==2)//2Ϊ����
        return "��";
    else if(i==0&&j==0)//����Ϊ��Ե������ʽ
        return "��";
    else if(i==MAXIMUS-1&&j==0)
        return "��";
    else if(i==MAXIMUS-1&&j==MAXIMUS-1)
        return "��";
    else if(i==0&&j==MAXIMUS-1)
        return "��";
    else if(i==0)
        return "��";
    else if(i==MAXIMUS-1)
        return "��";
    else if(j==0)
        return "��";
    else if(j==MAXIMUS-1)
        return "��";
    return "��";//�м�Ŀ�λ
}

static char* getCurse(int i,int j){//���ָ�����꽻��λ�����ϸ����ʽ��ͨ���Ʊ����ģ�������ʾ
    if(Putable)//������ʱ���Ϊ����
    {
        if(i==Cx){
            if(j==Cy)
                return "��";
            else if (j==Cy+1)
                return "��";
        }
        else if(i==Cx+1)
        {
            if(j==Cy)
                return "��";
            else if (j==Cy+1)
                return "��";
        }
    }
    else//��������ʱ���Ϊ����
    {
        if(i==Cx){
            if(j==Cy)
                return "��";
            else if (j==Cy+1)
                return "��";
        }
        else if(i==Cx+1)
        {
            if(j==Cy)
                return "��";
            else if (j==Cy+1)
                return "��";
        }
    }
    return "��";//������ڹ�긽����Ϊ��
}

static void write(char* c)//�򻺳���д���ַ���
{
    Copy(buff[wl]+wp,c);
    wp+=strlen(c);
}

static void ln()//������д��λ������
{
    wl+=1;
    wp=0;
}

static void Display()//�������������������Ļ
{
    int i,l=strlen(showText);//ѭ���������м�������Ϣ�ĳ���
    int Offset=MAXIMUS*2+2-l/2;//����м�������Ϣ������ʾ���ڵĺ�����λ��
    if(Offset%2==1)//���λ��Ϊ���������ƶ���ż�����������
    {
        Offset--;
    }
    Copy(buff[MAXIMUS]+Offset,showText);//���м�������Ϣ���Ƶ�������
    if(l%2==1)//����м����ֳ���Ϊ������������Ͽո񣬱������
    {
        *(buff[MAXIMUS]+Offset+l)=0x20;
    }
    system("cls");//������Ļ��׼��д��
    for(i=0;i<MAXIMUS*2+1;i++){//ѭ��д��ÿһ��
        printf("%s",buff[i]);
        if(i<MAXIMUS*2)//д����ÿһ����Ҫ����
            printf("\n");
    }
}

static void Print()//������������������浽��������Ȼ�����Display������ʾ����
{
    int i,j;//ѭ������
    wl=0;
    wp=0;
    for(j=0;j<=MAXIMUS;j++)//д����������Ͻǵ��ַ�����Ϊ��Ҫ��ӡ�������½ǣ����Ժ��Ժ��ݸ���һ��ѭ��
    {
        for(i=0;i<=MAXIMUS;i++)
        {
            write(getCurse(i,j));//д�����Ͻ��ַ�
            if(j==0||j==MAXIMUS)//������������̱�Ե��û�����ӵ����ߣ��ÿո����λ��
            {
                if(i!=MAXIMUS)
                    write("��");
            }
            else//����������м��������߳н�����
            {
                if(i==0||i==MAXIMUS-1)//���ұ�Ե�����߸���
                    write("��");
                else if(i!=MAXIMUS)//�м������
                    write("��");
            }
        }
        if(j==MAXIMUS)//��������һ��ѭ������ֻ��Ҫ����߲��ַ�������Ҫ��һ��
        {
            break;
        }
        ln();//���п�ʼ��ӡ��������
        write("��");//�ÿ�λ����λ��
        for(i=0;i<MAXIMUS;i++)//��������ѭ�������Ĵ���
        {
            write(getStyle(i,j));//д�뽻���ַ�
            if(i!=MAXIMUS-1)//����������Ҳ��򲹳�һ�����߳н�����
            {
                if(j==0||j==MAXIMUS-1)
                {
                    write("��");//���±�Ե�ĺ��߸���
                }
                else
                {
                    write("��");//�м�ĺ���
                }
            }
        }
        ln();//д��һ�к�����
    }
    Display();//�������������������Ļ
}

static int Put(){//�ڵ�ǰ���λ�����ӣ�����ǿգ��򷵻�0��ʾʧ��
    if(Putable)
    {
        p[Cx][Cy]=Now;//�ı��λ������
        AddRecord();
        return 1;//����1��ʾ�ɹ�
    }
    else
    {
        return 0;
    }
}

static int Check()//ʤ����飬���жϵ�ǰ����λ����û���������������
{
    int w=1,x=1,y=1,z=1,i;//�ۼƺ�����б��а�ĸ������������ͬ������Ŀ
    for(i=1;i<5;i++)if(Cy+i<MAXIMUS&&p[Cx][Cy+i]==Now)w++;else break;//���¼��
    for(i=1;i<5;i++)if(Cy-i>0&&p[Cx][Cy-i]==Now)w++;else break;//���ϼ��
    if(w>=5)return Now;//�����ﵽ5�����жϵ�ǰ�������ΪӮ��
    for(i=1;i<5;i++)if(Cx+i<MAXIMUS&&p[Cx+i][Cy]==Now)x++;else break;//���Ҽ��
    for(i=1;i<5;i++)if(Cx-i>0&&p[Cx-i][Cy]==Now)x++;else break;//������
    if(x>=5)return Now;//�����ﵽ5�����жϵ�ǰ�������ΪӮ��
    for(i=1;i<5;i++)if(Cx+i<MAXIMUS&&Cy+i<MAXIMUS&&p[Cx+i][Cy+i]==Now)y++;else break;//�����¼��
    for(i=1;i<5;i++)if(Cx-i>0&&Cy-i>0&&p[Cx-i][Cy-i]==Now)y++;else break;//�����ϼ��
    if(y>=5)return Now;//�����ﵽ5�����жϵ�ǰ�������ΪӮ��
    for(i=1;i<5;i++)if(Cx+i<MAXIMUS&&Cy-i>0&&p[Cx+i][Cy-i]==Now)z++;else break;//�����ϼ��
    for(i=1;i<5;i++)if(Cx-i>0&&Cy+i<MAXIMUS&&p[Cx-i][Cy+i]==Now)z++;else break;//�����¼��
    if(z>=5)return Now;//�����ﵽ5�����жϵ�ǰ�������ΪӮ��
    return 0;//��û�м�鵽�����飬�򷵻�0��ʾ��û����Ҵ��ʤ��
}
static void ReplayMode(){
    int i,j;//ѭ������
    system("title �ط���(��������ˣ��Ҽ���ո�ǰ��)��Esc�˳�");
    showText="";//������ʾ��Ϣ
    count=0;//�غ�������
    Putable=0;//��������״̬
    RecBeg->Back=newRecord();
    RecBeg->Back->Next=RecBeg;
    RecBeg=RecBeg->Back;
    for(i=0;i<MAXIMUS;i++)//���öԾ�����
    {
        for(j=0;j<MAXIMUS;j++)
        {
            p[i][j]=0;
        }
    }
    Now=1;//���õ�ǰΪ�ڷ�
}

static void RepForward()//�ط�ģʽǰ��
{
    if(RecNow->Next->Next!=NULL)//Խ����
    {
        RecNow=RecNow->Next;//��ǰ�ڵ�������һ����¼�ڵ�
        p[RecNow->X][RecNow->Y]=Now;//���ռ�¼��ԭһ���غ�
        Cx=RecNow->X;//���ù��λ��
        Cy=RecNow->Y;
        Now=3-Now;//ת����ǰ�ĺڰ׷�
    }
    else//���Ѵﵽ�����ѯ���˳�
    {
        ExitRep();
    }
}

static void RepBackward()//�ط�ģʽ����
{
    if(RecNow->Back!=NULL)//Խ����
    {
        p[RecNow->X][RecNow->Y]=0;//���ռ�¼����һ���غ�
        if(RecNow->Back->Back==NULL)//����������û������ʱ���ع��
        {
            Cx=-2;
            Cy=-2;
        }
        else if(RecNow->Back==NULL)//��ֻ��һ������ʱ�ƶ���굽������ӵ�λ��
        {
            Cx=RecNow->X;
            Cy=RecNow->Y;
        }
        else//����������ƶ���굽��һ�غϵ�λ��
        {
            Cx=RecNow->Back->X;
            Cy=RecNow->Back->Y;
        }
        RecNow=RecNow->Back;//��ǰ�ڵ��������һ����¼�ڵ�
        Now=3-Now;//ת����ǰ�ĺڰ׷�
        
    }
}

static void ShowReplay()
{
    int input;//�������
    ReplayMode();//��ʼ���ط�ģʽ
    RecNow=RecBeg;//��ǰ�۲��ͷ��ʼ
    RepForward();//��ʾ��һ������
    while(1)//��ʼ���޻غϵ���ѭ����ֱ��Esc�˳�
    {
        if(ExiRep==3)
        {
            ExiRep=0;
            break;
        }
        Print();//��ӡ����
        input=_getch();//�ȴ����̰���һ���ַ�
        if(input==27)//�����ESC���˳��ط�
        {
            return;
        }
        else if(input==0x20)//����ǿո���ǰ��
        {
            RepForward();
            continue;
        }
        else if(input==0xE0)//������µ��Ƿ������������������룬��һ��Ϊ0xE0��ʾ���µ��ǿ��Ƽ�
        {
            input=_getch();//��õڶ���������Ϣ
            switch(input)//�жϷ���������ƶ����λ��
            {
            case 0x4B:
                RepBackward();//�������
                break;
            case 0x4D:
                RepForward();//����ǰ��
                continue;
            }
        }
        ExiRep=0;//δ�ٴΰ�������׼���˳�
        showText="";
    }
}


static int Regret()//���峷��,���������û���Ӽ�Ϊ�˳�
{
    if(DelRecord()){//����ɾ����ǰ�ڵ㣬����нڵ����ɾ����
        p[RecNow->X][RecNow->Y]=0;//������ǰ�غ�
        if(RecNow->Back==NULL)//���ɾ�����ǵ�һ�����򽫹���ƶ�����һ����ԭ����λ����
        {
            Cx=RecNow->X;
            Cy=RecNow->Y;
        }
        else//���򽫹���ƶ�����һ������
        {
            Cx=RecNow->Back->X;
            Cy=RecNow->Back->Y;
        }
        Now=3-Now;//��ת��ǰ�ڰ׷�
        return 0;
    }
    else
    {
        //Exit();//���û�����ӿ��Գ�������ѯ���˳�
        return -1;
    }
}

static int RunGame()//���������Ծ֣�����Ӯ����Ϣ(��Ȼ������)
{
    int input;//�������
    int victor;//Ӯ����Ϣ
    Initialize();//��ʼ���Ծ�
    while(1){//��ʼ���޻غϵ���ѭ����ֱ������ʤ������
        Putable=p[Cx][Cy]==0;
        Print();//��ӡ����
        input=_getch();//�ȴ����̰���һ���ַ�
        if(input==0x1B) //�����ESC���˳�
            return 0;
        if(input==' ')//����ǿո������
        {
            Regret();
            Print();
            continue;
        }
        else if(input=='\r')//����ǻس���ʼ����
        {
            if(Put())//������ӳɹ����ж�ʤ��
            {
                victor=Check();
                Now=3-Now;//�ֻ���ǰ�������
                count++;
                if(victor==1)//����ڷ��ﵽʤ������ʾ��ʾ���ֲ��ȴ�һ�ΰ���������ʤ����Ϣ
                {
                    showText="�ڷ�ʤ��!��R�鿴�ط�,�����������¿���";
                    Print();
                    input=_getch();
                    if(input==0xE0)
                    {
                        _getch();
                    }
                    else if(input=='R'||input=='r')
                    {
                        ShowReplay();
                    }
                    return Now;
                }
                else if(victor==2)//����׷��ﵽʤ������ʾ��ʾ���ֲ��ȴ�һ�ΰ���������ʤ����Ϣ
                {
                    showText="�׷�ʤ��!��R�鿴�ط�,�����������¿���";
                    Print();
                    input=_getch();
                    if(input==0xE0)
                    {
                        _getch();
                    }
                    else if(input=='R'||input=='r')
                    {
                        ShowReplay();
                    }
                    return Now;
                }else if(count==MAXIMUS*MAXIMUS)//����غ����ﵽ�����������������̳�������Ϊƽ��
                {
                    showText="ƽ��!��R�鿴�ط�,�����������¿���";
                    Print();
                    input=_getch();
                    if(input==0xE0)
                    {
                        _getch();
                    }
                    else if(input=='R'||input=='r')
                    {
                        ShowReplay();
                    }
                    CleanRecord();
                    return 0;
                }
            }
        }
        else if(input==0xE0)//������µ��Ƿ������������������룬��һ��Ϊ0xE0��ʾ���µ��ǿ��Ƽ�
        {
            input=_getch();//��õڶ���������Ϣ
            switch(input)//�жϷ���������ƶ����λ��
            {
            case 0x4B://
                Cx--;
                break;
            case 0x48:
                Cy--;
                break;
            case 0x4D:
                Cx++;
                break;
            case 0x50:
                Cy++;
                break;
            }
            if(Cx<0)Cx=MAXIMUS-1;//������λ��Խ�����ƶ����Բ�
            if(Cy<0)Cy=MAXIMUS-1;
            if(Cx>MAXIMUS-1)Cx=0;
            if(Cy>MAXIMUS-1)Cy=0;
        }
        Exiting=0;//δ�ٴΰ���ESC��׼���˳�
        showText="";
    }
}

int gobang_main(int argc,char **argv)
{
    int res;
	HANDLE hConsoleOutput;
    CONSOLE_SCREEN_BUFFER_INFO info;
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // ��ȡ����̨������
    GetConsoleScreenBufferInfo(hConsoleOutput, &info);

    system("mode con cols=63 lines=32");//���ô��ڴ�С
    system("color E8");//������ɫ
    while(1){//ѭ��ִ����Ϸ
        res = RunGame();
        if(res == 0)
            break;
    }
    SetConsoleTextAttribute(hConsoleOutput, info.wAttributes);
    system("cls"); // ����
    system("mode con cols=100 lines=50");//���ô��ڴ�С
	fflush(stdin);
    return 0;
}
