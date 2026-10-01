/*
 * ============================================================
 * BAI 01: STM32F103C8T6 GIAO TIEP LCD 1602 QUA I2C
 * ============================================================
 *
 * Muc tieu:
 * - STM32F103C8T6 giao tiep voi LCD 1602 thong qua module
 *   chuyen doi I2C PCF8574.
 * - Khong su dung HAL va khong su dung thu vien STD.
 * - Truc tiep thao tac voi cac thanh ghi cua STM32.
 *
 * So do ket noi:
 *
 * STM32F103C8T6       PCF8574 / LCD
 * ------------------------------------------------
 * PB6  -------------> SCL
 * PB7  -------------> SDA
 * GND  -------------> GND
 *
 * LCD I2C address = 0x27
 *
 * Clock:
 * - Sau khi reset, STM32 su dung HSI = 8MHz.
 * - Trong bai nay khong cau hinh PLL nen CPU van chay 8MHz.
 *
 * PB6:
 * - Dong vai tro SCL: duong xung clock cua I2C.
 *
 * PB7:
 * - Dong vai tro SDA: duong truyen du lieu cua I2C.
 *
 * Qua trinh hoat dong tong quat:
 *
 * main()
 *   |
 *   +--> I2C_GPIO_Init()
 *   |       |
 *   |       +--> Bat clock GPIOB
 *   |       +--> Cau hinh PB6, PB7 open-drain
 *   |
 *   +--> LCD_Init()
 *   |       |
 *   |       +--> Dua LCD vao che do 4-bit
 *   |       +--> Cau hinh LCD 2 dong, font 5x8
 *   |       +--> Xoa man hinh
 *   |       +--> Cau hinh con tro
 *   |
 *   +--> LCD_SetCursor()
 *   |
 *   +--> LCD_Print()
 *           |
 *           +--> LCD_Data()
 *                   |
 *                   +--> LCD_Send()
 *                           |
 *                           +--> I2C
 *
 * ============================================================
 */


/*
 * ============================================================
 * 1. KHAI BAO CAC THANH GHI
 * ============================================================
 *
 * STM32F103 khong dung thu vien stm32f10x.h trong bai nay.
 * Thay vao do, ta tu khai bao dia chi cac thanh ghi.
 *
 * Cu phap:
 *
 * (*(volatile unsigned int *)DIA_CHI)
 *
 * co nghia la:
 * - Ep DIA_CHI thanh con tro toi thanh ghi.
 * - volatile: bao cho compiler biet day la thanh ghi phan cung,
 *   moi lan doc/ghi deu phai thuc hien that.
 *
 * RCC_APB2ENR:
 * - Thanh ghi Enable Clock cho cac ngoai vi tren bus APB2.
 */
#define RCC_APB2ENR  (*(volatile unsigned int *)0x40021018)


/*
 * GPIOB_CRL:
 * - Cau hinh cac chan GPIOB tu PB0 den PB7.
 *
 * GPIOB_IDR:
 * - Thanh ghi Input Data Register.
 * - Dung de doc muc logic tai cac chan GPIOB.
 *
 * GPIOB_ODR:
 * - Thanh ghi Output Data Register.
 * - Dung de dat muc logic dau ra GPIOB.
 */
#define GPIOB_CRL    (*(volatile unsigned int *)0x40010C00)
#define GPIOB_IDR    (*(volatile unsigned int *)0x40010C08)
#define GPIOB_ODR    (*(volatile unsigned int *)0x40010C0C)


/*
 * ============================================================
 * 2. XAC DINH CHAN I2C
 * ============================================================
 *
 * PB6 = SCL
 * PB7 = SDA
 *
 * SCL:
 * - Serial Clock.
 * - Tao xung clock dong bo viec truyen du lieu.
 *
 * SDA:
 * - Serial Data.
 * - Duong truyen bit du lieu.
 */
#define I2C_SCL       6
#define I2C_SDA       7


/*
 * Dia chi I2C 7-bit cua module PCF8574.
 *
 * LCD 1602 khong giao tiep I2C truc tiep.
 * Module PCF8574 dung de chuyen:
 *
 *     I2C <--> GPIO song song cua LCD
 *
 * Dia chi 0x27 la dia chi cua module trong bai nay.
 */
#define LCD_ADDR      0x27


/*
 * ============================================================
 * 3. CAC BIT DIEU KHIEN PCF8574
 * ============================================================
 *
 * PCF8574 co 8 bit dau ra.
 * Cac bit nay duoc dung de dieu khien LCD.
 *
 * Bit 0 - RS:
 * - RS = 0: gui lenh cho LCD.
 * - RS = 1: gui du lieu/ky tu cho LCD.
 *
 * Bit 1 - RW:
 * - RW = 0: ghi vao LCD.
 * - RW = 1: doc tu LCD.
 * - Bai nay chi ghi nen RW luon o muc 0.
 *
 * Bit 2 - EN:
 * - Enable.
 * - Tao xung de LCD chot 4 bit du lieu dang gui.
 *
 * Bit 3 - BL:
 * - Backlight.
 * - Dieu khien den nen LCD.
 */
#define LCD_RS        0x01
#define LCD_RW        0x02
#define LCD_EN        0x04
#define LCD_BL        0x08


/*
 * ============================================================
 * 4. HAM DELAY MICRO GIAY
 * ============================================================
 *
 * Ham nay tao tre tuong doi.
 *
 * volatile:
 * - Bien i khong bi compiler toi uu bo qua.
 *
 * nop:
 * - No Operation.
 * - CPU thuc hien mot lenh nhung khong thay doi du lieu.
 *
 * Muc dich:
 * - Tao khoang thoi gian giua cac thay doi SDA/SCL.
 * - Giup tin hieu I2C co thoi gian on dinh.
 *
 * Luu y:
 * - Day khong phai bo dinh thoi chinh xac.
 * - Thoi gian phu thuoc vao tan so CPU va compiler.
 */
void delay_us(unsigned int us)
{
    volatile unsigned int i;

    while (us--)
    {
        /*
         * Lap lenh NOP de tao mot khoang tre nho.
         */
        for (i = 0; i < 2; i++)
        {
            __asm volatile ("nop");
        }
    }
}


/*
 * Tao delay theo don vi mili giay.
 *
 * 1 ms = 1000 us
 *
 * Vi vay moi lan lap ta goi delay_us(1000).
 */
void delay_ms(unsigned int ms)
{
    while (ms--)
    {
        delay_us(1000);
    }
}


/*
 * ============================================================
 * 5. DIEU KHIEN DUONG SCL
 * ============================================================
 *
 * I2C su dung 2 duong:
 *
 *     SDA: Data
 *     SCL: Clock
 *
 * Cac chan I2C duoc cau hinh open-drain.
 *
 * Trong open-drain:
 * - Keo xuong LOW: GPIO co the chu dong keo duong tin hieu
 *   xuong 0.
 *
 * - Dua len HIGH: GPIO "tha" duong bus.
 *   Dien tro pull-up se dua bus len muc 1.
 *
 * Day la dac diem quan trong cua giao tiep I2C.
 */


/*
 * Keo SCL xuong muc LOW.
 */
void SCL_Low(void)
{
    GPIOB_ODR &= ~(1U << I2C_SCL);
}


/*
 * Tha SCL de bus tro ve muc HIGH.
 *
 * Vi PB6 la open-drain nen ghi 1 co the hieu la
 * tha duong bus.
 */
void SCL_High(void)
{
    GPIOB_ODR |= (1U << I2C_SCL);
}


/*
 * Keo SDA xuong muc LOW.
 */
void SDA_Low(void)
{
    GPIOB_ODR &= ~(1U << I2C_SDA);
}


/*
 * Tha SDA len muc HIGH.
 */
void SDA_High(void)
{
    GPIOB_ODR |= (1U << I2C_SDA);
}


/*
 * ============================================================
 * 6. KHOI TAO GPIO CHO I2C
 * ============================================================
 *
 * Muc dich:
 * - Bat clock cho GPIOB.
 * - Cau hinh PB6 va PB7.
 * - Dua SCL va SDA ve trang thai HIGH ban dau.
 */
void I2C_GPIO_Init(void)
{
    /*
     * RCC_APB2ENR bit 3 = IOPBEN.
     *
     * Dat bit 3 = 1
     * --> cap clock cho GPIOB.
     *
     * Neu khong bat clock GPIOB thi viec cau hinh PB6/PB7
     * se khong hoat dong.
     */
    RCC_APB2ENR |= (1U << 3);


    /*
     * --------------------------------------------------------
     * Cau hinh PB6 va PB7 trong GPIOB_CRL
     * --------------------------------------------------------
     *
     * GPIOB_CRL cau hinh PB0 -> PB7.
     *
     * Moi GPIO chiem 4 bit cau hinh:
     *
     *   bit [1:0] = MODE
     *   bit [3:2] = CNF
     *
     * PB6 nam tai bit 24 -> 27.
     * PB7 nam tai bit 28 -> 31.
     *
     * Ta dat:
     *
     * MODE = 10
     * --> Output 2MHz
     *
     * CNF = 01
     * --> General Purpose Open-Drain
     *
     * Ma cau hinh:
     *
     * CNF = 01
     * MODE = 10
     *
     * --> 0110 = 0x6
     *
     * Do PB6 va PB7 deu co cau hinh 0x6:
     *
     * PB7 = 0x6
     * PB6 = 0x6
     *
     * Ghe´p lai thanh:
     *
     *     0x66
     *
     * Dich sang bit 24:
     *
     *     0x66 << 24
     */
    GPIOB_CRL &= ~(0xFFU << 24);
    GPIOB_CRL |=  (0x66U << 24);


    /*
     * Dua ca SCL va SDA len HIGH.
     *
     * Day la trang thai nghi cua bus I2C.
     *
     * I2C START chi xuat hien khi:
     *
     *     SDA: HIGH -> LOW
     *     trong khi SCL = HIGH.
     */
    SCL_High();
    SDA_High();


    /*
     * Cho 10ms sau khi khoi tao GPIO
     * de bus va module LCD on dinh.
     */
    delay_ms(10);
}


/*
 * ============================================================
 * 7. TAO DIEU KIEN START CUA I2C
 * ============================================================
 *
 * START la tin hieu bao cho slave biet:
 *
 * "Master bat dau mot lan truyen moi."
 *
 * Trinh tu:
 *
 * 1. SDA = HIGH
 * 2. SCL = HIGH
 * 3. SDA chuyen HIGH -> LOW
 *    trong khi SCL van HIGH
 * 4. Keo SCL xuong LOW
 *
 * Diem dac trung cua START:
 *
 *       SCL = HIGH
 *       SDA: HIGH -> LOW
 */
void I2C_Start(void)
{
    /*
     * Dua bus ve trang thai nghi:
     * SCL = HIGH
     * SDA = HIGH
     */
    SDA_High();
    SCL_High();

    delay_us(5);


    /*
     * SDA chuyen tu HIGH xuong LOW
     * trong khi SCL dang HIGH.
     *
     * Day chinh la dieu kien START.
     */
    SDA_Low();

    delay_us(5);


    /*
     * Sau khi tao START,
     * keo SCL xuong LOW de bat dau gui bit.
     */
    SCL_Low();
}


/*
 * ============================================================
 * 8. TAO DIEU KIEN STOP CUA I2C
 * ============================================================
 *
 * STOP bao cho slave biet:
 *
 * "Ket thuc lan truyen."
 *
 * Trinh tu:
 *
 * 1. SDA = LOW
 * 2. Dua SCL len HIGH
 * 3. SDA chuyen LOW -> HIGH
 *    trong khi SCL dang HIGH
 *
 * Diem dac trung:
 *
 *       SCL = HIGH
 *       SDA: LOW -> HIGH
 */
void I2C_Stop(void)
{
    SDA_Low();
    delay_us(5);


    /*
     * Dua SCL len HIGH truoc.
     */
    SCL_High();
    delay_us(5);


    /*
     * SDA chuyen LOW -> HIGH
     * trong khi SCL dang HIGH.
     *
     * Day la dieu kien STOP.
     */
    SDA_High();
    delay_us(5);
}


/*
 * ============================================================
 * 9. GUI 1 BYTE QUA I2C
 * ============================================================
 *
 * I2C truyen du lieu theo tung bit.
 *
 * Moi byte co 8 bit.
 *
 * Bai nay truyen:
 *
 *     MSB truoc
 *
 * Vi du:
 *
 *     0x27 = 0010 0111
 *
 * Bit dau tien duoc gui la bit 7.
 *
 * Sau moi bit:
 *
 *     SCL HIGH -> slave doc bit
 *     SCL LOW  -> chuan bi bit tiep theo
 */
void I2C_WriteByte(unsigned char data)
{
    unsigned int i;

    /*
     * Lap 8 lan de gui du 8 bit.
     */
    for (i = 0; i < 8; i++)
    {
        /*
         * Kiem tra bit cao nhat bit 7.
         *
         * Neu bit 7 = 1:
         * --> SDA = HIGH
         *
         * Neu bit 7 = 0:
         * --> SDA = LOW
         */
        if (data & 0x80)
        {
            SDA_High();
        }
        else
        {
            SDA_Low();
        }


        /*
         * Cho SDA on dinh truoc khi tao xung clock.
         */
        delay_us(3);


        /*
         * Dua SCL len HIGH.
         *
         * Slave se doc gia tri SDA trong khoang SCL HIGH.
         */
        SCL_High();
        delay_us(5);


        /*
         * Dua SCL xuong LOW.
         *
         * Ket thuc viec truyen bit hien tai.
         */
        SCL_Low();
        delay_us(3);


        /*
         * Dich trai 1 bit.
         *
         * Bit tiep theo se duoc dua vao vi tri bit 7.
         *
         * Vi du:
         *
         * 10110010
         *    <<
         *
         * 01100100
         */
        data <<= 1;
    }


    /*
     * --------------------------------------------------------
     * Xung ACK
     * --------------------------------------------------------
     *
     * Sau khi master gui 8 bit,
     * slave se co co hoi phan hoi ACK.
     *
     * Master phai tha SDA.
     *
     * Neu slave muon ACK:
     *     slave keo SDA = LOW
     *
     * Sau do master tao them mot xung SCL.
     *
     * Trong code nay:
     * - Ta tha SDA.
     * - Tao xung SCL.
     * - NHUNG khong doc GPIOB_IDR de kiem tra ACK.
     *
     * Vi vay day la truyen I2C co ACK ve mat giao thuc,
     * nhung chuong trinh chua xu ly ket qua ACK.
     */
    SDA_High();
    delay_us(3);

    SCL_High();
    delay_us(5);

    SCL_Low();
    delay_us(3);
}


/*
 * ============================================================
 * 10. GHI DU LIEU VAO PCF8574
 * ============================================================
 *
 * PCF8574 la IC chuyen doi:
 *
 *     I2C --> 8 chan GPIO
 *
 * LCD 1602 dung giao tiep song song 4-bit,
 * vi vay PCF8574 dung 8 bit cua no de dieu khien LCD.
 *
 * Trinh tu:
 *
 * START
 *   |
 *   +--> Dia chi slave + Write
 *   |
 *   +--> Du lieu
 *   |
 * STOP
 */
void PCF8574_Write(unsigned char data)
{
    /*
     * Bat dau giao tiep I2C.
     */
    I2C_Start();


    /*
     * Dia chi I2C co dang 7 bit.
     *
     * LCD_ADDR = 0x27
     *
     * Dich trai 1 bit:
     *
     *     0x27 << 1
     *
     * Bit 0 la bit R/W.
     *
     * R/W = 0 --> Write.
     *
     * Do do:
     *
     *     (LCD_ADDR << 1) | 0
     *
     * la dia chi slave kem lenh ghi.
     */
    I2C_WriteByte((LCD_ADDR << 1) | 0);


    /*
     * Gui du lieu ra PCF8574.
     *
     * data | LCD_BL:
     * - Dam bao bit BL = 1
     * - Giup bat den nen LCD.
     */
    I2C_WriteByte(data | LCD_BL);


    /*
     * Ket thuc giao tiep I2C.
     */
    I2C_Stop();
}


/*
 * ============================================================
 * 11. TAO XUNG ENABLE CHO LCD
 * ============================================================
 *
 * LCD 1602 su dung chan EN de chot du lieu.
 *
 * Khi gui 4 bit:
 *
 *     1. Dat du lieu len cac chan D4-D7.
 *     2. EN = HIGH.
 *     3. EN = LOW.
 *
 * LCD chot du lieu tai xung Enable.
 *
 * Vi vay ham nay tao mot xung:
 *
 *       EN: LOW -> HIGH -> LOW
 */
void LCD_Enable(unsigned char data)
{
    /*
     * Dua EN len HIGH.
     *
     * LCD nhan biet day la thoi diem
     * chot 4 bit du lieu.
     */
    PCF8574_Write(data | LCD_EN);

    delay_us(2);


    /*
     * Dua EN ve LOW.
     *
     * Ket thuc xung Enable.
     */
    PCF8574_Write(data & (unsigned char)(~LCD_EN));

    /*
     * Cho LCD xu ly du lieu vua nhan.
     */
    delay_us(50);
}


/*
 * ============================================================
 * 12. GUI 4 BIT DU LIEU CHO LCD
 * ============================================================
 *
 * LCD dang hoat dong o che do 4-bit.
 *
 * Vi vay moi lan gui chi gui 4 bit:
 *
 *     D4 D5 D6 D7
 *
 * data:
 *     4 bit cao duoc dat vao vi tri D4-D7.
 *
 * Sau khi dat du lieu:
 *     tao xung EN de LCD chot du lieu.
 */
void LCD_Write4Bits(unsigned char data)
{
    /*
     * Bat den nen LCD.
     */
    data |= LCD_BL;


    /*
     * Gui 4 bit du lieu thong qua PCF8574.
     */
    PCF8574_Write(data);


    /*
     * Tao xung Enable de LCD chot 4 bit.
     */
    LCD_Enable(data);
}


/*
 * ============================================================
 * 13. GUI 1 BYTE THEO CHE DO 4-BIT
 * ============================================================
 *
 * LCD nhan moi byte gom 8 bit.
 *
 * Nhung bus LCD dang o che do 4-bit.
 *
 * Do do phai chia byte thanh:
 *
 *     4 bit cao
 *     4 bit thap
 *
 * Vi du:
 *
 *     value = 0x41
 *
 *     0x41 = 0100 0001
 *
 * 4 bit cao:
 *
 *     0100
 *
 * 4 bit thap:
 *
 *     0001
 *
 * Ta gui 2 lan:
 *
 *     Lan 1 --> 0100
 *     Lan 2 --> 0001
 */
void LCD_Send(unsigned char value, unsigned char mode)
{
    unsigned char high;
    unsigned char low;


    /*
     * Lay 4 bit cao cua value.
     *
     * 0xF0 = 1111 0000
     *
     * Phep AND giu lai 4 bit cao.
     */
    high = value & 0xF0;


    /*
     * Lay 4 bit thap va dua len 4 bit cao.
     *
     * Vi du:
     *
     * value = xxxx abcd
     *
     * value << 4
     *
     * --> abcd 0000
     *
     * Sau do & 0xF0 de giu 4 bit can gui.
     */
    low  = (value << 4) & 0xF0;


    /*
     * Gui nibble cao truoc.
     *
     * mode:
     *     0    --> lenh
     *     LCD_RS --> du lieu/ky tu
     */
    LCD_Write4Bits(high | mode);


    /*
     * Gui nibble thap sau.
     */
    LCD_Write4Bits(low | mode);
}


/*
 * ============================================================
 * 14. GUI LENH CHO LCD
 * ============================================================
 *
 * Khi RS = 0:
 * LCD hieu byte nhan duoc la LENH.
 *
 * Vi du:
 *
 *     0x01 --> Clear Display
 *     0x02 --> Return Home
 *     0x28 --> 4-bit, 2 dong
 *     0x0C --> Bat hien thi
 */
void LCD_Command(unsigned char command)
{
    /*
     * Gui command voi mode = 0.
     *
     * mode = 0
     * --> RS = 0
     * --> LCD hieu day la lenh.
     */
    LCD_Send(command, 0);


    /*
     * Hai lenh nay can thoi gian xu ly lau hon
     * cac lenh thong thuong.
     *
     * 0x01 = Clear Display
     * 0x02 = Return Home
     */
    if (command == 0x01 || command == 0x02)
    {
        delay_ms(2);
    }
}


/*
 * ============================================================
 * 15. GUI KY TU CHO LCD
 * ============================================================
 *
 * Khi RS = 1:
 * LCD hieu byte nhan duoc la DU LIEU/KY TU.
 *
 * Vi du:
 *
 *     LCD_Data('A')
 *
 * se hien ky tu A tren man hinh.
 */
void LCD_Data(unsigned char data)
{
    /*
     * LCD_RS = 1
     * --> LCD nhan du lieu hien thi.
     */
    LCD_Send(data, LCD_RS);
}


/*
 * ============================================================
 * 16. XOA MAN HINH LCD
 * ============================================================
 *
 * Lenh:
 *
 *     0x01 = Clear Display
 *
 * LCD se xoa noi dung hien tai
 * va dua con tro ve vi tri dau.
 */
void LCD_Clear(void)
{
    LCD_Command(0x01);

    /*
     * Cho LCD hoan thanh lenh xoa.
     */
    delay_ms(2);
}


/*
 * ============================================================
 * 17. DAT VI TRI CON TRO
 * ============================================================
 *
 * LCD 1602 co:
 *
 *     Dong 0: dia chi bat dau 0x00
 *     Dong 1: dia chi bat dau 0x40
 *
 * Vi du:
 *
 *     row = 0, col = 0
 *     --> dia chi = 0x00
 *
 *     row = 1, col = 0
 *     --> dia chi = 0x40
 *
 * De dat DDRAM address cua LCD:
 *
 *     command = 0x80 | address
 *
 * Bit 7 = 1 cho biet day la lenh Set DDRAM Address.
 */
void LCD_SetCursor(unsigned char row, unsigned char col)
{
    unsigned char address;


    /*
     * Neu la dong 0:
     *
     *     address = 0x00 + col
     */
    if (row == 0)
    {
        address = 0x00 + col;
    }
    else
    {
        /*
         * Neu la dong 1:
         *
         *     address = 0x40 + col
         */
        address = 0x40 + col;
    }


    /*
     * 0x80 | address:
     *
     * tao lenh Set DDRAM Address.
     *
     * Sau lenh nay LCD se hien ky tu tiep theo
     * tai vi tri vua chon.
     */
    LCD_Command(0x80 | address);
}


/*
 * ============================================================
 * 18. HIEN THI MOT CHUOI KY TU
 * ============================================================
 *
 * Ham nhan vao con tro str tro den chuoi ky tu.
 *
 * Vi du:
 *
 *     LCD_Print("STM32F103");
 *
 * Chuong trinh se:
 *
 *     S -> LCD_Data()
 *     T -> LCD_Data()
 *     M -> LCD_Data()
 *     3 -> LCD_Data()
 *     ...
 *
 * Ky tu ket thuc chuoi C la:
 *
 *     '\0'
 *
 * Khi gap '\0' thi dung.
 */
void LCD_Print(const char *str)
{
    /*
     * Lap cho den khi gap ky tu ket thuc chuoi.
     */
    while (*str)
    {
        /*
         * Gui tung ky tu vao LCD.
         */
        LCD_Data((unsigned char)*str);

        /*
         * Tang con tro de tro sang ky tu tiep theo.
         */
        str++;
    }
}


/*
 * ============================================================
 * 19. KHOI TAO LCD 1602
 * ============================================================
 *
 * Day la ham rat quan trong.
 *
 * Sau khi cap nguon, LCD chua ngay lap tuc vao che do
 * 4-bit ma phai gui chuoi lenh khoi tao.
 *
 * Trinh tu:
 *
 * 1. Cho LCD khoi dong.
 * 2. Gui 0x30 ba lan.
 * 3. Gui 0x20 de chuyen sang 4-bit.
 * 4. Cau hinh LCD.
 * 5. Tat hien thi trong luc cau hinh.
 * 6. Xoa man hinh.
 * 7. Cau hinh con tro tu dong tang.
 * 8. Bat hien thi.
 */
void LCD_Init(void)
{
    /*
     * Cho LCD khoi dong sau khi cap nguon.
     */
    delay_ms(50);


    /*
     * --------------------------------------------------------
     * Buoc 1: Gui 0x30
     * --------------------------------------------------------
     *
     * Muc dich:
     * Dua LCD ve trang thai giao tiep 8-bit ban dau.
     */
    LCD_Write4Bits(0x30);
    delay_ms(5);


    /*
     * Gui 0x30 lan thu hai.
     */
    LCD_Write4Bits(0x30);
    delay_us(150);


    /*
     * Gui 0x30 lan thu ba.
     */
    LCD_Write4Bits(0x30);
    delay_us(150);


    /*
     * --------------------------------------------------------
     * Buoc 2: Chuyen LCD sang giao tiep 4-bit
     * --------------------------------------------------------
     *
     * Gui 0x20.
     *
     * Sau buoc nay LCD hoat dong theo che do 4-bit.
     *
     * Tu day moi byte se duoc chia thanh:
     *
     *     nibble cao
     *     nibble thap
     */
    LCD_Write4Bits(0x20);
    delay_us(150);


    /*
     * --------------------------------------------------------
     * Buoc 3: Function Set
     * --------------------------------------------------------
     *
     * 0x28:
     *
     *     4-bit interface
     *     2 dong
     *     font 5x8
     */
    LCD_Command(0x28);


    /*
     * --------------------------------------------------------
     * Buoc 4: Tat hien thi trong luc cau hinh
     * --------------------------------------------------------
     *
     * 0x08:
     *     Display OFF
     *     Cursor OFF
     *     Blink OFF
     */
    LCD_Command(0x08);


    /*
     * --------------------------------------------------------
     * Buoc 5: Xoa man hinh
     * --------------------------------------------------------
     *
     * 0x01 = Clear Display
     */
    LCD_Clear();


    /*
     * --------------------------------------------------------
     * Buoc 6: Entry Mode Set
     * --------------------------------------------------------
     *
     * 0x06:
     * - Con tro tu dong tang sau moi ky tu.
     * - Khong dich toan bo man hinh.
     *
     * Vi du:
     *
     * Viet A tai cot 0
     * --> con tro chuyen sang cot 1.
     */
    LCD_Command(0x06);


    /*
     * --------------------------------------------------------
     * Buoc 7: Display Control
     * --------------------------------------------------------
     *
     * 0x0C:
     * - Bat LCD.
     * - Tat con tro.
     * - Tat blink.
     */
    LCD_Command(0x0C);
}


/*
 * ============================================================
 * 20. CHUONG TRINH CHINH
 * ============================================================
 *
 * Day la noi chuong trinh bat dau chay sau Reset_Handler.
 *
 * Trinh tu:
 *
 *     I2C_GPIO_Init()
 *          |
 *          +--> Cau hinh PB6/PB7
 *
 *     LCD_Init()
 *          |
 *          +--> Khoi tao LCD 1602
 *
 *     LCD_SetCursor()
 *          |
 *          +--> Chon vi tri viet
 *
 *     LCD_Print()
 *          |
 *          +--> Gui chuoi qua I2C
 *
 */
int main(void)
{
    /*
     * --------------------------------------------------------
     * Buoc 1: Khoi tao GPIO cho giao tiep I2C
     * --------------------------------------------------------
     *
     * Sau lenh nay:
     *
     *     PB6 = SCL
     *     PB7 = SDA
     *
     * ca hai o che do open-drain.
     */
    I2C_GPIO_Init();


    /*
     * --------------------------------------------------------
     * Buoc 2: Khoi tao LCD
     * --------------------------------------------------------
     *
     * Dua LCD vao:
     *
     *     4-bit
     *     2 dong
     *     font 5x8
     *     display ON
     *     cursor OFF
     */
    LCD_Init();


    /*
     * --------------------------------------------------------
     * Buoc 3: Dat con tro dong 0, cot 0
     * --------------------------------------------------------
     *
     * Dong dau tien, vi tri dau tien.
     */
    LCD_SetCursor(0, 0);


    /*
     * Hien thi chuoi:
     *
     *     STM32F103
     *
     * Tren dong 1 cua LCD.
     */
    LCD_Print("STM32F103");


    /*
     * --------------------------------------------------------
     * Buoc 4: Dat con tro dong 1, cot 0
     * --------------------------------------------------------
     *
     * Chuyen xuong dong thu hai.
     */
    LCD_SetCursor(1, 0);


    /*
     * Hien thi:
     *
     *     I2C LCD: OK
     *
     * Tren dong thu hai.
     */
    LCD_Print("I2C LCD: OK");


    /*
     * --------------------------------------------------------
     * Vong lap vo han
     * --------------------------------------------------------
     *
     * Sau khi hien thi xong, khong can gui lai du lieu.
     *
     * LCD tu giu noi dung tren man hinh.
     *
     * Vi vay MCU chi can dung tai day.
     */
    while (1)
    {
        /* LCD giu nguyen noi dung */
    }
}
