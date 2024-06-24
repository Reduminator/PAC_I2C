/**
 *             |        MSP430FR2433        |
 *             |                            |
 *             |                            |
 *             |                            |        Código de comunicación I2C - Esclavo receptor
 *             |           P1.2 UCB0SDA <-->|
 *             |           P1.3 UCB0SCL <---|
 *             |                            |
 *             |                            |
 *             |        P1.0(Led rojo)  --->|
 *             |                            |
 */
#include <msp430.h> 

volatile unsigned char RXData;

int main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P1DIR |= BIT1;               // Declaramos como salida el pin P1.0
	
	// Configuración de los pines para la comunicación I2C
	P1SEL0 |= BIT2 | BIT3;      // Pines de I2C

	PM5CTL0 &= ~LOCKLPM5;        // Desactivamos la alta impedancia de los puertos

	// Configuración del USCI_B0 para el modo I2C
	    UCB0CTLW0 |= UCSWRST;                                 //Software reset habilitado
	    UCB0CTLW0 |= UCMODE_3;                                //Modo I2C, modo esclavo
	    UCB0I2COA0 = 0x0A | UCOAEN;                           //Escribimos nuestra dirección de esclavo y la habilitamos
	    UCB0CTLW0 &=~UCSWRST;                                 //Limpiamos el registro del reset

	    UCB0IE |=  UCRXIE0;                                   //Habilitamos la interrupción de recepción

	    while(1){
	        __bis_SR_register(LPM0_bits | GIE);                   //Entramos al modo de bajo consumo y habilitamos las interrupciones generales
	        __no_operation();
	    }
}


#pragma vector = USCI_B0_VECTOR
__interrupt void USCIB0_ISR(void){
   switch(__even_in_range(UCB0IV,USCI_I2C_UCBIT9IFG)){
      case USCI_NONE: break;                                   // Vector 0: No interrupts break;
      case USCI_I2C_UCALIFG: break;                                   // Vector 2: ALIFG break;
      case USCI_I2C_UCNACKIFG: break;                                   // Vector 4: NACKIFG break;
      case USCI_I2C_UCSTTIFG: break;                                   // Vector 6: STTIFG break;
      case USCI_I2C_UCSTPIFG: break;                                   // Vector 8: STPIFG break;
      case USCI_I2C_UCRXIFG3: break;                                   // Vector 10: RXIFG3 break;
      case USCI_I2C_UCTXIFG3: break;                                   // Vector 14: TXIFG3 break;
      case USCI_I2C_UCRXIFG2: break;                                   // Vector 16: RXIFG2 break;
      case USCI_I2C_UCTXIFG2: break;                                   // Vector 18: TXIFG2 break;
      case USCI_I2C_UCRXIFG1: break;                                   // Vector 20: RXIFG1 break;
      case USCI_I2C_UCTXIFG1: break;                                   // Vector 22: TXIFG1 break;
      case USCI_I2C_UCRXIFG0:                                          // Vector 24: RXIFG0 break;
          RXData = UCB0RXBUF;                     // Se obtiene el dato del buffer RX
          if(RXData == 0xAA){
              P1OUT |= BIT1;
          }
          else{
              P1OUT &= ~BIT1;
          }
          __bic_SR_register_on_exit(LPM0_bits);   // Exit LPM0
          break;
      case USCI_I2C_UCTXIFG0: break;                                   // Vector 26: TXIFG0 break;
      case USCI_I2C_UCBCNTIFG: break;                                   // Vector 28: BCNTIFG break;
      case USCI_I2C_UCCLTOIFG: break;                                   // Vector 30: clock low timeout break;
      case USCI_I2C_UCBIT9IFG: break;                                   // Vector 32: 9th bit break;
      default: break;
    }

}
