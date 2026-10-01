/*
 * BAI 01: STM32F103C8T6 GIAO TIEP LCD 1602 I2C
 *
 * CPU clock: HSI 8MHz
 * SCL: PB6
 * SDA: PB7
 * LCD I2C address: 0x27
 *
 * Khong dung HAL / STD library
 */

#define RCC_APB2ENR  (*(volatile unsigned int *)0x40021018)

#define GPIOB_CRL    (*(volatile unsigned int *)0x40010C00)
#define GPIOB_IDR    (*(volatile unsigned int *)0x40010C08)
#define GPIOB_ODR    (*(volatile unsigned int *)0x40010C0C)

#define I2C_SCL       6
#define I2C_SDA       7

#define LCD_ADDR      0x27

/* Bit dieu khien PCF8574 */
#define LCD_RS        0x01
#define LCD_RW        0x02
#define LCD_EN        0x04
#define LCD_BL        0x08


/* Tao tre tuong doi */
void delay_us(unsigned int us)
{
    volatile unsigned int i;

    while (us--)
    {
        for (i = 0; i < 2; i++)
        {
            __asm volatile ("nop");
        }
    }
}


void delay_ms(unsigned int ms)
{
    while (ms--)
    {
        delay_us(1000);
    }
}


/* Dieu khien hai duong I2C */
void SCL_Low(void)
{
    GPIOB_ODR &= ~(1U << I2C_SCL);
}


void SCL_High(void)
{
    GPIOB_ODR |= (1U << I2C_SCL);
}


void SDA_Low(void)
{
    GPIOB_ODR &= ~(1U << I2C_SDA);
}


void SDA_High(void)
{
    GPIOB_ODR |= (1U << I2C_SDA);
}


/* Khoi tao PB6, PB7 output open-drain */
void I2C_GPIO_Init(void)
{
    /* Bat clock GPIOB: APB2ENR bit 3 */
    RCC_APB2ENR |= (1U << 3);

    /*
     * PB6 va PB7:
     * MODE = 10: Output 2MHz
     * CNF  = 01: General purpose open-drain
     * Ma cau hinh moi chan = 0x6
     */
    GPIOB_CRL &= ~(0xFFU << 24);
    GPIOB_CRL |=  (0x66U << 24);

    /* Tha SCL va SDA len muc cao */
    SCL_High();
    SDA_High();

    delay_ms(10);
}


/* I2C START */
void I2C_Start(void)
{
    SDA_High();
    SCL_High();
    delay_us(5);

    SDA_Low();
    delay_us(5);

    SCL_Low();
}


/* I2C STOP */
void I2C_Stop(void)
{
    SDA_Low();
    delay_us(5);

    SCL_High();
    delay_us(5);

    SDA_High();
    delay_us(5);
}


/* Gui 1 byte, MSB truoc */
void I2C_WriteByte(unsigned char data)
{
    unsigned int i;

    for (i = 0; i < 8; i++)
    {
        if (data & 0x80)
        {
            SDA_High();
        }
        else
        {
            SDA_Low();
        }

        delay_us(3);

        SCL_High();
        delay_us(5);

        SCL_Low();
        delay_us(3);

        data <<= 1;
    }

    /*
     * Xung ACK:
     * Nha SDA de thiet bi slave phan hoi.
     * Ban co ban nay chua doc va kiem tra ACK.
     */
    SDA_High();
    delay_us(3);

    SCL_High();
    delay_us(5);

    SCL_Low();
    delay_us(3);
}


/* Ghi 1 byte ra module PCF8574 */
void PCF8574_Write(unsigned char data)
{
    I2C_Start();

    /* Dia chi 7-bit + bit Write = 0 */
    I2C_WriteByte((LCD_ADDR << 1) | 0);

    I2C_WriteByte(data | LCD_BL);

    I2C_Stop();
}


/* Tao xung Enable cho LCD */
void LCD_Enable(unsigned char data)
{
    PCF8574_Write(data | LCD_EN);
    delay_us(2);

    PCF8574_Write(data & (unsigned char)(~LCD_EN));
    delay_us(50);
}


/* Gui 4 bit du lieu */
void LCD_Write4Bits(unsigned char data)
{
    data |= LCD_BL;

    PCF8574_Write(data);
    LCD_Enable(data);
}


/* Gui 1 byte theo che do 4-bit */
void LCD_Send(unsigned char value, unsigned char mode)
{
    unsigned char high;
    unsigned char low;

    high = value & 0xF0;
    low  = (value << 4) & 0xF0;

    LCD_Write4Bits(high | mode);
    LCD_Write4Bits(low | mode);
}


/* Gui lenh */
void LCD_Command(unsigned char command)
{
    LCD_Send(command, 0);

    if (command == 0x01 || command == 0x02)
    {
        delay_ms(2);
    }
}


/* Gui ky tu */
void LCD_Data(unsigned char data)
{
    LCD_Send(data, LCD_RS);
}


/* Xoa man hinh */
void LCD_Clear(void)
{
    LCD_Command(0x01);
    delay_ms(2);
}


/* Dat vi tri con tro */
void LCD_SetCursor(unsigned char row, unsigned char col)
{
    unsigned char address;

    if (row == 0)
    {
        address = 0x00 + col;
    }
    else
    {
        address = 0x40 + col;
    }

    LCD_Command(0x80 | address);
}


/* Hien thi chuoi ky tu */
void LCD_Print(const char *str)
{
    while (*str)
    {
        LCD_Data((unsigned char)*str);
        str++;
    }
}


/* Khoi tao LCD 1602 */
void LCD_Init(void)
{
    delay_ms(50);

    /* Khoi tao LCD vao che do 4-bit */
    LCD_Write4Bits(0x30);
    delay_ms(5);

    LCD_Write4Bits(0x30);
    delay_us(150);

    LCD_Write4Bits(0x30);
    delay_us(150);

    LCD_Write4Bits(0x20);
    delay_us(150);

    /* 4-bit, 2 dong, font 5x8 */
    LCD_Command(0x28);

    /* Tat hien thi khi cau hinh */
    LCD_Command(0x08);

    /* Xoa man hinh */
    LCD_Clear();

    /* Tu dong tang con tro */
    LCD_Command(0x06);

    /* Bat hien thi, tat con tro */
    LCD_Command(0x0C);
}


/* Chuong trinh chinh */
int main(void)
{
    I2C_GPIO_Init();

    LCD_Init();

    LCD_SetCursor(0, 0);
    LCD_Print("STM32F103");

    LCD_SetCursor(1, 0);
    LCD_Print("I2C LCD: OK");

    while (1)
    {
        /* LCD giu nguyen noi dung */
    }
}
