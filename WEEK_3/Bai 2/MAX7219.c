#include "MAX7219.h"


/*
 * ============================================================
 * CHUONG TRINH CHINH - BAI 02
 * GIAO TIEP SPI GIUA STM32F103C8T6 VA MAX7219
 * ============================================================
 *
 * Muc dich:
 * - STM32F103C8T6 giao tiep voi IC MAX7219 thong qua SPI.
 * - MAX7219 dieu khien LED Matrix 8x8.
 * - Chuong trinh hien thi so "1" tren LED Matrix.
 *
 * Giao tiep SPI su dung:
 *
 *     PA5 -> SCK
 *     PA7 -> MOSI / DIN
 *     PA4 -> CS
 *
 * Trong bai nay:
 *
 *     STM32                 MAX7219
 *     --------------------------------
 *     PA5  --------------> CLK
 *     PA7  --------------> DIN
 *     PA4  --------------> CS
 *     5V   --------------> VCC
 *     GND  --------------> GND
 *
 * MAX7219 se nhan du lieu tu STM32 va tu duy tri
 * trang thai hien thi tren LED Matrix.
 */


/*
 * ============================================================
 * 1. HAM MAIN
 * ============================================================
 *
 * Sau khi STM32 khoi dong va Reset_Handler goi main(),
 * chuong trinh bat dau thuc hien tu day.
 */
int main(void)
{
    /*
     * ========================================================
     * BUOC 1: KHOI TAO MAX7219
     * ========================================================
     *
     * Ham MAX7219_Init() thuc hien hai cong viec chinh:
     *
     * 1. Khoi tao SPI1 cua STM32.
     *
     * 2. Cau hinh cac thanh ghi cua MAX7219.
     *
     * SPI1 duoc cau hinh de STM32 dong vai tro MASTER.
     *
     * Trong giao tiep SPI:
     *
     *     STM32 -> MAX7219
     *
     * STM32 tao xung Clock tren SCK va gui du lieu
     * tren duong MOSI.
     *
     * MAX7219 nhan du lieu thong qua chan DIN.
     */
    MAX7219_Init();


    /*
     * ========================================================
     * BUOC 2: HIEN THI SO 1
     * ========================================================
     *
     * LED Matrix co kich thuoc:
     *
     *             8 cot
     *             |
     *       +-----+-----+
     *       |           |
     *       |  LED 8x8  |
     *       |           |
     *       +-----------+
     *
     * Co 8 hang.
     *
     * MAX7219 co 8 thanh ghi hien thi:
     *
     *     Register 1 -> hang 1
     *     Register 2 -> hang 2
     *     Register 3 -> hang 3
     *     ...
     *     Register 8 -> hang 8
     *
     * Moi thanh ghi chua 8 bit.
     *
     * Moi bit tuong ung voi mot LED tren hang do.
     *
     * Quy uoc:
     *
     *     bit = 1 --> LED sang
     *     bit = 0 --> LED tat
     *
     * Do do, de ve mot ky tu tren LED Matrix,
     * ta can gui 8 byte tuong ung voi 8 hang.
     */


    /*
     * --------------------------------------------------------
     * HANG 1
     * --------------------------------------------------------
     *
     * 0x18 = 0001 1000
     *
     * Hai bit o giua duoc bat.
     */
    MAX7219_Write(1, 0x18);


    /*
     * --------------------------------------------------------
     * HANG 2
     * --------------------------------------------------------
     *
     * 0x38 = 0011 1000
     *
     * Tao phan dau cua so 1 rong hon.
     */
    MAX7219_Write(2, 0x38);


    /*
     * --------------------------------------------------------
     * HANG 3
     * --------------------------------------------------------
     *
     * 0x18 = 0001 1000
     *
     * Tiep tuc tao than cua so 1.
     */
    MAX7219_Write(3, 0x18);


    /*
     * --------------------------------------------------------
     * HANG 4
     * --------------------------------------------------------
     *
     * 0x18 = 0001 1000
     *
     * Tiep tuc tao than so 1.
     */
    MAX7219_Write(4, 0x18);


    /*
     * --------------------------------------------------------
     * HANG 5
     * --------------------------------------------------------
     *
     * 0x18 = 0001 1000
     *
     * Tiep tuc tao than so 1.
     */
    MAX7219_Write(5, 0x18);


    /*
     * --------------------------------------------------------
     * HANG 6
     * --------------------------------------------------------
     *
     * 0x18 = 0001 1000
     */
    MAX7219_Write(6, 0x18);


    /*
     * --------------------------------------------------------
     * HANG 7
     * --------------------------------------------------------
     *
     * 0x18 = 0001 1000
     */
    MAX7219_Write(7, 0x18);


    /*
     * --------------------------------------------------------
     * HANG 8
     * --------------------------------------------------------
     *
     * 0x7E = 0111 1110
     *
     * Bat 6 LED o hang cuoi.
     *
     * Hang nay tao phan chan cua so 1
     * theo mau hien thi duoc lap trinh.
     */
    MAX7219_Write(8, 0x7E);


    /*
     * ========================================================
     * BUOC 3: GIU CHUONG TRINH CHAY
     * ========================================================
     *
     * MAX7219 co bo mach quet va bo nho hien thi rieng.
     *
     * Sau khi STM32 gui xong 8 byte:
     *
     *     MAX7219 tu quet LED Matrix
     *     va duy tri trang thai LED.
     *
     * Vì v?y STM32 khong can phai gui lai 8 byte lien tuc.
     *
     * Neu thoat khoi main() thi chuong trinh se khong co
     * noi dung xu ly tiep theo.
     *
     * Do do ta dung vong lap vo han.
     */
    while (1)
    {
        /*
         * Khong can lam gi them.
         *
         * MAX7219 tu duy tri hien thi so 1.
         */
    }
}
