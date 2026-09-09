#define RCC_APB2ENR (*(volatile unsigned int *)0x40021018) // Bat thanh ghi cap xung nhip RCC APB2
#define GPIOA_CRL   (*(volatile unsigned int *)0x40010800) // Thanh ghi cau hinh cac chan thap Port A (PA0-PA7)
#define GPIOA_IDR   (*(volatile unsigned int *)0x40010808) // Thanh ghi doc trang thai dau vao cua Port A
#define GPIOA_ODR   (*(volatile unsigned int *)0x4001080C) // Thanh ghi xuat dau ra cua Port A

// Ham tao do tre (delay) chong rung phim va giu trang thai
void delay(volatile unsigned int count) {
    while(count--) {}
}

int main(void) {
    RCC_APB2ENR |= (1 << 2); // Kich hoạt xung nhip cho Port A (bit so 2)
    
    // Cau hinh PA0 la Input Pull-up/Pull-down, PA1 la Output Push-Pull (toc do 2MHz)
    GPIOA_CRL &= 0xFFFFFF00;   // Xoa cau hinh cu cua PA0 va PA1
    GPIOA_CRL |= 0x00000028;   // PA1 = 0x2 (Output), PA0 = 0x8 (Input Pull-up/Pull-down)
    GPIOA_ODR |= (1 << 0);     // Ghi 1 vao PA0 cua ODR de kich hoat dien tro keo len (Pull-up)
    
    int last_state = 1;        // Luu trang thai truoc do cua nut nhan (mac dinh keo len muc 1)
    int led_state = 0;         // Luu trang thai hien tai cua LED (0: tat, 1: sang)

    while(1) {
        // Doc trang thai hien tai cua nut nhan tai chan PA0
        int current_state = (GPIOA_IDR & (1 << 0)) ? 1 : 0;
        
        // Phat hien suon xuong roi suon len (nhan roi nha nut): trang thai chuyen tu 0 sang 1
        if (last_state == 0 && current_state == 1) {
            led_state = !led_state; // Dao trang thai LED
            
            // Cap nhat tin hieu dieu khiển LED ra chan PA1
            if (led_state) {
                GPIOA_ODR |= (1 << 1);  // Bat LED o PA1
            } else {
                GPIOA_ODR &= ~(1 << 1); // Tat LED o PA1
            }
        }
        
        last_state = current_state; // Cap nhat trang thai nut cho chu ky sau
        delay(30000);               // Thoi gian tre chong rung (debounce) cho nut nhan
    }
}
