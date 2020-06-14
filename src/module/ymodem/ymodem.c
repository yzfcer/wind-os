#include "util.h" 
#include "wind_debug.h" 
#include "ymodem.h" 

 
static const char *u32_to_str(w_uint32_t val) 
{ 
	static char num_str[11]; 
	w_int32_t  pos = 10; 
	num_str[10] = 0; 
	if (val == 0) 
		return "0";  
	while ((val != 0) && (pos > 0)) 
	{ 
		num_str[--pos] = (val % 10) + '0'; 
		val /= 10; 
	} 

	return &num_str[pos]; 
} 
 
static w_uint32_t str_to_u32(char* str) 
{ 
	const char *s = str; 
	w_uint32_t acc; 
	w_int32_t c; 
	do 
	{ 
		c = *s++; 
	} while (c == ' '); 

	for (acc = 0; (c >= '0') && (c <= '9'); c = *s++) 
	{ 
		c -= '0'; 
		acc *= 10; 
		acc += c; 
	} 
	return acc; 
} 
 
static w_int32_t recv_packet(char *data, w_int32_t *length) 
{ 
        w_int32_t i, c; 
        w_uint32_t packet_size; 
 
        *length = 0; 
 
        c = _getchar(PACKET_TIMEOUT); 
        if (c < 0) 
		{ 
                return -1; 
        } 
 
        switch(c) { 
        case SOH: 
                packet_size = PACKET_SIZE; 
                break; 
        case STX: 
                packet_size = PACKET_1K_SIZE; 
                break; 
        case EOT: 
                return 0; 
        case CAN: 
                c = _getchar(PACKET_TIMEOUT); 
                if (c == CAN) { 
                        *length = -1; 
                        return 0; 
                } 
        default: 
                *length = -1; 
                return 0; 
        } 
 
        *data = (char)c; 
 
        for(i = 1; i < (packet_size + PACKET_OVERHEAD); ++i) { 
                c = _getchar(PACKET_TIMEOUT); 
                if (c < 0) { 
                        return -1; 
                } 
                data[i] = (char)c; 
        } 
 
        if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) { 
                return 1; 
        } 
 
        if (wind_crc16(data + PACKET_HEADER, packet_size + PACKET_TRAILER,0) != 0) { 
                return 1; 
        } 
        *length = packet_size; 
 
        return 0; 
} 
 
w_uint32_t ymodem_recv(unsigned char *buf, w_uint32_t length) 
{ 
        unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD]; 
        w_int32_t packet_length, i, file_done, session_done, crc_tries, crc_nak; 
        w_uint32_t packets_recvd, errors, first_try = 1; 
        char file_name[FILE_NAME_LENGTH], file_size[FILE_SIZE_LENGTH], *file_ptr; 
        unsigned char *buf_ptr; 
        w_uint32_t size = 0; 
 
        wind_printf("Ymodem rcv:\n"); 
        file_name[0] = 0; 
 
        for (session_done = 0, errors = 0; ; ) { 
                crc_tries = crc_nak = 1; 
                if (!first_try) { 
                        _putchar(CRC); 
                } 
                first_try = 0; 
                for (packets_recvd = 0, file_done = 0, buf_ptr = buf; ; ) { 
                        switch (recv_packet(packet_data, &packet_length)) { 
 
                        case 0: 
                                errors = 0; 
                                switch (packet_length) { 
                                        case -1:
                                                _putchar(ACK); 
                                                return 0; 
                                        case 0:
                                                _putchar(ACK); 
                                                file_done = 1; 
                                                break; 
                                        default:
                                        if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_recvd & 0xff)) { 
                                                _putchar(NAK); 
                                        } else { 
                                                if (packets_recvd == 0) { 
                                                        for (i = PACKET_HEADER; i < PACKET_HEADER + 4; i++) { 
                                                                if (packet_data[i] != 0) { 
                                                                        break; 
                                                                } 
                                                        } 
                                                        if (i < PACKET_HEADER + 4) {
                                                                for (file_ptr = packet_data + PACKET_HEADER, i = 0; *file_ptr && i < FILE_NAME_LENGTH; ) { 
                                                                        file_name[i++] = *file_ptr++; 
                                                                } 
                                                                file_name[i++] = '\0'; 
                                                                for (++file_ptr, i = 0; *file_ptr != ' ' && i < FILE_SIZE_LENGTH; ) { 
                                                                        file_size[i++] = *file_ptr++; 
                                                                } 
                                                                file_size[i++] = '\0'; 
                                                                size = str_to_u32(file_size); 
                                                                if (size > length) { 
                                                                        _putchar(CAN); 
                                                                        _putchar(CAN); 
                                                                        _sleep(1); 
                                                                        wind_printf("\nrcv buffer too small (0x%08x vs 0x%08x)\n", length, size); 
                                                                        return 0; 
                                                                } 
                                                                _putchar(ACK); 
                                                                _putchar(crc_nak ? CRC : NAK); 
                                                                crc_nak = 0; 
                                                        } else {
                                                                _putchar(ACK); 
                                                                file_done = 1; 
                                                                session_done = 1; 
                                                                break; 
                                                        } 
                                                } else { 
                                                        if ((buf_ptr + packet_length) - buf > length) { 
                                                                _putchar(CAN); 
                                                                _putchar(CAN); 
                                                                _sleep(1); 
                                                                wind_printf("\nbuffer overflow: %d\n", length); 
                                                                return 0; 
                                                        } 
                                                        for (i=0; i<packet_length; i++) { 
                                                                buf_ptr[i] = packet_data[PACKET_HEADER+i]; 
                                                        } 
                                                        buf_ptr += packet_length; 
                                                        _putchar(ACK); 
                                                } 
                                                ++packets_recvd; 
                                        }
                                } 
                                break; 
 
                        default: 
                                if (packets_recvd != 0) { 
                                        if (++errors >= MAX_ERRORS) { 
                                                _putchar(CAN); 
                                                _putchar(CAN); 
                                                _sleep(1); 
                                                wind_printf("\ntoo many errors - aborted.\n"); 
                                                return 0; 
                                        } 
                                } 
                                _putchar(CRC); 
                        } 
                        if (file_done) { 
                                break; 
                        } 
                }
 
                if (session_done) 
                        break; 
 
        }
 
        wind_printf("\n"); 
        if (size > 0) { 
                wind_printf("read:%s\n", file_name); 
                wind_printf("len:0x%08x\n", size); 
        } 
        return size; 
} 
 
static void send_packet(unsigned char *data, w_int32_t block_no) 
{ 
        w_int32_t count, crc, packet_size; 
         if (block_no == 0) { 
                packet_size = PACKET_SIZE; 
        } else { 
                packet_size = PACKET_1K_SIZE; 
        } 
        crc = wind_crc16(data, packet_size,0); 
        _putchar((block_no==0)?SOH:STX); 
        _putchar(block_no & 0xFF); 
        _putchar(~block_no & 0xFF); 
 
        for (count=0; count<packet_size; count++) { 
                _putchar(data[count]); 
        } 
        _putchar((crc >> 8) & 0xFF); 
        _putchar(crc & 0xFF); 
} 
 
static void send_packet0(char* filename, w_uint32_t size) 
{ 
        w_uint32_t count = 0; 
        unsigned char block[PACKET_SIZE]; 
        const char* num; 
 
        if (filename) { 
                while (*filename && (count < PACKET_SIZE-FILE_SIZE_LENGTH-2)) { 
                        block[count++] = *filename++; 
                } 
                block[count++] = 0; 
 
                num = u32_to_str(size); 
                while(*num) { 
                        block[count++] = *num++; 
                } 
        } 
 
        while (count < PACKET_SIZE) { 
                block[count++] = 0; 
        } 
 
        send_packet(block, 0); 
} 
 
 
static void send_data_packets(unsigned char* data, w_uint32_t size) 
{ 
        w_int32_t blockno = 1; 
        w_uint32_t send_size; 
        w_int32_t ch; 
 
        while (size > 0) { 
                if (size > PACKET_1K_SIZE) { 
                        send_size = PACKET_1K_SIZE; 
                } else { 
                        send_size = size; 
                } 
 
                send_packet(data, blockno); 
                ch = _getchar(PACKET_TIMEOUT); 
                if (ch == ACK) { 
                        blockno++; 
                        data += send_size; 
                        size -= send_size; 
                } else { 
                        if((ch == CAN) || (ch == -1)) { 
                                return; 
                        } 
                } 
        } 
 
        do { 
                _putchar(EOT); 
                ch = _getchar(PACKET_TIMEOUT); 
        } while((ch != ACK) && (ch != -1)); 
 
        
        if (ch == ACK) { 
                ch = _getchar(PACKET_TIMEOUT); 
                if (ch == CRC) { 
                        do { 
                                send_packet0(0, 0); 
                                ch = _getchar(PACKET_TIMEOUT); 
                        } while((ch != ACK) && (ch != -1)); 
                } 
        } 
} 
 
w_uint32_t ymodem_send(unsigned char* buf, w_uint32_t size, char* filename) 
{ 
        w_int32_t ch, crc_nak = 1; 
 
        wind_printf("Ymodem send:\n"); 
 
        _sleep(1); 
        while (serial_read() >= 0); 
 
        do { 
                _putchar(CRC); 
                ch = _getchar(1); 
        } while (ch < 0); 
 
        if (ch == CRC) { 
                do { 
                        send_packet0(filename, size); 
                        ch = _getchar(PACKET_TIMEOUT); 
 
                        if (ch == ACK) { 
                                ch = _getchar(PACKET_TIMEOUT); 
                                if (ch == CRC) { 
                                        send_data_packets(buf, size); 
                                        wind_printf("\nsent:%s\n", filename); 
                                        wind_printf("len:0x%08x\n", size); 
                                        return size; 
                                } 
                        } else if ((ch == CRC) && (crc_nak)) { 
                                crc_nak = 0; 
                                continue; 
                        } else if ((ch != NAK) || (crc_nak)) { 
                                break; 
                        } 
                } while(1); 
        } 
        _putchar(CAN); 
        _putchar(CAN); 
        _sleep(1); 
        wind_printf("\naborted.\n"); 
        return 0; 
} 