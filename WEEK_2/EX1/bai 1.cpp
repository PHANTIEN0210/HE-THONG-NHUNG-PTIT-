#define RCC_APB2ENR (*(volatile unsigned int *)0x40021018)

#define GPIOA_CRH   (*(volatile unsigned int *)0x40010804)

#define USART1_SR   (*(volatile unsigned int *)0x40013800)
#define USART1_DR   (*(volatile unsigned int *)0x40013804)
#define USART1_BRR  (*(volatile unsigned int *)0x40013808)
#define USART1_CR1  (*(volatile unsigned int *)0x4001380C)

#define BUFFER_SIZE 64

char rx_buffer[BUFFER_SIZE];
unsigned int rx_index = 0;


/*
 * Khoi tao USART1
 *
 * CPU clock : 8 MHz
 * PCLK2     : 8 MHz
 * Baud rate : 115200
 * Data      : 8 bit
 * Parity     : None
 * Stop bit   : 1
 */
void USART1_Init(void)
{
    /*
     * Bat clock GPIOA
     * Bit 2
     */
    RCC_APB2ENR |= (1 << 2);

    /*
     * Bat clock USART1
     * Bit 14
     */
    RCC_APB2ENR |= (1 << 14);

    /*
     * PA9  = USART1_TX
     *       Alternate Function Push-Pull 50MHz
     *
     * PA10 = USART1_RX
     *       Input Floating
     */
    GPIOA_CRH &= 0xFFFFF00F;
    GPIOA_CRH |= 0x000004B0;

    /*
     * PCLK2 = 8 MHz
     * Baud rate = 115200
     *
     * USARTDIV = 8MHz / (16 * 115200)
     *
     * BRR = 0x0045
     */
    USART1_BRR = 0x0045;

    /*
     * UE = bit 13
     * TE = bit 3
     * RE = bit 2
     */
    USART1_CR1 = (1 << 13)
               | (1 << 3)
               | (1 << 2);
}


/*
 * Gui 1 ky tu
 */
void USART1_SendChar(char c)
{
    /*
     * Cho TXE = 1
     */
    while (!(USART1_SR & (1 << 7)))
    {
    }

    USART1_DR = c;
}


/*
 * Gui chuoi
 */
void USART1_SendString(const char *str)
{
    while (*str)
    {
        USART1_SendChar(*str);
        str++;
    }
}


/*
 * Nhan 1 ky tu
 */
char USART1_ReceiveChar(void)
{
    /*
     * Cho RXNE = 1
     */
    while (!(USART1_SR & (1 << 5)))
    {
    }

    return (char)(USART1_DR & 0xFF);
}


int main(void)
{
    USART1_Init();

    /*
     * Thong bao USART da khoi dong
     */
    USART1_SendString("STM32 UART START\r\n");

    while (1)
    {
        char c;

        /*
         * Nhan ky tu tu PC
         */
        c = USART1_ReceiveChar();

        /*
         * Neu nhan ky tu !
         * thi ket thuc ban tin
         */
        if (c == '!')
        {
            /*
             * Ket thuc chuoi
             */
            rx_buffer[rx_index] = '\0';

            /*
             * Gui ma lop + ma nhom
             */
            USART1_SendString("D23HTNN01: ");

            /*
             * Gui lai ban tin
             */
            USART1_SendString(rx_buffer);

            /*
             * Xuong dong
             */
            USART1_SendString("\r\n");

            /*
             * Reset bo dem
             */
            rx_index = 0;
        }
        else
        {
            /*
             * Luu ky tu vao buffer
             */
            if (rx_index < BUFFER_SIZE - 1)
            {
                rx_buffer[rx_index] = c;
                rx_index++;
            }
            else
            {
                /*
                 * Buffer day
                 * reset de nhan lai
                 */
                rx_index = 0;
            }
        }
    }
}
