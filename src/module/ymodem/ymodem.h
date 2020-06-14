#if !defined(_YMODEM_H) 
#define _YMODEM_H 
 
#define PACKET_SEQNO_INDEX      (1) 
#define PACKET_SEQNO_COMP_INDEX (2) 
 
#define PACKET_HEADER           (3)     // start, block, block-complement
#define PACKET_TRAILER          (2)     // CRC bytes
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER) 
#define PACKET_SIZE             (128) 
#define PACKET_1K_SIZE          (1024) 
#define PACKET_TIMEOUT          (1) 
 
#define INITIAL 
 
#define FILE_NAME_LENGTH (64) 
#define FILE_SIZE_LENGTH (16) 
 
// ASCII control codes:
#define SOH (0x01)      // start of 128-byte data packet
#define STX (0x02)      // start of 1024-byte data packet
#define EOT (0x04)      // end of transmission
#define ACK (0x06)      // recv OK
#define NAK (0x15)      // recvr error; retry
#define CAN (0x18)      // two of these in succession aborts transfer
#define CRC (0x43)      // use in place of first NAK for CRC mode
 
// Number of consecutive recv errors before giving up
#define MAX_ERRORS    (5) 
 
extern w_uw_int32_t32_t ymodem_recv(unsigned char *buf, w_uw_int32_t32_t length); 
extern w_uw_int32_t32_t ymodem_send(unsigned char *buf, w_uw_int32_t32_t size, char* filename); 
 
#endif