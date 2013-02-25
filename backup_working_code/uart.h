//baudrate = 115200, F_CPU = 16000000
void uart_init();
void uart_send_byte(unsigned char c);
void uart_print_string(unsigned char *p);
void uart_print_num(int i);

