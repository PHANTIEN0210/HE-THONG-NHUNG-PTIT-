#define RCC_APB2ENR (*(volatile unsigned int *)0x40021018) // Bat thanh ghi cap xung nhip RCC APB2
#define GPIOA_CRL   (*(volatile unsigned int *)0x40010800) // Thanh ghi cau hinh 8 chan thap Port A (PA0-PA7)
#define GPIOA_CRH   (*(volatile unsigned int *)0x40010804) // Thanh ghi cau hinh 8 chan cao Port A (PA8-PA15)
#define GPIOA_IDR   (*(volatile unsigned int *)0x40010808) // Thanh ghi doc trang thai dau vao Port A
#define GPIOA_ODR   (*(volatile unsigned int *)0x4001080C) // Thanh ghi xuat dau ra Port A

int main(void) {
    RCC_APB2ENR |= (1 << 2); // Kich hoat xung nhip cho Port A (bit so 2)
    
    // Cau hinh PA0, PA1, PA2 la Input (dung che do Pull-up/Pull-down: 0x8 cho moi chan)
    // 3 chan dau thap CRL: PA0, PA1, PA2 tuong ung 12 bit dau (0x444 duoc, hoac 0x888 cho input pull)
    // De don gian va chac chan: Cau hinh PA0-PA2 la Input Floating (0x4) hoac Pull (0x8)
    GPIOA_CRL = 0x44444488; // PA0, PA1, PA2 cau hinh la Input, cac chan khac giu nguyen
    
    // Cau hinh PA8, PA9, PA10 la Output Push-Pull, toc do 2MHz (ma 0x2 cho moi chan trong CRH)
    // Moi chan trong CRH chiem 4 bit. PA8, PA9, PA10 tuong ung 12 bit dau cua CRH
    GPIOA_CRH &= 0xFFFF0000; // Xoa cau hinh cu cua PA8, PA9, PA10, PA11
    GPIOA_CRH |= 0x00002222; // Set PA8, PA9, PA10 la Output Push-Pull 2MHz
    
    while(1) {
        // Doc trang thai 3 nut nhan tu PA0, PA1, PA2 (lay 3 bit thap nhat)
        unsigned int input_val = GPIOA_IDR & 0x07; 
        
        // Dao trang thai 3 bit nay (0 thanh 1, 1 thanh 0) trong pham vi 3 bit thap
        unsigned int inverted_val = (~input_val) & 0x07; 
        
        // Ghi gia tri da dao ra 3 LED tai PA8, PA9, PA10 (dich trai 8 bit)
        // Giu nguyen cac bit khac cua ODR bang cach mask phan cao
        GPIOA_ODR = (GPIOA_ODR & 0xF8FF) | (inverted_val << 8); 
    }
}
