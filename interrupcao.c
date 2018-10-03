// CONFIG1H
#pragma config OSC = HS        // Oscillator Selection bits (HS oscillator (HS))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)
// CONFIG3H
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)
// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)
// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)
// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)
// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)
// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

#include <xc.h>

#define _XTAL_FREQ 4000000

unsigned short disp_1 = 0;
unsigned short disp_2 = 0;
unsigned short disp_3 = 0;
unsigned short disp_4 = 0;
int count = 0;
int stop = 0;

unsigned short mask(unsigned short num) {
    switch (num) {
        case 0 : return 0x3F;
        case 1 : return 0x06;
        case 2 : return 0x5B;
        case 3 : return 0x4F;
        case 4 : return 0x66;
        case 5 : return 0x6D;
        case 6 : return 0x7D;
        case 7 : return 0x07;
        case 8 : return 0x7F;
        case 9 : return 0x6F;
    }
}

void set_values_display(unsigned short disp1, unsigned short disp2, unsigned short disp3, unsigned short disp4){
    PORTD = mask(disp1);
    PORTAbits.RA2 = 1;
    __delay_ms(4);
    PORTAbits.RA2 = 0;
    
    PORTD = mask(disp2);
    PORTAbits.RA3 = 1;
    __delay_ms(4);
    PORTAbits.RA3 = 0;
    
    PORTD = mask(disp3);
    PORTAbits.RA4 = 1;
    __delay_ms(4);
    PORTAbits.RA4 = 0;
    
    PORTD = mask(disp4);
    PORTAbits.RA5 = 1;
    __delay_ms(4);
    PORTAbits.RA5 = 0;   
}

void set_incr(){
    disp_4 += 1;
    if(disp_4 == 10){
        disp_4 = 0;
        disp_3 += 1;
        if(disp_3 == 10){
            disp_3 = 0;
            disp_2 += 1;
            if(disp_2 == 10){
                disp_2 = 0;
                disp_1 += 1;
                if(disp_1 == 10){
                    disp_1 = disp_2 = disp_3 = disp_4 = 0;
                }
            }
        }
    }
}

void init_regs(){
    
    RCONbits.IPEN = 0; //desativando prioridades nas interrupções
    INTCONbits.GIE = 1; //ligando chave geral de interrupção
    INTCONbits.PEIE = 1; //ligando chave de interrupção para os dispositivos perifericos
    
    INTCON3bits.INT1IE = 1; //habilitando interrupção externa 1
    INTCON3bits.INT1IF = 0; //zerando o bit de sinalização de interrupção externa
    INTCON2bits.INTEDG1 = 1; //interrupção externa ocorrerá na borda de subida
    
    INTCONbits.TMR0IE = 1; // abilitando interrupção de estouro do timer
    INTCONbits.TMR0IF = 0; //limpando flag do timer
    
    T0CON = 0b11000111;/**bit 6 configurado para temporalizador
                        * bit 5 incrementado pelo ciclo de instrução
                        * bit 3 habilitação de prescaler
                        * bit 2-0 fator de prescaler 1:256
                        * ...
                        * obs: tempo de estouro sera de 0,16s
                        */
    TMR0L = 5;      //acionando o timer
    ADCON1 = 0x0F;  //configurando pinos das PORTA e PORTE como digitais
    PORTA  = 0;     //zerando PORTA
    PORTD  = 0;     //zerando PORTD
    TRISA  = 0;     //definido PORTA como saida
    TRISD  = 0;     //definido PORTD como saida
    
    //inicializando variaveis disp's
    disp_1 = 0;
    disp_2 = 0;
    disp_3 = 0;
    disp_4 = 0;
    count = 0;
}

void interrupt isr(void) {
    if(INTCON3bits.INT1IF) {
        INTCON3bits.INT1IF = 0; //Limpando a flag interrupção        
        stop == 0 ? stop ++ : stop--;
    } else if( INTCONbits.TMR0IF){
        if(stop == 0){
            if(count == 100){
                set_incr();
                count = 0;
            } else {
                count ++;
            }
        }
        TMR0L = 5;              //recarregando o timer
        INTCONbits.TMR0IF = 0;  // limpando flag de estouro
    }
}

void main() {
    init_regs();
    while (1) {
        set_values_display(disp_1, disp_2, disp_3, disp_4);
    }
}