#include "hardware_encoder.hpp"

static void alternate(GPIO_TypeDef *gpio, unsigned pin, unsigned af) {
    gpio->MODER = (gpio->MODER & ~(3UL << (pin * 2))) | (2UL << (pin * 2));
    gpio->PUPDR &= ~(3UL << (pin * 2));
    unsigned shift = (pin % 8) * 4;
    gpio->AFR[pin / 8] = (gpio->AFR[pin / 8] & ~(15UL << shift)) | (af << shift);
}

void rob::HardwareEncoder::setup() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN; //GPIOC,Aにクロック供給
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_TIM8EN; //TIM1,8にクロック供給
    volatile uint32_t ready = RCC->APB2ENR;
    (void)ready; //voidへの型変換をして値を使わないことを明示？
    if (tim == TIM8) {
        RCC->APB2RSTR |= RCC_APB2RSTR_TIM8RST; 
        RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM8RST; //タイマ設定の初期化
        alternate(GPIOC, 6, 3); alternate(GPIOC, 7, 3); //Alternate Function modeを使用し、該当するピンをTIM8に接続
    } else if (tim == TIM1) {
        RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST;
        RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST; //タイマ設定の初期化
        alternate(GPIOA, 8, 1); alternate(GPIOA, 9, 1); //Alternate Function modeを使用し、該当するピンをTIM1に接続
    } else return;
    tim->CR1 = 0; tim->CR2 = 0; tim->DIER = 0; //CR1=0でカウントの停止
	//CR2は使用しない DIER = 0で割り込みの無効化？(有効にしてなくてもハードが読み取るらし)
    tim->CCER = 0; tim->CCMR2 = 0; tim->PSC = 0; //CCER = 0でch1,ch2を無効化している psc:クロックのカウント周期変えれる ccmr:使用しない
    tim->ARR = 0xFFFF; tim->RCR = 0; //AAR:カウンタの上限(今回2^16-1) RCR:繰り返しカウンタ
    tim->CCMR1 = (1UL << 0) | (3UL << 4) | (1UL << 8) | (3UL << 12);
	/*
	| フィールド  |        設定の意味          |
	|-------------|----------------------------|
	| `CC1S=01`   | CH1を入力にして、TI1へ接続 |
	| `CC2S=01`   | CH2を入力にして、TI2へ接続 |
	| `IC1F=0011` | A相に入力フィルタを設定    |
	| `IC2F=0011` | B相に入力フィルタを設定    |
	*/
    tim->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E; //設定終わったからch1,ch2を有効化
    tim->SMCR = 3; // x4 quadrature; timer clock follows encoder edges マイコンがカウントを4倍にする これはエンコーダモード3
    tim->EGR = TIM_EGR_UG;
	tim->CNT = 0;
	tim->SR = 0; //EGR:更新イベント(つまり初期化) CNT=0でカウンタを0に初期化,SR=0で状態の初期化
    tim->CR1 = TIM_CR1_CEN; //カウントを開始
    reset();
}
