/*
SD card attached to SPI bus as follows:
 ** MOSI - pin 16
 ** MISO - pin 14
 ** CLK - pin 15
 ** SCK- pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

#include <SPI.h>
#include <SD.h>
#include <GyverButton.h>
#include <GyverOLED.h>
#include <Wire.h>


File root;

int NumOfFiles = 0; //Кол-во файлов на карте
int CurrentFileNum = 0; //Счётчик файлов для меню
String* FileNames; //Динамический массив для меню

GButton MainButt(7);

GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;

void setup() 
{ 
  //Настройки кнопок 
  MainButt.setTimeout(500);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW); 
  // Запуск serial (последовательного) порта:
  delay(2000);
  Serial.begin(115200); 
  while(!Serial){;}
  // Инициализация microsd карты:
  Serial.print("Инициализация microsd карты... "); 
  if (!SD.begin(10)){Serial.println("ошибка инициализации!");while(1);} 
  Serial.println("инициализация произведена."); 
  //Вывод списка файлов:
  root = SD.open("/KEYS");
  printDirectory(root);
  root.close();
  //Заполнение массива именами файлов:
  root = SD.open("/KEYS");
  appendtoarray(root);
  root.close();
  //Инициализация дисплея:
  oled.init();        // инициализация
  oled.clear();       // очистка
  oled.setScale(2);   // масштаб текста (1..4)
  oled.home();        // курсор в 0,0
  oled.print(FileNames[CurrentFileNum]);
  oled.setCursor(0, 3);
  oled.setScale(1);  
  oled.print("Active: False"); 
}

//Функция читает файлы в папке, объявленной в переменной "root" и выводит их в Serial.print
String printDirectory(File dir) 
{
 Serial.print("Список файлов на microsd карте:\n");
 while (true) 
 {
  File entry =  dir.openNextFile();
  if (! entry) { break; } // Завершить чтение файлов, если файлы закончились   
  Serial.println(entry.name());
  String FileName = entry.name();
  entry.close();
  NumOfFiles++;   
 } 
 Serial.print("Кол-во файлов на microsd карте: ");
 Serial.print(NumOfFiles);
 Serial.println();
 FileNames = new String[NumOfFiles];
}

//Функция заполняет массив именами файлов
void appendtoarray(File dir)
{
 for(int i=0; i<NumOfFiles; i++)
 {
  File entry = dir.openNextFile();
  if (! entry) { break; } // Завершить чтение файлов, если файлы закончились   
  entry.close();
  FileNames[i]=entry.name(); 
 }
}

//Фунция выводит информацию о текущем файле
void print_inf(int)
{
  oled.clear();       
  oled.home();       
  oled.setScale(2);  
  oled.print(FileNames[CurrentFileNum]);
  oled.setCursor(0, 3);
  oled.setScale(1);  
  oled.print("Active: False");  
  Serial.print(CurrentFileNum);
  Serial.println(FileNames[CurrentFileNum]);
}
void loop()
{
 MainButt.tick();
 if (MainButt.isSingle())
  {
   if (CurrentFileNum==NumOfFiles-1) { CurrentFileNum=0; } 
   else { CurrentFileNum++; }
   print_inf(CurrentFileNum);
  }
 if (MainButt.isDouble())
  { 
   if (CurrentFileNum==0) { CurrentFileNum=NumOfFiles-1; }
   else { CurrentFileNum--; }   
   print_inf(CurrentFileNum);
  }
 if (MainButt.isTriple())
  {
   Serial.print(FileNames[CurrentFileNum]); 
   Serial.print(" key is active\n");
   oled.setCursor(0, 3);
   oled.setScale(1); 
   oled.print("Active: True ");  
  }
}
