#define RCC_APB2ENR (*(volatile unsigned int *)0x40021018) // Dinh nghia dia chi thanh ghi cap xung nhip RCC APB2
#define GPIOA_CRL   (*(volatile unsigned int *)0x40010800) // Dinh nghia thanh ghi cau hinh 8 chan thap cua Port A (PA0-PA7)
#define GPIOA_ODR   (*(volatile unsigned int *)0x4001080C) // Dinh nghia thanh ghi xuat du lieu ra cac chan Port A

// Ham tao do tre (delay) don gian bang vong lap giam dan
void delay(volatile unsigned int count) {
    while(count--) {}
}

int main(void) {
    RCC_APB2ENR |= (1 << 2); // Bat xung nhip (clock) cho Port A (nam o bit so 2 cua thanh ghi)
    GPIOA_CRL = 0x22222222;  // Cau hinh toan bo 8 chan PA0-PA7 thanh che do Output Push-Pull, toc do 2MHz
    
    while(1) {
        // Vong lap 1: Cho LED sang chay tu trai sang phai (tu PA0 den PA7)
        for (int i = 0; i < 8; i++) {
            GPIOA_ODR = (1 << i); // Dich bit 1 sang trai theo bien i de bat tung chan LED
            delay(500000);        // Goi ham delay de giu trang thai LED trong mot khoang thoi gian
        }
        
        // Vong lap 2: Cho LED sang chay nguoc lai tu phai sang trai (tu PA6 ve PA1)
        for (int i = 6; i > 0; i--) {
            GPIOA_ODR = (1 << i); // Dich bit 1 giam dan de chuyen huong LED chay nguoc lai
            delay(500000);        // Goi ham delay de tao hieu quan sat ro rang
        }
    }
}
