//  Maestro tx.
//                                /|\  /|\
//                MSP430F5529     10k  10k     MSP430F5529
//                   esclavo       |    |         maestro
//             -----------------   |    |   -----------------
//           -|XIN  P3.0/UCB0SDA|<-|----+->|P3.0/UCB0SDA  XIN|-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |             XOUT|-
//            |     P3.1/UCB0SCL|<-+------>|P3.1/UCB0SCL     |
//            |                 |          |                 |
//******************************************************************************

#include <msp430.h>

/* Variables globales*/
unsigned char TXData;
unsigned char TXByteCtr;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Detenemos perro guardian
  P3SEL |= 0x03;                            // Asignamos 3.0 y 3.1 a USCI_B0
  UCB0CTL1 |= UCSWRST;                      // Reiniciamos maquina de estados de modulo
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C maestro en modo sincrono
  UCB0CTL1 = UCSSEL_1 + UCSWRST;            // SMCLK
  UCB0BR0 = 32;                             // fSCL = SMCLK/20
  UCB0BR1 = 0;
  UCB0I2CSA = 0x48;                         // Direccion del esclavo
  UCB0CTL1 &= ~UCSWRST;                     // Limpiamos reset de la maquina de estados
  UCB0IE |= UCTXIE;                         // Habilitamos interrupcion por TX

  TXData = 0x01;                            // Inicializamos varibale de transmision

  while (1)
  {
    TXByteCtr = 1;                          // Cargamos dato

    while (UCB0CTL1 & UCTXSTP);             // Aseguramos condicion de paro
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, Condicion de inicio

   //__bis_SR_register(LPM0_bits + GIE);     // LPM0 con interrupcion
   //__no_operation();                       //
    __delay_cycles(1000);

    TXData = 0x01;                               // Incrementamos dato
  }
}


// USCIAB0_ISR trasnmite cualquier dato cargado en TXByteCtr (Hoja de datos)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB0IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;                           // Vector  4: NACKIFG
  case  6: break;                           // Vector  6: STTIFG
  case  8: break;                           // Vector  8: STPIFG
  case 10: break;                           // Vector 10: RXIFG
  case 12:                                  // Vector 12: TXIFG
    if (TXByteCtr)                          // Check TX byte counter
    {
      UCB0TXBUF = TXData;                   // Cargamos buffer de TX
      TXByteCtr--;                          // Se decrementa contador
    }
    else
    {
      UCB0CTL1 |= UCTXSTP;                  // I2C en condicion de paro
      UCB0IFG &= ~UCTXIFG;                  // Limpiamos bandera de TX
      //__delay_cycles(1000);
      //__bic_SR_register_on_exit(LPM0_bits); // Sale de LPM0
    }
    break;
  default: break;
  }
}
