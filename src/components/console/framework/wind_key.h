/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_key.h
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: ����PC�ļ��̵�ASCII��ֵ�Ķ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2013.08.10
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_KEY_H__
#define WIND_KEY_H__
#ifdef __cplusplus
extern "C" {
#endif

#define WVK_NULL         0x00            // null char
#define WVK_SOH          0x01            // Ctrl+A :
#define WVK_STX          0x02            // Ctrl+B : 
#define WVK_ETX          0x03            // Ctrl+C : 
#define WVK_EOT          0x04            // Ctrl+D : end of transmit
#define WVK_ENQ          0x05            // Ctrl+E : 
#define WVK_ACK          0x06            // Ctrl+F : 
#define WVK_BELL         0x07            // Ctrl+G : bell sound
#define WVK_BACKSPACE    0x08            // Ctrl+H : backspace
#define WVK_TAB          0x09            // Ctrl+I : harizontal tab
#define WVK_LINEFEED     0x0A            // Ctrl+J : line feed, EOL(end of line for UNIX)
#define WVK_VTAB         0x0B            // Ctrl+K : vertical tab
#define WVK_FORMFEED     0x0C            // Ctrl+L : form feed
#define WVK_ENTER        0x0D            // Ctrl+M : carriage return
#define WVK_SO           0x0E            // Ctrl+N : 
#define WVK_SI           0x0F            // Ctrl+O : 
#define WVK_DLE          0x10            // Ctrl+P : 
#define WVK_XON          0x11            // Ctrl+Q : device control 1, XON
#define WVK_DC2          0x12            // Ctrl+R : 
#define WVK_XOFF         0x13            // Ctrl+S : device control 3, XOFF
#define WVK_DC4          0x14            // Ctrl+T : 
#define WVK_NAK          0x15            // Ctrl+U : Not ACK
#define WVK_SYN          0x16            // Ctrl+V : 
#define WVK_ETB          0x17            // Ctrl+W : 
#define WVK_CAN          0x18            // Ctrl+X : 
#define WVK_EM           0x19            // Ctrl+Y : 
#define WVK_SUB          0x1A            // Ctrl+Z : EOF(end of file for MSDOS)
#define WVK_EOF          0x1A            // Ctrl+Z : EOF(end of file for MSDOS)
#define WVK_ESCAPE       0x1B            // Ctrl+[ : escape
#define WVK_FS           0x1C            // Ctrl+ : 
#define WVK_GS           0x1D            // Ctrl+] : 
#define WVK_RS           0x1E            // Ctrl+^ : 
#define WVK_US           0x1F            // Ctrl+_ : 
//
// PRINTABLE ASCII CODE
//
#define WVK_SPACE        0x20            // space char

#define WVK_0            0x30            /* 0 */
#define WVK_1            0x31            /* 1 */
#define WVK_2            0x32            /* 2 */
#define WVK_3            0x33            /* 3 */
#define WVK_4            0x34            /* 4 */
#define WVK_5            0x35            /* 5 */
#define WVK_6            0x36            /* 6 */
#define WVK_7            0x37            /* 7 */
#define WVK_8            0x38            /* 8 */
#define WVK_9            0x39            /* 9 */
#define WVK_A            0x41            /* A */
#define WVK_B            0x42            /* B */
#define WVK_C            0x43            /* C */
#define WVK_D            0x44            /* D */
#define WVK_E            0x45            /* E */
#define WVK_F            0x46            /* F */
#define WVK_G            0x47            /* G */
#define WVK_H            0x48            /* H */
#define WVK_I            0x49            /* I */
#define WVK_J            0x4A            /* J */
#define WVK_K            0x4B            /* K */
#define WVK_L            0x4C            /* L */
#define WVK_M            0x4D            /* M */
#define WVK_N            0x4E            /* N */
#define WVK_O            0x4F            /* O */
#define WVK_P            0x50            /* P */
#define WVK_Q            0x51            /* Q */
#define WVK_R            0x52            /* R */
#define WVK_S            0x53            /* S */
#define WVK_T            0x54            /* T */
#define WVK_U            0x55            /* U */
#define WVK_V            0x56            /* V */
#define WVK_W            0x57            /* W */
#define WVK_X            0x58            /* X */
#define WVK_Y            0x59            /* Y */
#define WVK_Z            0x5A            /* Z */
#define WVK_a            0x61            /* a */
#define WVK_b            0x62            /* b */
#define WVK_c            0x63            /* c */
#define WVK_d            0x64            /* d */
#define WVK_e            0x65            /* e */
#define WVK_f            0x66            /* f */
#define WVK_g            0x67            /* g */
#define WVK_h            0x68            /* h */
#define WVK_i            0x69            /* i */
#define WVK_j            0x6A            /* j */
#define WVK_k            0x6B            /* k */
#define WVK_l            0x6C            /* l */
#define WVK_m            0x6D            /* m */
#define WVK_n            0x6E            /* n */
#define WVK_o            0x6F            /* o */
#define WVK_p            0x70            /* p */
#define WVK_q            0x71            /* q */
#define WVK_r            0x72            /* r */
#define WVK_s            0x73            /* s */
#define WVK_t            0x74            /* t */
#define WVK_u            0x75            /* u */
#define WVK_v            0x76            /* v */
#define WVK_w            0x77            /* w */
#define WVK_x            0x78            /* x */
#define WVK_y            0x79            /* y */
#define WVK_z            0x7A            /* z */

#define WVK_DEL          0x7f            /* del */

//KEY event
#define KEY_EVT_UP           0x1B5B41
#define KEY_EVT_DOWN         0x1B5B42
#define KEY_EVT_LEFT         0x1B5B44
#define KEY_EVT_RIGHT        0x1B5B43

#define KEY_EVT_HOME         0x1B5B317E
#define KEY_EVT_INS          0x1B5B327E
#define KEY_EVT_DEL          0x1B5B337E
#define KEY_EVT_END          0x1B5B347E
#define KEY_EVT_PGUP         0x1B5B357E
#define KEY_EVT_PGDN         0x1B5B367E


//control words
#define VT100_RESET    "\033[0m" //��������
#define VT100_BRIGHT   "\033[1m" //����
#define VT100_DIM      "\033[2m" //�߰�
#define VT100_UNDERSCORE "\033[4m" //�»���
#define VT100_BLINK      "\033[5m" //��˸
#define VT100_REVERSE    "\033[7m" //����
#define VT100_HIDDEN     "\033[8m" //����

#define VT100_ERASE_END  "\033[K"  //����ӹ�굽��β������
#define VT100_ERASE_HEAD "\033[1K" //����ӹ�굽���׵�����
#define VT100_ERASE_LINE "\033[2K" //������е�����


#define VT100_ERASE_DOWN   "\033[J"  //����ӵ�ǰ�е���Ļ�ײ�
#define VT100_ERASE_UP     "\033[1J" //����ӵ�ǰ�е���Ļ����
#define VT100_ERASE_SCREEN "\033[2J" //�����Ļ





#ifdef __cplusplus
}
#endif


#endif
