//**************************
//DS1302配置
#define CLK 3//时钟引脚
#define DAT 2//数据引脚
#define RST 4//片选输入
//**************************

uint8_t WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};
//---DS1302时钟初始化2016年5月7日星期六12点00分00秒。---//
//---存储顺序是秒分时日月周年,存储格式是用BCD码---//
uint8_t TIME[7] = {0, 0, 0x12, 0x07, 0x05, 0x06, 0x16};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(CLK, OUTPUT);
  pinMode(DAT, OUTPUT);
  pinMode(RST, OUTPUT);
  DS_writer(0x8E, 0x00); //关闭写保护
  DS_writer(0x80, B00000000);//将秒寄存器最高位设置为0以开始工作

  for (int n = 0; n < 7; n++) //写入7个字节的时钟信号：分秒时日月周年
  {
    DS_writer(WRITE_RTC_ADDR[n], TIME[n]);
  }
  DS_writer(0x8E, B10000000); //打开写保护
}

void loop() {
  Serial.println(BCD(DS_read(0x81)));
  delay(1000);
}

//***********************************************************************************************
//***************************************
//将bcd码转为十进制
//***************************************
int BCD(byte data) {
  byte temp;
  temp = ((data >> 4) * 10 + (data & 0x0f));
  return temp;
}


//读取ds1302的数据
uint8_t DS_read(uint8_t dizhi) {
  uint8_t data = 0x00;
  digitalWrite(RST, LOW); //将片选引脚拉高
  digitalWrite(CLK, LOW); //将时钟引脚拉低
  //***************************************
  //发送地址字节
  //***************************************
  digitalWrite(RST, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWrite(CLK, LOW);
    delayMicroseconds(1);
    if (dizhi & 0x01 == 1) {
      digitalWrite(DAT, HIGH);
    } else {
      digitalWrite(DAT, LOW);
    }
    delayMicroseconds(1);
    digitalWrite(CLK, HIGH);
    dizhi = dizhi >> 1;
    delayMicroseconds(1);
  }
  //***************************************
  //读取数据字节
  //***************************************
  digitalWrite(DAT, LOW);
  pinMode(DAT, INPUT);
  for (int i = 0; i < 8; i++) {
    digitalWrite(CLK, LOW);
    delayMicroseconds(1);
    //Serial.println(digitalRead(DAT));
    data >>= 1;
    if (digitalRead(DAT)) {
      data |= B10000000;
    } else {
      data &= B01111111;
    }
    //data = (data >> 1) + digitalRead(DAT);
    delayMicroseconds(1);
    digitalWrite(CLK, HIGH);
    delayMicroseconds(1);
  }
  digitalWrite(RST, LOW); //将片选引脚拉低结束接收
  delayMicroseconds(1);
  pinMode(DAT, OUTPUT);
  return data;
}

//向ds1302写入8bit的数据
void DS_writer(uint8_t dizhi, uint8_t data) {
  digitalWrite(RST, LOW); //将片选引脚拉高
  digitalWrite(CLK, LOW); //将时钟引脚拉低
  //***************************************
  //发送地址字节
  //***************************************
  digitalWrite(RST, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWrite(CLK, LOW);
    delayMicroseconds(1);
    if (dizhi & 0x01 == 1) {
      digitalWrite(DAT, HIGH);
    } else {
      digitalWrite(DAT, LOW);
    }
    delayMicroseconds(1);
    digitalWrite(CLK, HIGH);
    dizhi = dizhi >> 1;
    delayMicroseconds(1);
  }
  //***************************************
  //发送数据字节
  //***************************************
  for (int i = 0; i < 8; i++) {
    digitalWrite(CLK, LOW);
    delayMicroseconds(1);
    if (data & 0x01 == 1) {
      digitalWrite(DAT, HIGH);
    } else {
      digitalWrite(DAT, LOW);
    }
    delayMicroseconds(1);
    digitalWrite(CLK, HIGH);
    data = data >> 1;
    delayMicroseconds(1);
  }
  digitalWrite(RST, LOW); //将片选引脚拉低结束传送
  delayMicroseconds(1);
}
