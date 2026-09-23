/*========================================================
 * BAI 3 - ADC STM32F103
 *
 * CPU / PCLK2 = 8 MHz
 * USART1 = 9600 baud
 *
 * ADC1 Channel 3
 * PA3 = Analog Input
 *
 * Bien tro:
 *   Chan ngoai  -> 3.3V
 *   Chan giua   -> PA3
 *   Chan ngoai  -> GND
 *
 * Khong dung HAL
 *========================================================*/


/*========================================================
 * RCC
 *========================================================*/
#define RCC_APB2ENR   (*(volatile unsigned int *)0x40021018)


/*========================================================
 * GPIOA
 *========================================================*/
#define GPIOA_CRL     (*(volatile unsigned int *)0x40010800)
#define GPIOA_CRH     (*(volatile unsigned int *)0x40010804)


/*========================================================
 * USART1
 *========================================================*/
#define USART1_SR     (*(volatile unsigned int *)0x40013800)
#define USART1_DR     (*(volatile unsigned int *)0x40013804)
#define USART1_BRR    (*(volatile unsigned int *)0x40013808)
#define USART1_CR1    (*(volatile unsigned int *)0x4001380C)


/*========================================================
 * ADC1
 *========================================================*/
#define ADC1_SR       (*(volatile unsigned int *)0x40012400)
#define ADC1_CR1      (*(volatile unsigned int *)0x40012404)
#define ADC1_CR2      (*(volatile unsigned int *)0x40012408)
#define ADC1_SMPR2    (*(volatile unsigned int *)0x40012410)
#define ADC1_SQR1     (*(volatile unsigned int *)0x4001242C)
#define ADC1_SQR3     (*(volatile unsigned int *)0x40012434)
#define ADC1_DR       (*(volatile unsigned int *)0x4001244C)


/*========================================================
 * DELAY
 * CPU = 8 MHz
 *========================================================*/
void delay_ms(unsigned int ms)
{
    volatile unsigned int i;

    while (ms--)
    {
        for (i = 0; i < 2000; i++)
        {
            __asm volatile ("nop");
        }
    }
}


/*========================================================
 * USART1 INIT
 *
 * PCLK2 = 8 MHz
 * Baudrate = 9600
 *
 * BRR = 0x0341
 *========================================================*/
void USART1_Init(void)
{
    /* Enable GPIOA clock */
    RCC_APB2ENR |= (1 << 2);

    /* Enable USART1 clock */
    RCC_APB2ENR |= (1 << 14);


    /*
     * PA9  = USART1_TX
     * PA10 = USART1_RX
     */
    GPIOA_CRH &= 0xFFFFF00F;
    GPIOA_CRH |= 0x000004B0;


    /*
     * PCLK2 = 8 MHz
     * Baudrate = 9600
     */
    USART1_BRR = 0x0341;


    /*
     * UE = bit 13
     * TE = bit 3
     * RE = bit 2
     */
    USART1_CR1 =
        (1 << 13) |
        (1 << 3)  |
        (1 << 2);
}


/*========================================================
 * USART1 SEND CHAR
 *========================================================*/
void USART1_SendChar(char c)
{
    while (!(USART1_SR & (1 << 7)))
    {
    }

    USART1_DR = c;
}


/*========================================================
 * USART1 SEND STRING
 *========================================================*/
void USART1_SendString(const char *str)
{
    while (*str)
    {
        USART1_SendChar(*str);
        str++;
    }
}


/*========================================================
 * USART1 SEND NUMBER
 *========================================================*/
void USART1_SendNumber(unsigned int number)
{
    char buffer[10];
    int i = 0;

    if (number == 0)
    {
        USART1_SendChar('0');
        return;
    }

    while (number > 0)
    {
        buffer[i] = (number % 10) + '0';
        i++;

        number = number / 10;
    }

    while (i > 0)
    {
        i--;

        USART1_SendChar(buffer[i]);
    }
}


/*========================================================
 * ADC1 INIT
 *
 * ADC1 Channel 3
 * PA3 = Analog Input
 *========================================================*/
void ADC1_Init(void)
{
    /*-----------------------------------------------
     * Enable GPIOA clock
     *-----------------------------------------------*/
    RCC_APB2ENR |= (1 << 2);


    /*-----------------------------------------------
     * Enable ADC1 clock
     *-----------------------------------------------*/
    RCC_APB2ENR |= (1 << 9);


    /*-----------------------------------------------
     * PA3 = Analog Input
     *
     * PA3:
     * MODE3 = 00
     * CNF3  = 00
     *
     * PA3 nam trong GPIOA_CRL
     *
     * Moi chan GPIO chiem 4 bit.
     * PA3 bat dau tai bit 12.
     *-----------------------------------------------*/
    GPIOA_CRL &= ~(0xF << 12);


    /*-----------------------------------------------
     * ADC Channel 3 sample time
     *
     * Channel 3:
     *
     * SMP3 = bit 11:9
     *
     * 111 = 239.5 cycles
     *-----------------------------------------------*/
    ADC1_SMPR2 &= ~(7 << 9);

    ADC1_SMPR2 |= (7 << 9);


    /*-----------------------------------------------
     * Regular sequence:
     * Chi co 1 conversion
     *
     * L = 0000
     *-----------------------------------------------*/
    ADC1_SQR1 &= ~(0xF << 20);


    /*-----------------------------------------------
     * SQ1 = Channel 3
     *
     * SQR3 bit 4:0
     *-----------------------------------------------*/
    ADC1_SQR3 &= ~(0x1F << 0);

    ADC1_SQR3 |= (3 << 0);


    /*-----------------------------------------------
     * EXTSEL = 111
     *
     * Chon software trigger
     *
     * Bit 19:17
     *-----------------------------------------------*/
    ADC1_CR2 &= ~(7 << 17);

    ADC1_CR2 |= (7 << 17);


    /*-----------------------------------------------
     * EXTTRIG = 1
     *
     * Bit 20
     *
     * Cho phep trigger
     *-----------------------------------------------*/
    ADC1_CR2 |= (1 << 20);


    /*-----------------------------------------------
     * Enable ADC
     *
     * ADON = bit 0
     *-----------------------------------------------*/
    ADC1_CR2 |= (1 << 0);


    /* Cho ADC on dinh */
    delay_ms(10);


    /*-----------------------------------------------
     * Reset calibration
     *
     * RSTCAL = bit 3
     *-----------------------------------------------*/
    ADC1_CR2 |= (1 << 3);

    while (ADC1_CR2 & (1 << 3))
    {
    }


    /*-----------------------------------------------
     * Start calibration
     *
     * CAL = bit 2
     *-----------------------------------------------*/
    ADC1_CR2 |= (1 << 2);

    while (ADC1_CR2 & (1 << 2))
    {
    }
}


/*========================================================
 * ADC1 READ
 *
 * ADC1 Channel 3
 * PA3
 *========================================================*/
unsigned int ADC1_Read(void)
{
    /*-----------------------------------------------
     * Clear EOC
     *-----------------------------------------------*/
    ADC1_SR &= ~(1 << 1);


    /*-----------------------------------------------
     * Start ADC conversion
     *
     * SWSTART = bit 22
     *-----------------------------------------------*/
    ADC1_CR2 |= (1 << 22);


    /*-----------------------------------------------
     * Wait EOC
     *-----------------------------------------------*/
    while (!(ADC1_SR & (1 << 1)))
    {
    }


    /*-----------------------------------------------
     * Read ADC data
     *
     * 0    -> 0V
     * 4095 -> 3.3V
     *-----------------------------------------------*/
    return ADC1_DR & 0x0FFF;
}


/*========================================================
 * MAIN
 *========================================================*/
int main(void)
{
    unsigned int adc_value;
    unsigned int voltage_mv;


    /*====================================================
     * USART1
     *====================================================*/
    USART1_Init();

    USART1_SendString("STM32 ADC START\r\n");
    USART1_SendString("CPU/PCLK2 = 8MHz\r\n");
    USART1_SendString("USART1 = 9600 baud\r\n");
    USART1_SendString("ADC = PA3 / Channel 3\r\n");
    USART1_SendString("--------------------\r\n");


    /*====================================================
     * ADC1
     *====================================================*/
    ADC1_Init();

    USART1_SendString("ADC INIT OK\r\n");
    USART1_SendString("--------------------\r\n");


    /*====================================================
     * MAIN LOOP
     *====================================================*/
    while (1)
    {
        /*
         * Bao hieu bat dau doc ADC
         */
        USART1_SendString("READ ADC...\r\n");


        /*
         * Doc ADC PA3
         */
        adc_value = ADC1_Read();


        /*
         * In ADC
         */
        USART1_SendString("ADC = ");

        USART1_SendNumber(adc_value);

        USART1_SendString("\r\n");


        /*-----------------------------------------------
         * ADC -> Voltage
         *
         * Vref = 3.3V
         *
         * Voltage(mV)
         * = ADC * 3300 / 4095
         *-----------------------------------------------*/
        voltage_mv =
            (adc_value * 3300) / 4095;


        /*
         * In voltage
         */
        USART1_SendString("Voltage = ");


        /* Phan nguyen */
        USART1_SendNumber(voltage_mv / 1000);

        USART1_SendChar('.');


        /* Hang tram */
        if ((voltage_mv % 1000) < 100)
        {
            USART1_SendChar('0');
        }


        /* Hang chuc */
        if ((voltage_mv % 1000) < 10)
        {
            USART1_SendChar('0');
        }


        /* Hang don vi */
        USART1_SendNumber(voltage_mv % 1000);

        USART1_SendString(" V\r\n");


        USART1_SendString("--------------------\r\n");


        /* Doc lai sau 1 giay */
        delay_ms(1000);
    }
}
