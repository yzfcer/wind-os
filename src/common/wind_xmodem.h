#ifndef WIND_XMODEM_H__
#define WIND_XMODEM_H__
#include "wind_config.h"
#include "wind_type.h"  
#define XMODEM_BUFF_LEN 1030
#define XMODEM_MIN_LEN 128
typedef enum 
{
    XM_IDLE = 0,
    XM_RECV_WAIT_HEAD,
    XM_RECV_DATA,
    XM_RECV_COPY_DATA,
    XM_RECV_ACK,
    XM_RECV_CAN,
    XM_RECV_EOT,
    XM_SEND_WAIT_START,
    XM_SEND_PACK_DATA,
    XM_SEND_DATA,
    XM_SEND_WAIT_ACK,
    XM_ERROR
}xm_stat_e;

typedef enum 
{
    XM_DIR_INIT = 0,
    XM_DIR_RECV = 1,
    XM_DIR_SEND = 2,
}xm_dir_e;


typedef w_int32_t (*xm_write_fn)(w_uint8_t ch);
typedef w_int32_t (*xm_read_fn)(w_uint8_t *ch,w_uint32_t time_out);

typedef struct 
{
    xm_stat_e stat;//xmodem״̬��״̬
    xm_dir_e dir;//���䷽��
    w_uint8_t crcmode;//У��ģʽ��1Ϊcrc��0ΪУ���
    w_uint8_t trychar;//�����ַ�
    w_uint8_t  pack_no;//�����
    w_uint8_t  ack;//Ӧ��
    w_int8_t retry;//���Դ���
    
    //֡���ݻ���
    w_uint8_t *frbuff;//֡���ݻ���
    w_int16_t frbuffsize;//����֡����ռ��С
    w_int16_t frlen;//����֡�ռ䳤��
    w_int16_t frdatalen;//ÿ֡�е����ݲ��ֳ���
    w_int16_t fridx;//֡�����±�

    //��Ч���ݻ���
    w_uint8_t *buff;//��������ݻ���
    w_int16_t bufflen;//��������ݳ���
    w_int16_t buffidx;//��������ݳ���

    //����ӿ�
    xm_write_fn write;//����ӿ�
    xm_read_fn read;//����ӿ�
}xm_ctx_s;


w_err_t xmodem_init(xm_ctx_s *ctx,xm_dir_e dir,w_uint8_t *frbuff,
                    w_int32_t frbuff_size,
                    xm_write_fn write,xm_read_fn read);

w_int32_t xmodem_recv(xm_ctx_s *ctx,w_uint8_t *data, w_int32_t size);

w_int32_t xmodem_send(xm_ctx_s *ctx,w_uint8_t *data, w_int32_t size);

w_err_t xmodem_end(xm_ctx_s *ctx);

#endif

