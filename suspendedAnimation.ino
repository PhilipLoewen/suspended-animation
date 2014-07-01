/*Port Definitions*/
int Max7219_pinCLK = 10;
int Max7219_pinCS = 9;
int Max7219_pinDIN = 5;
 
void Write_Max7219_byte(unsigned char DATA) {
  unsigned char i;
  digitalWrite(Max7219_pinCS,LOW);		
  for(i=8;i>=1;i--) {
  digitalWrite(Max7219_pinCLK,LOW);
  digitalWrite(Max7219_pinDIN,DATA&0x80);// Extracting a bit data
  DATA = DATA<<1;
  digitalWrite(Max7219_pinCLK,HIGH);
  }
}
 
void Write_Max7219(unsigned char address,unsigned char dat) {
  digitalWrite(Max7219_pinCS,LOW);
  Write_Max7219_byte(address);           //addressï¼Œcode of LED
  Write_Max7219_byte(dat);               //dataï¼Œfigure on LED 
  digitalWrite(Max7219_pinCS,HIGH);
}
 
void Init_MAX7219(void) {
 Write_Max7219(0x09, 0x00);       //decoding BCD
 Write_Max7219(0x0a, 0x03);       //brightness 
 Write_Max7219(0x0b, 0x07);       //scanlimit-8 LEDs
 Write_Max7219(0x0c, 0x01);       //power-down modeï¼š0ï¼Œnormal modeï¼š1
 Write_Max7219(0x0f, 0x00);       //test displayï¼š1ï¼›EOTï¼Œdisplayï¼š0
}
