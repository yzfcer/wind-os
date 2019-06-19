#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
 
#ifdef _MSC_VER // M$�ı�����Ҫ���������չ�
  #if _MSC_VER <= 1200 // VC6�����°汾
    #error ���ǲ��ǻ�����VC6����
  #else // VC6���ϰ汾
    #if _MSC_VER >= 1600 // ��˵VC10�����ϰ汾��stdint.h��
      #include <stdint.h>
    #else // VC10���°汾���Լ�����int8_t��uint16_t
      typedef signed char int8_t;
      typedef unsigned short uint16_t;
    #endif
    #ifndef __cplusplus // ��˵VC��û��stdbool.h������C++���룬�Լ�����bool
      typedef int bool;
      #define true 1
      #define false 0
    #endif
  #endif
#else // �����ı���������˵
  #include <stdint.h>
  #ifndef __cplusplus // ����C++���룬��Ҫstdbool.h���bool
    #include <stdbool.h>
  #endif
#endif
 
// =============================================================================
// 7�ַ����4��ת״̬��4λΪһ�У�
static const uint16_t gs_uTetrisTable[7][4] =
{
  { 0x00F0U, 0x2222U, 0x00F0U, 0x2222U }, // I��
  { 0x0072U, 0x0262U, 0x0270U, 0x0232U }, // T��
  { 0x0223U, 0x0074U, 0x0622U, 0x0170U }, // L��
  { 0x0226U, 0x0470U, 0x0322U, 0x0071U }, // J��
  { 0x0063U, 0x0264U, 0x0063U, 0x0264U }, // Z��
  { 0x006CU, 0x0462U, 0x006CU, 0x0462U }, // S��
  { 0x0660U, 0x0660U, 0x0660U, 0x0660U }  // O��
};
 
// =============================================================================
// ��ʼ״̬����Ϸ��
// ÿ��Ԫ�ر�ʾ��Ϸ�ص�һ�У��±�������Ϸ�صײ�
// ���˸���2��1���ײ�2ȫ��Ϊ1�����ڽ�����ײ���
// ����һ����Ϸ�صĿ��Ϊ12��
// �����Ҫ��ͳ��10�У�ֻ���������1���ɣ�0xE007������Ȼ��ʾ��ز���ҲҪ��֮�Ķ�
// ��ĳ��Ԫ��Ϊ0xFFFFUʱ��˵�������ѱ�����
// ����4�����ڸ����飬����ʾ����
// �ٳ�ȥ�ײ�2�У���ʾ��������Ϸ�ظ߶�Ϊ22��
static const uint16_t gs_uInitialTetrisPool[28] =
{
  0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U,
  0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U,
  0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U,
  0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xFFFFU, 0xFFFFU
};
 
#define COL_BEGIN 2
#define COL_END 14
#define ROW_BEGIN 4
#define ROW_END 26
 
// =============================================================================
typedef struct TetrisManager // ����ṹ��洢��Ϸ�������
{
  uint16_t pool[28]; // ��Ϸ��
  int8_t x; // ��ǰ����x���꣬�˴�����Ϊ�������Ͻ�����
  int8_t y; // ��ǰ����y����
  int8_t type[3]; // ��ǰ����һ��������һ����������
  int8_t orientation[3]; // ��ǰ����һ��������һ��������ת״̬
  unsigned score; // �÷�
  unsigned erasedCount[4]; // ������
  unsigned erasedTotal; // ��������
  unsigned tetrisCount[7]; // ��������
  unsigned tetrisTotal; // ��������
  bool dead; // ��
} TetrisManager;
 
// =============================================================================
typedef struct TetrisControl // ����ṹ��洢�����������
{
  bool pause; // ��ͣ
  bool clockwise; // ��ת����˳ʱ��Ϊtrue
  int8_t direction; // �ƶ�����0�����ƶ� 1�����ƶ�
  // ��Ϸ����ÿ�����ɫ
  // ���ڴ˰汾�ǲ�ɫ�ģ�������Ϸ�������޷��洢��ɫ��Ϣ
  // ��Ȼ�����ֻʵ�ֵ�ɫ��ģ���û��Ҫ�����������
  int8_t color[28][16];
} TetrisControl;
 
HANDLE g_hConsoleOutput; // ����̨������
 
// =============================================================================
// ��������
// ���ʹ��ȫ�ֱ�����ʽʵ�֣���û��Ҫ������
void initGame(TetrisManager *manager, TetrisControl *control); // ��ʼ����Ϸ
void restartGame(TetrisManager *manager, TetrisControl *control); // ���¿�ʼ��Ϸ
void giveTetris(TetrisManager *manager); // ��һ������
bool checkCollision(const TetrisManager *manager); // ��ײ���
void insertTetris(TetrisManager *manager); // ���뷽��
void removeTetris(TetrisManager *manager); // �Ƴ�����
void horzMoveTetris(TetrisManager *manager, TetrisControl *control); // ˮƽ�ƶ�����
void moveDownTetris(TetrisManager *manager, TetrisControl *control); // �����ƶ�����
void rotateTetris(TetrisManager *manager, TetrisControl *control); // ��ת����
void dropDownTetris(TetrisManager *manager, TetrisControl *control); // ����ֱ�����
bool checkErasing(TetrisManager *manager, TetrisControl *control); // ���м��
void keydownControl(TetrisManager *manager, TetrisControl *control, int key); // ������
void setPoolColor(const TetrisManager *manager, TetrisControl *control); // ������ɫ
void gotoxyWithFullwidth(short x, short y); // ��ȫ�Ƕ�λ
void printPoolBorder(); // ��ʾ��Ϸ�ر߽�
void printTetrisPool(const TetrisManager *manager, const TetrisControl *control); // ��ʾ��Ϸ��
void printCurrentTetris(const TetrisManager *manager, const TetrisControl *control); // ��ʾ��ǰ����
void printNextTetris(const TetrisManager *manager); // ��ʾ��һ��������һ������
void printScore(const TetrisManager *manager); // ��ʾ�÷���Ϣ
void runGame(TetrisManager *manager, TetrisControl *control); // ������Ϸ
void printPrompting(); // ��ʾ��ʾ��Ϣ
bool ifPlayAgain(); // ����һ��
 
// =============================================================================

// =============================================================================
// ��ʼ����Ϸ
void initGame(TetrisManager *manager, TetrisControl *control)
{
  CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE }; // �����Ϣ
 
  g_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // ��ȡ����̨������
  SetConsoleCursorInfo(g_hConsoleOutput, &cursorInfo); // ���ù������
  SetConsoleTitleA("����˹����");
 
  restartGame(manager, control);
}
 
// =============================================================================
// ���¿�ʼ��Ϸ
void restartGame(TetrisManager *manager, TetrisControl *control)
{
  memset(manager, 0, sizeof(TetrisManager)); // ȫ����0
 
  // ��ʼ����Ϸ��
  memcpy(manager->pool, gs_uInitialTetrisPool, sizeof(uint16_t [28]));
  srand((unsigned)time(NULL)); // �����������
 
  manager->type[1] = rand() % 7; // ��һ��
  manager->orientation[1] = rand() & 3;
 
  manager->type[2] = rand() % 7; // ����һ��
  manager->orientation[2] = rand() & 3;
 
  memset(control, 0, sizeof(TetrisControl)); // ȫ����0
 
  giveTetris(manager); // ����һ������
  setPoolColor(manager, control); // ������ɫ
}
 
// =============================================================================
// ��һ������
void giveTetris(TetrisManager *manager)
{
  uint16_t tetris;
 
  manager->type[0] = manager->type[1]; // ��һ��������Ϊ��ǰ
  manager->orientation[0] = manager->orientation[1];
 
  manager->type[1] = manager->type[2];// ����һ���÷���Ϊ��һ��
  manager->orientation[1] = manager->orientation[2];
 
  manager->type[2] = rand() % 7;// �����������һ������
  manager->orientation[2] = rand() & 3;
 
  tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]]; // ��ǰ����
 
  // ���õ�ǰ����y���꣬��֤�ո���ʱֻ��ʾ����������һ��
  // ����ʵ��ʹ����ҿ����Ժܿ���ٶȽ��������ڲ���ʾ�����Ķ���4����
  if (tetris & 0xF000)
  {
    manager->y = 0;
  }
  else
  {
    manager->y = (tetris & 0xFF00) ? 1 : 2;
  }
  manager->x = 6; // ���õ�ǰ����x����
 
  if (checkCollision(manager)) // ��⵽��ײ
  {
    manager->dead = true; // �����Ϸ����
  }
  else // δ��⵽��ײ
  {
    insertTetris(manager); // ����ǰ���������Ϸ��
  }
 
  ++manager->tetrisTotal; // ��������
  ++manager->tetrisCount[manager->type[0]]; // ��Ӧ������
 
  printNextTetris(manager); // ��ʾ��һ������
  printScore(manager); // ��ʾ�÷���Ϣ
}
 
// =============================================================================
// ��ײ���
bool checkCollision(const TetrisManager *manager)
{
  // ��ǰ����
  uint16_t tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]];
  uint16_t dest = 0;
 
  // ��ȡ��ǰ��������Ϸ���е�����
  // ��Ϸ������x y��С������Ϣ�����͵��ߴ����16λ�޷�������
  dest |= (((manager->pool[manager->y + 0] >> manager->x) << 0x0) & 0x000F);
  dest |= (((manager->pool[manager->y + 1] >> manager->x) << 0x4) & 0x00F0);
  dest |= (((manager->pool[manager->y + 2] >> manager->x) << 0x8) & 0x0F00);
  dest |= (((manager->pool[manager->y + 3] >> manager->x) << 0xC) & 0xF000);
 
  // ����ǰ������Ŀ����������ص�����ײ������λ��Ľ����Ϊ0
  return ((dest & tetris) != 0);
}
 
// =============================================================================
// ���뷽��
void insertTetris(TetrisManager *manager)
{
  // ��ǰ����
  uint16_t tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]];
 
  // ��ǰ����ÿ4λȡ����λ����Ϸ����Ӧλ�ã�����ɲ��뷽��
  manager->pool[manager->y + 0] |= (((tetris >> 0x0) & 0x000F) << manager->x);
  manager->pool[manager->y + 1] |= (((tetris >> 0x4) & 0x000F) << manager->x);
  manager->pool[manager->y + 2] |= (((tetris >> 0x8) & 0x000F) << manager->x);
  manager->pool[manager->y + 3] |= (((tetris >> 0xC) & 0x000F) << manager->x);
}
 
// =============================================================================
// �Ƴ�����
void removeTetris(TetrisManager *manager)
{
  // ��ǰ����
  uint16_t tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]];
 
  // ��ǰ����ÿ4λȡ������λȡ����λ�뵽��Ϸ����Ӧλ�ã�������Ƴ�����
  manager->pool[manager->y + 0] &= ~(((tetris >> 0x0) & 0x000F) << manager->x);
  manager->pool[manager->y + 1] &= ~(((tetris >> 0x4) & 0x000F) << manager->x);
  manager->pool[manager->y + 2] &= ~(((tetris >> 0x8) & 0x000F) << manager->x);
  manager->pool[manager->y + 3] &= ~(((tetris >> 0xC) & 0x000F) << manager->x);
}
 
// =============================================================================
// ������ɫ
void setPoolColor(const TetrisManager *manager, TetrisControl *control)
{
  // ������ʾ��Ϸ��ʱ����Ҫ����Ϸ�����ж�ĳһ�����з������ʾ��Ӧ�������ɫ
  // ����ֻ�����ü��ɣ�û��Ҫ���
  // ���ƶ�������һ������ʱ����
 
  int8_t i, x, y;
 
  // ��ǰ����
  uint16_t tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]];
 
  for (i = 0; i < 16; ++i)
  {
    y = (i >> 2) + manager->y; // �����õ���
    if (y > ROW_END) // �����ײ�����
    {
      break;
    }
    x = (i & 3) + manager->x; // �����õ���
    if ((tetris >> i) & 1) // ���ĵ�С�������ڵ�ǰ��������
    {
      control->color[y][x] = (manager->type[0] | 8); // ������ɫ
    }
  }
}
 
// =============================================================================
// ��ת����
void rotateTetris(TetrisManager *manager, TetrisControl *control)
{
  int8_t ori = manager->orientation[0]; // ��¼ԭ��ת״̬
 
  removeTetris(manager); // ���ߵ�ǰ����
 
  // ˳/��ʱ����ת
  manager->orientation[0] = (control->clockwise) ? ((ori + 1) & 3) : ((ori + 3) & 3);
 
  if (checkCollision(manager)) // ��⵽��ײ
  {
    manager->orientation[0] = ori; // �ָ�Ϊԭ��ת״̬
    insertTetris(manager); // ���뵱ǰ���顣����״̬û�ı䣬����Ҫ������ɫ
  }
  else
  {
    insertTetris(manager); // ���뵱ǰ����
    setPoolColor(manager, control); // ������ɫ
    printCurrentTetris(manager, control); // ��ʾ��ǰ����
  }
}
 
// =============================================================================
// ˮƽ�ƶ�����
void horzMoveTetris(TetrisManager *manager, TetrisControl *control)
{
  int x = manager->x; // ��¼ԭ��λ��
 
  removeTetris(manager); // ���ߵ�ǰ����
  control->direction == 0 ? (--manager->x) : (++manager->x); // ��/���ƶ�
 
  if (checkCollision(manager)) // ��⵽��ײ
  {
    manager->x = x; // �ָ�Ϊԭ��λ��
    insertTetris(manager); // ���뵱ǰ���顣����λ��û�ı䣬����Ҫ������ɫ
  }
  else
  {
    insertTetris(manager); // ���뵱ǰ����
    setPoolColor(manager, control); // ������ɫ
    printCurrentTetris(manager, control); // ��ʾ��ǰ����
  }
}
 
// =============================================================================
// �����ƶ�����
void moveDownTetris(TetrisManager *manager, TetrisControl *control)
{
  int8_t y = manager->y; // ��¼ԭ��λ��
 
  removeTetris(manager); // ���ߵ�ǰ����
  ++manager->y; // �����ƶ�
 
  if (checkCollision(manager)) // ��⵽��ײ
  {
    manager->y = y; // �ָ�Ϊԭ��λ��
    insertTetris(manager); // ���뵱ǰ���顣����λ��û�ı䣬����Ҫ������ɫ
    if (checkErasing(manager, control)) // ��⵽����
    {
      printTetrisPool(manager, control); // ��ʾ��Ϸ��
    }
  }
  else
  {
    insertTetris(manager); // ���뵱ǰ����
    setPoolColor(manager, control); // ������ɫ
    printCurrentTetris(manager, control); // ��ʾ��ǰ����
  }
}
 
// =============================================================================
// ����ֱ�����
void dropDownTetris(TetrisManager *manager, TetrisControl *control)
{
  removeTetris(manager); // ���ߵ�ǰ����
  for (; manager->y < ROW_END; ++manager->y) // ��������
  {
    if (checkCollision(manager)) // ��⵽��ײ
    {
      break;
    }
  }
  --manager->y; // ����һ��Ȼû����ײ
 
  insertTetris(manager); // ���뵱ǰ����
  setPoolColor(manager, control); // ������ɫ
 
  checkErasing(manager, control); // �������
  printTetrisPool(manager, control); // ��ʾ��Ϸ��
}
 
// =============================================================================
// ���м��
bool checkErasing(TetrisManager *manager, TetrisControl *control)
{
  static const unsigned scores[5] = { 0, 10, 30, 90, 150 }; // ���е÷�
  int8_t count = 0;
  int8_t k = 0, y = manager->y + 3; // �������ϼ��
 
  do
  {
    if (y < ROW_END && manager->pool[y] == 0xFFFFU) // ��Ч��������һ��������
    {
      ++count;
      // ����һ�з���
      memmove(manager->pool + 1, manager->pool, sizeof(uint16_t) * y);
      // ��ɫ�����Ԫ����֮�ƶ�
      memmove(control->color[1], control->color[0], sizeof(int8_t [16]) * y);
    }
    else
    {
      --y;
      ++k;
    }
  } while (y >= manager->y && k < 4);
 
  manager->erasedTotal += count; // ��������
  manager->score += scores[count]; // �÷�
 
  if (count > 0)
  {
    ++manager->erasedCount[count - 1]; // ����
  }
 
  giveTetris(manager); // ����һ������
  setPoolColor(manager, control); // ������ɫ
 
  return (count > 0);
}
 
// =============================================================================
// ������
void keydownControl(TetrisManager *manager, TetrisControl *control, int key)
{
  if (key == 13) // ��ͣ/�����ͣ
  {
    control->pause = !control->pause;
  }
 
  if (control->pause) // ��ͣ״̬����������
  {
    return;
  }
 
  switch (key)
  {
  case 'w': case 'W': case '8': case 72: // ��
    control->clockwise = true; // ˳ʱ����ת
    rotateTetris(manager, control); // ��ת����
    break;
  case 'a': case 'A': case '4': case 75: // ��
    control->direction = 0; // �����ƶ�
    horzMoveTetris(manager, control); // ˮƽ�ƶ�����
    break;
  case 'd': case 'D': case '6': case 77: // ��
    control->direction = 1; // �����ƶ�
    horzMoveTetris(manager, control); // ˮƽ�ƶ�����
    break;
  case 's': case 'S': case '2': case 80: // ��
    moveDownTetris(manager, control); // �����ƶ�����
    break;
  case ' ': // ֱ�����
    dropDownTetris(manager, control);
    break;
  case '0': // ��ת
    control->clockwise = false; // ��ʱ����ת
    rotateTetris(manager, control); // ��ת����
    break;
  default:
    break;
  }
}
 
// =============================================================================
// ��ȫ�Ƕ�λ
void gotoxyWithFullwidth(short x, short y)
{
  static COORD cd;
 
  cd.X = (short)(x << 1);
  cd.Y = y;
  SetConsoleCursorPosition(g_hConsoleOutput, cd);
}
 
// =============================================================================
// ��ʾ��Ϸ�ر߽�
void printPoolBorder()
{
  int8_t y;
 
  SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
  gotoxyWithFullwidth(10, 0); // �ײ��߽�
  printf("%28s", "");
  for (y = ROW_BEGIN; y < ROW_END; ++y) // ����ʾ����4�к͵ײ�2��
  {
    gotoxyWithFullwidth(10, y - 3);
    printf("%2s", "");
    gotoxyWithFullwidth(23, y - 3);
    printf("%2s", "");
  }
 
  gotoxyWithFullwidth(10, y - 3); // �ײ��߽�
  printf("%28s", "");
}
 
// ��λ����Ϸ���еķ���
#define gotoxyInPool(x, y) gotoxyWithFullwidth(x + 9, y - 3)
 
// =============================================================================
// ��ʾ��Ϸ��
void printTetrisPool(const TetrisManager *manager, const TetrisControl *control)
{
  int8_t x, y;
 
  for (y = ROW_BEGIN; y < ROW_END; ++y) // ����ʾ����4�к͵ײ�2��
  {
    gotoxyInPool(2, y); // ���㵽��Ϸ���еķ���
    for (x = COL_BEGIN; x < COL_END; ++x) // ����ʾ���ұ߽�
    {
      if ((manager->pool[y] >> x) & 1) // ��Ϸ�ظ÷����з���
      {
        // ����Ӧ��ɫ����ʾһ��ʵ�ķ���
        SetConsoleTextAttribute(g_hConsoleOutput, control->color[y][x]);
        printf("��");
      }
      else // û�з��飬��ʾ�հ�
      {
        SetConsoleTextAttribute(g_hConsoleOutput, 0);
        printf("%2s", "");
      }
    }
  }
}
 
// =============================================================================
// ��ʾ��ǰ����
void printCurrentTetris(const TetrisManager *manager, const TetrisControl *control)
{
  int8_t x, y;
 
  // ��ʾ��ǰ���������ƶ�����õģ�Ϊ��ȥ�ƶ�ǰ�ķ��飬��Ҫ��չ��ʾ����
  // ���ڲ����������ƶ����ʲ���Ҫ������չ
  y = (manager->y > ROW_BEGIN) ? (manager->y - 1) : ROW_BEGIN; // ������չһ��
  for (; y < ROW_END && y < manager->y + 4; ++y)
  {
    x = (manager->x > COL_BEGIN) ? (manager->x - 1) : COL_BEGIN; // ������չһ��
    for (; x < COL_END && x < manager->x + 5; ++x) // ������չһ��
    {
      gotoxyInPool(x, y); // ���㵽��Ϸ���еķ���
      if ((manager->pool[y] >> x) & 1) // ��Ϸ�ظ÷����з���
      {
        // ����Ӧ��ɫ����ʾһ��ʵ�ķ���
        SetConsoleTextAttribute(g_hConsoleOutput, control->color[y][x]);
        printf("��");
      }
      else // û�з��飬��ʾ�հ�
      {
        SetConsoleTextAttribute(g_hConsoleOutput, 0);
        printf("%2s", "");
      }
    }
  }
}
 
// =============================================================================
// ��ʾ��һ��������һ������
void printNextTetris(const TetrisManager *manager)
{
  int8_t i;
  uint16_t tetris;
 
  // �߿�
  SetConsoleTextAttribute(g_hConsoleOutput, 0xF);
  gotoxyWithFullwidth(26, 1);
  printf("�����������ש���������");
  gotoxyWithFullwidth(26, 2);
  printf("��%8s��%8s��", "", "");
  gotoxyWithFullwidth(26, 3);
  printf("��%8s��%8s��", "", "");
  gotoxyWithFullwidth(26, 4);
  printf("��%8s��%8s��", "", "");
  gotoxyWithFullwidth(26, 5);
  printf("��%8s��%8s��", "", "");
  gotoxyWithFullwidth(26, 6);
  printf("�����������ߩ���������");
 
  // ��һ��������Ӧ��ɫ��ʾ
  tetris = gs_uTetrisTable[manager->type[1]][manager->orientation[1]];
  SetConsoleTextAttribute(g_hConsoleOutput, manager->type[1] | 8);
  for (i = 0; i < 16; ++i)
  {
    gotoxyWithFullwidth((i & 3) + 27, (i >> 2) + 2);
    ((tetris >> i) & 1) ? printf("��") : printf("%2s", "");
  }
 
  // ����һ��������ʾ��ɫ
  tetris = gs_uTetrisTable[manager->type[2]][manager->orientation[2]];
  SetConsoleTextAttribute(g_hConsoleOutput, 8);
  for (i = 0; i < 16; ++i)
  {
    gotoxyWithFullwidth((i & 3) + 32, (i >> 2) + 2);
    ((tetris >> i) & 1) ? printf("��") : printf("%2s", "");
  }
}
 
// =============================================================================
// ��ʾ�÷���Ϣ
void printScore(const TetrisManager *manager)
{
  static const char *tetrisName = "ITLJZSO";
  int8_t i;
 
  SetConsoleTextAttribute(g_hConsoleOutput, 0xE);
 
  gotoxyWithFullwidth(2, 2);
  printf("���÷֣�%u", manager->score);
 
  gotoxyWithFullwidth(1, 6);
  printf("������������%u", manager->erasedTotal);
  for (i = 0; i < 4; ++i)
  {
    gotoxyWithFullwidth(2, 8 + i);
    printf("����%d��%u", i + 1, manager->erasedCount[i]);
  }
 
  gotoxyWithFullwidth(1, 15);
  printf("������������%u", manager->tetrisTotal);
 
  for (i = 0; i < 7; ++i)
  {
    gotoxyWithFullwidth(2, 17 + i);
    printf("��%c�Σ�%u", tetrisName[i], manager->tetrisCount[i]);
  }
}
 
// =============================================================================
// ��ʾ��ʾ��Ϣ
void printPrompting()
{
  SetConsoleTextAttribute(g_hConsoleOutput, 0xB);
  gotoxyWithFullwidth(26, 10);
  printf("�����ƣ�");
  gotoxyWithFullwidth(27, 12);
  printf("�������ƶ����� A 4");
  gotoxyWithFullwidth(27, 13);
  printf("�������ƶ����� D 6");
  gotoxyWithFullwidth(27, 14);
  printf("�������ƶ����� S 2");
  gotoxyWithFullwidth(27, 15);
  printf("��˳ʱ��ת���� W 8");
  gotoxyWithFullwidth(27, 16);
  printf("����ʱ��ת��0");
  gotoxyWithFullwidth(27, 17);
  printf("��ֱ����أ��ո�");
  gotoxyWithFullwidth(27, 18);
  printf("����ͣ��Ϸ���س�");
  gotoxyWithFullwidth(25, 23);
}
 
// =============================================================================
// ������Ϸ
void runGame(TetrisManager *manager, TetrisControl *control)
{
  clock_t clockLast, clockNow;
 
  clockLast = clock(); // ��ʱ
  printTetrisPool(manager, control); // ��ʾ��Ϸ��
 
  while (!manager->dead) // û��
  {
    while (_kbhit()) // �м�����
    {
      keydownControl(manager, control, _getch()); // ������
    }
 
    if (!control->pause) // δ��ͣ
    {
      clockNow = clock(); // ��ʱ
      // ���μ�ʱ�ļ������0.45��
      if (clockNow - clockLast > 0.45F * CLOCKS_PER_SEC)
      {
        clockLast = clockNow;
        keydownControl(manager, control, 80); // ����������
      }
    }
  }
}
 
// =============================================================================
// ����һ��
bool ifPlayAgain()
{
  int ch;
 
  SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
  gotoxyWithFullwidth(15, 10);
  printf("��Ϸ����");
  gotoxyWithFullwidth(13, 11);
  printf("��Y���棬��N�˳�");
 
  do
  {
    ch = _getch();
    if (ch == 'Y' || ch == 'y')
    {
      return true;
    }
    else if (ch == 'N' || ch == 'n')
    {
      return false;
    }
  } while (1);
}

// ������
int tetris_main(int argc,char **argv)
{
  CONSOLE_SCREEN_BUFFER_INFO info;
  TetrisManager tetrisManager;
  TetrisControl tetrisControl;
  CONSOLE_CURSOR_INFO cursorInfo = { 1, TRUE }; // �����Ϣ
  
  g_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // ��ȡ����̨������
  GetConsoleScreenBufferInfo(g_hConsoleOutput, &info);
  initGame(&tetrisManager, &tetrisControl); // ��ʼ����Ϸ

  do
  {
    printPrompting(); // ��ʾ��ʾ��Ϣ
    printPoolBorder(); // ��ʾ��Ϸ�ر߽�
    runGame(&tetrisManager, &tetrisControl); // ������Ϸ
    if (ifPlayAgain()) // ����һ��
    {
      SetConsoleTextAttribute(g_hConsoleOutput, 0x7);
      system("cls"); // ����
      restartGame(&tetrisManager, &tetrisControl); // ���¿�ʼ��Ϸ
    }
    else
    {
      break;
    }
  } while (1);
  gotoxyWithFullwidth(0, 0);
  SetConsoleTextAttribute(g_hConsoleOutput, info.wAttributes);
  system("cls"); // ����
  g_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // ��ȡ����̨������
  SetConsoleCursorInfo(g_hConsoleOutput, &cursorInfo); // ���ù������
  return 0;
}
 

