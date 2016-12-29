/*
 * lora_tx_3a.cpp
 *
 * Created: 23.11.2016 15:52:23
 * Author : Muhammed
 */ 

 #include <avr/io.h>
 #include <stdio.h>
 #include <stdlib.h>
 #define F_CPU 1000000UL
 #include <string.h>
 #include <util/delay.h>
 #include <avr/interrupt.h>
 #include <avr/sleep.h>
 #define BAUD 9600
 #define MYUBRR F_CPU/16/BAUD-1
 #include <kutub/spi_uart_methods_atmega8_for_gps.h>
 #include <math.h>
 #include "D:\Çalýþmalarým\Tracker dosyalarý\LORA\LORA.h"
 #define XOSC 32000000
 #define BAUD 9600
 #define Veriboyutubiti 7		// 7-bit data length
 #define Asenkron2mislihiz 0		// asynchronous operation (normal speed)
 #define Paritciftyadatek yok	//  Parity bit - none
 #define Durdurmabiti 1			// stop bit as selected 1


 #define TX_tampon_boyutu 128


 #define bufferSize 10
 #define logic1(port,pin) port |= (1 << pin)
 #define logic0(port,pin) port &= ~(1 << pin)

 /*Queue Information*/
 char seritampon[TX_tampon_boyutu];
 uint8_t seriokuPoz = 0;
 uint8_t seriyazPoz = 0;

 void Seriyibirlestir(char c);
 void seriyaz(char  c[]);
 void ele(unsigned char *bnum);
 void gps(unsigned char []);
 void USART_Init(unsigned int);
 void USART_Transmit(unsigned char);
 unsigned char USART_Receive(void);
 /*Queue Information*/


 void verici_modu(void);
 uint8_t SPIregister_oku(uint8_t);
 uint8_t SPIregister_yaz(uint8_t ,uint8_t );
 void SPIpilotsinyali_yaz(unsigned char adres,unsigned char *ptr,unsigned char len);
 void SPIpilotsinyali_oku(unsigned char adres,unsigned char *ptr,unsigned char len);

 void reset_sx1276(void);
 void Config_SX1276(void);
 void verici_modu(void);
 void init_rx(void);
 bool setFrequency(unsigned long);


 // digital pinleri tanýmla
 #define MOSI   PORTB
 #define MISO   PORTB
 #define SCK    PORTB
 #define SS	   PORTB

 #define NRESET PORTD
 #define TXENT  PORTD
 #define RXENR  PORTD
 #define led1   PORTC
 #define led2   PORTC

 #define MOSII   3
 #define MISOO   4
 #define SCKK    5
 #define SSS	 2

 #define NRESETT 7
 #define TXENN   3
 #define RXENN   2
 #define led11   4
 #define led22   5

 // Register Tanýmlarý

 #define LR_RegFifo                       0x00
 #define LR_RegOpMode                     0x01
 #define LR_RegBitrateMsb                 0x02
 #define LR_RegBitrateLsb                 0x03
 #define LR_RegFdevMsb                    0x04
 #define LR_RegFdMsb                      0x05
 #define LR_RegFrMsb                      0x06
 #define LR_RegFrMid                      0x07
 #define LR_RegFrLsb                      0x08
 #define LR_RegPaConfig                   0x09
 #define LR_RegPaRamp                     0x0A
 #define LR_RegOcp                        0x0B
 #define LR_RegLna                        0x0C
 #define LR_RegFifoAddrPtr                0x0D
 #define LR_RegFifoTxBaseAddr             0x0E
 #define LR_RegFifoRxBaseAddr             0x0F
 #define LR_RegFifoRxCurrentaddr          0x10
 #define LR_RegIrqFlagsMask               0x11
 #define LR_RegIrqFlags                   0x12
 #define LR_RegRxNbBytes                  0x13
 #define LR_RegRxHeaderCntValueMsb        0x14
 #define LR_RegRxHeaderCntValueLsb        0x15
 #define LR_RegRxPacketCntValueMsb        0x16
 #define LR_RegRxPacketCntValueLsb        0x17
 #define LR_RegModemStat                  0x18
 #define LR_RegPktSnrValue                0x19
 #define LR_RegPktRssiValue               0x1A
 #define LR_RegRssiValue                  0x1B
 #define LR_RegHopChannel                 0x1C
 #define LR_RegModemConfig1               0x1D
 #define LR_RegModemConfig2               0x1E
 #define LR_RegSymbTimeoutLsb             0x1F
 #define LR_RegPreambleMsb                0x20
 #define LR_RegPreambleLsb                0x21
 #define LR_RegPayloadLength              0x22
 #define LR_RegMaxPayloadLength           0x23
 #define LR_RegHopPeriod                  0x24
 #define LR_RegFifoRxByteAddr             0x25
 #define LR_RegModemConfig3               0x26
 #define REG_LR_DIOMAPPING1               0x40
 #define REG_LR_DIOMAPPING2               0x41
 #define REG_LR_VERSION                   0x42
 #define REG_LR_PLLHOP                    0x44
 #define REG_LR_TCXO                      0x4B
 #define REG_LR_PADAC                     0x4D
 #define REG_LR_FORMERTEMP                0x5B
 #define REG_LR_AGCREF                    0x61
 #define REG_LR_AGCTHRESH1                0x62
 #define REG_LR_AGCTHRESH2                0x63
 #define REG_LR_AGCTHRESH3                0x64

 // boylam veri alaný
 #define boylam_alani 29

 // iletim paketi
 unsigned char txbuf[boylam_alani];
 // alým paketi
 unsigned char rxbuf[30];



 int main()
 {

	 DDRB = 0xEF; // giriþ çýkýþ ayarlarý
	 DDRD = 0xF2; // nreset çýkýþ olarak ayarlý.
	 DDRC = 0xFF; // led1 led2 çýkýþ olarak ayarlý
	 USART_Init(MYUBRR);
	 UCSRB = (1 << TXEN)  | (1 << TXCIE); // -> TXCIE0 -> TX Complete Interrupt Enable
	 UCSRB |= (1 << RXEN) | (1 << RXCIE);
//	 _delay_ms(150);	// delay 150ms

	 
	 
	 logic1(NRESET,NRESETT); // modülü sýfýrla
	 logic0(TXENT,TXENN);	// verici anten devresini sýfýrla
	 logic0(RXENR,RXENN);	// alýcý anten devresini sýfýrla
	 logic0(led1,led11);
	 logic0(led2,led22);
	 
	 // spi haberleþmesi
	 
	 SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
	 
	 // sleep mode
	 
	 while (1)
	 {
		 
		 logic0(led1,led11);
		 logic0(led2,led22);
		 reset_sx1276();
		 Config_SX1276();  // Rf modülünü baþlat
		 

		 while(1)
		 {
		//	 cli();
			 gps(txbuf);
			 
	//		 _delay_ms(300);			 
		 }
	 }
	 return 0;
 }

 void USART_Init( unsigned int ubrr)
 {
	 /* Set baud rate */
	 UBRRH = (unsigned char)(ubrr>>8);
	 UBRRL = (unsigned char)ubrr;
	 /* Enable receiver and transmitter */
	 UCSRB = (1<<RXEN)|(1<<TXEN);
	 /* Set frame format: 8data, 2stop bit */
	 UCSRC = (1<<URSEL)|(0<<USBS)|(2<<UCSZ0);
	 UCSRC |= (1 << UPM1);
	 UCSRC &= ( ~(1 << UPM0) | ~(1 << U2X));

 }

 void Seriyibirlestir(char c)
 {
	 seritampon[seriyazPoz] = c;
	 seriyazPoz++;
	 
	 if(seriyazPoz >= TX_tampon_boyutu)
	 {
		 seriyazPoz = 0;
	 }
 }

 void seriyaz(char c[])
 {
	 for(uint8_t i = 0; i< strlen(c); i++)
	 {
		 Seriyibirlestir(c[i]);
		 
	 }
	 hazirla_UART0();
	 
 }

 
 uint8_t SPIregister_oku(uint8_t adres){
	 
	 uint8_t sonuc;
	 
	 logic0(SS,SSS); // lora modülünü seç
	 
	 
	 SPDR = adres; // oku modunda sx1276 için vericiye 7 bitlik baþlangýç paketini gönder
	 
	 while(!(SPSR & (1<<SPIF))); // veri iletilene kadar bekle

	 sonuc = SPDR; // okunan deðeri kaydet
	 
	 SPDR = 0x0;
	 while(!(SPSR & (1<<SPIF)));
	 sonuc = SPDR; // alýnan deðeri kaydet
	 
	 logic1(SS,SSS); // lora modülünü seçimini kaldýr

	 return(sonuc);
 }

 uint8_t SPIregister_yaz(uint8_t adres,uint8_t deger){
	 
	 uint8_t sonuc;
	 
	 logic0(SS,SSS); // LoRa modülünü seç.
	 
	 SPDR = adres | 0x80; // adresi gönder ve veri iletimini baþlat. Yazma modunda adresin 7.biti sx1276 için herzaman 1 dir!
	 while(!(SPSR & (1<<SPIF)));
	 sonuc = SPDR;  // ilk okunaný sil.
	 
	 SPDR =deger;
	 while(!(SPSR & (1<<SPIF)));       // veri iletiminin bitmesi bekle
	 sonuc = SPDR;
	 logic1(SS,SSS);	//Lora modülünü seçmeyi býrak.
	 return(sonuc);
 }
 void SPIpilotsinyali_yaz(unsigned char adres,unsigned char *ptr,unsigned char len)
 {
	 unsigned char i;
	 
//	 char kelime[boylam_alani];
	 
	 
	 logic0(SS,SSS);
	 
	 SPDR = adres | 0x80;
	 while(!(SPSR & (1<<SPIF)));
	 

	 //Debug
	 /*char pointer to hex*/

// 	 fgets(kelime, boylam_alani+1, stdin);
// 	 kelime[boylam_alani+1] = '\0';
	 for (i=0; i<= len; i++)
	 {
		 SPDR =*ptr;
		 while(!(SPSR & (1<<SPIF)));

		 //debug
		 //	seriyaz((char*)txbuf);
// 		 sprintf(kelime,"%02X",(unsigned char)txbuf[i]);
// 		 
// 		 seriyaz(kelime);
		 
		 
		 //debug
		 ptr++;
		 
	 }
	 // debug
	 //	seriyaz("4D2E41736C616E0");
	 
	 
	 
	 seriyaz("\n");
	 // debug
	 
	 
	 logic1(SS,SSS);
 }

 void SPIpilotsinyali_oku(unsigned char adres,unsigned char *ptr,unsigned char len)
 {
	 unsigned char i;

	 
	 logic0(SS,SSS);
	 
	 SPDR = adres;
	 while(!(SPSR & (1<<SPIF)));
	 
	 
	 for (i=0; i<= len; i++)
	 {
		 SPDR = 0;
		 while(!(SPSR & (1<<SPIF)));
		 *ptr = SPDR;
		 
		 ptr++;
	 }
	 
	 //debug
	 seriyaz("\n");
	 //debug
	 
	 logic1(SS,SSS);
 }

 void reset_sx1276(void)
 {
	 logic0(TXENT,TXENN);
	 logic0(RXENR,RXENN);
	 
	 logic0(NRESET,NRESETT);
//	 _delay_ms(10);
	 logic1(NRESET,NRESETT);
//	 _delay_ms(20);
 }

 void Config_SX1276(void)
 {
	 SPIregister_yaz(LR_RegOpMode,0x00);		// uyku modu, yüksek frekans
//	 _delay_ms(10);
	 
	 SPIregister_yaz(REG_LR_TCXO,0x09);		// harici kristal
	 SPIregister_yaz(LR_RegOpMode,0x80);		// LoRa mode, yüksek frekans

	 // 	SPIregister_yaz(LR_RegFrMsb,0xE4);
	 // 	SPIregister_yaz(LR_RegFrMid,0xC0);
	 // 	SPIregister_yaz(LR_RegFrLsb,0x00);		// frekans : 915 Mhz
	 setFrequency(868000000);

	 
	 SPIregister_yaz(LR_RegPaConfig,0xff);	// max çýkýþ gücü (opsiyondur, kapatýlabilir)
	 SPIregister_yaz(LR_RegOcp,0x0B);		// aþýrý akým korumasý kapalý (ocp)
	 SPIregister_yaz(LR_RegLna,0x23);		// LNA aktif
	 
	 SPIregister_yaz(LR_RegModemConfig1, 0x72); //signal bandwidth: 125khz,error coding= 4/5, explicit header mode
	 
	 SPIregister_yaz(LR_RegModemConfig2,0xC7);  // yayýlma faktörü: 12 (spreading factor)
	 
	 SPIregister_yaz(LR_RegModemConfig3,0x08);  // LNA? optimized for low data rate
	 
	 SPIregister_yaz(LR_RegSymbTimeoutLsb,0xFF);// en fazla alma zaman aþýmý
	 
	 SPIregister_yaz(LR_RegPreambleMsb,0x00);
	 SPIregister_yaz(LR_RegPreambleLsb,16);		// baþlangýç(preamble) 16 byte
	 
	 SPIregister_yaz(REG_LR_PADAC,0x87);			// iletim gücü 20dBm
	 SPIregister_yaz(LR_RegHopPeriod,0x00);		// frekans atlamasý yok
	 
	 SPIregister_yaz(REG_LR_DIOMAPPING2,0x01);	// DIO5=Mod hazýr, DIO4 Cad Tespit edildi
	 SPIregister_yaz(LR_RegOpMode,0x01);			// bekleme modu, yüksek frekans
	 
 }

 void verici_modu(void)
 {
	 unsigned char adres,temp;
	 
	 logic1(TXENT,TXENN);							// tx anten anahtarýný aç
	 logic0(RXENR,RXENN);
	 
	 SPIregister_yaz(REG_LR_DIOMAPPING1,0x41);	// DIO0=TxDone,DIO1=RxTimeout,DIO3=ValidHeader
	 
	 SPIregister_yaz(LR_RegIrqFlags,0xff);		// clearing interupt
	 SPIregister_yaz(LR_RegIrqFlagsMask,0xf7);		// enabling txdone
	 SPIregister_yaz(LR_RegPayloadLength,boylam_alani);	// payload length
	 
	 adres = SPIregister_oku(LR_RegFifoTxBaseAddr);	// read TxBaseAddr
	 
	 SPIregister_yaz(LR_RegFifoAddrPtr,adres);	// TxBaseAddr->FifoAddrPtr
	 
	 SPIpilotsinyali_yaz(0x00,txbuf,boylam_alani);	// write data in fifo
	 SPIregister_yaz(LR_RegOpMode,0x03);			// mode tx, high frequency
	 
	 
	 
	 if (bit_is_clear(PINC, 4))
	 {
		 logic1(led1,led11);
	 }
	 else
	 {
		 logic0(led1,led11);
	 }
	 
	 temp = SPIregister_oku(LR_RegIrqFlags); // kesme bayraðýný oku
	 while (!(temp & 0x08))					// tx tamamlanana dek bekle
	 {
		 temp = SPIregister_oku(LR_RegIrqFlags);
	 }
	 
	 logic0(TXENT,TXENN);					// tx anten anahtarý kapalý
	 logic0(RXENR,RXENN);
	 
	 SPIregister_yaz(LR_RegIrqFlags,0xFF); // kesmeyi temizle
	 SPIregister_yaz(LR_RegOpMode,0x01);	  // bekleme modu, yüksek frekans
	 
 }

 void init_rx(void)
 {
	 unsigned char adres;
	 
	 logic0(TXENT,TXENN);					  // rx anten anahtarýný aç
	 logic1(RXENR,RXENN);
	 
	 SPIregister_yaz(REG_LR_DIOMAPPING1,0x01);	//DIO0=00, DIO1=00, DIO2=00, DIO3=01  DIO0=00--RXDONE
	 
	 SPIregister_yaz(LR_RegIrqFlagsMask,0x3f);		// enable rxdone and rxtimeout
	 SPIregister_yaz(LR_RegIrqFlags,0xff);		// clearing interupt

	 adres = SPIregister_oku(LR_RegFifoRxBaseAddr);	// read RxBaseAddr
	 SPIregister_yaz(LR_RegFifoAddrPtr,adres);	// RxBaseAddr->FifoAddrPtr
	 SPIregister_yaz(LR_RegOpMode,0x05);			// rx mode continuous high frequency
 }

 bool setFrequency(uint32_t freq)
 {
	 uint32_t frf;
	 uint32_t temp1;
	 uint32_t temp2;
	 uint8_t reg[3];

	 // Frf(23:0)=frequency/(XOSC/2^19)

	 temp1=freq/1000000;
	 temp2=LORA_XOSC/1000000;
	 frf=temp1*524288/temp2;

	 temp1=freq%1000000/1000;
	 temp2=LORA_XOSC/1000;
	 frf=frf+temp1*524288/temp2;

	 temp1=freq%1000;
	 temp2=LORA_XOSC;
	 frf=frf+temp1*524288/temp2;

	 reg[0]=frf>>16&0xff;
	 reg[1]=frf>>8&0xff;
	 reg[2]=frf&0xff;

	 SPIregister_yaz(LR_RegFrMsb,reg[0]);
	 SPIregister_yaz(LR_RegFrMid,reg[1]);
	 SPIregister_yaz(LR_RegFrLsb,reg[2]);

	 // read if the value has been in register
	 if((reg[0]!=SPIregister_oku(LR_RegFrMsb))||(reg[1]!=SPIregister_oku(LR_RegFrMid))||(reg[2]!=SPIregister_oku(LR_RegFrLsb)))
	 return false;
 }





void USART_Transmit( unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) )
	;
	/* Get and return received data from buffer */
	return UDR;
}


void gps(unsigned char bnum[29])
{
	unsigned char data[128],a;
	uint8_t i=0, j=0, k=0;		
	i=0;j=0;

	//delay
	for(i=0; i<127; i++)
	{
		
		(data[i] = USART_Receive());
		
		if (data[i] == ',' && data[i-1] == 'L' && data[i-2] == 'L' && data[i-3] == 'G' && data[i-4] == 'P' && data[i-5] == 'G')
		{
			
			for (j=0;j<29;j++)
			{
				a = USART_Receive();
				if ((a == 'N')||(a == 'S')||(a == 'W')||(a == 'E'))
				{
					--j;
					continue;
				}
				if (a == ',')
				{
					k++;
				}
				if (k == 2)
				{
					--j;
					k = 0;
					continue;
					
				}
				bnum[j] = a;
					if(j == 28)			
					if (bnum[4] == '.')
					{
						verici_modu();
					}	
			}
			for (j=0;j<29;j++)
				USART_Transmit(bnum[j]);
			
		
		}
	}
}

ISR(USART_RXC_vect)
{

}
ISR(USART_TXC_vect)
{

}






