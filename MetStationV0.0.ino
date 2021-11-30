 /*
  * Interrupção pela báscula do pluviômetro, aumenta a quantidade de chuva se o sensor de chuva estiver acionado, senão descarta.
  * Tem que setar o sleep do ESP, pode ser só o sleep do WIFI mesmo
  * A cada intervalo, o ESP faz a leitura do estado dos sensores e envia para a planilha.
  * "velocidade/direção/Chuva?/Qtd de chuva/
  * 
  * 
  */
  #include "DHT.h"
  #include "user_interface.h"

  //Definir variáveis
  #define tempo  15 //Tempo que o wifi fica ocioso (em minutos)
  #define DHTTYPE DHT11 //Tipo do DHT
  float pulsePluvi = 0;
  
  //Define pinos
  #define PluviINTERRUPT 13 //D7 - Pino de interrupção da báscula
  #define DHTin 2 //D4 Pino de dados do DHT11
  #define pinoChuva 14 //D5 Pino digital do sensor de chuva
  #define pinoEncoder 12 //D6 Definição do pino do sensor óptico

  DHT dht(DHTin, DHTTYPE); //Cria o sensor DHT
 
void ICACHE_RAM_ATTR interrupcao()
{
  if(pinoChuva == LOW)
  {
    pulsePluvi++;
  }
  delayMicroseconds(100);
}

void setup() 
{
  Serial.begin(9600); //Inicial Serial
  attachInterrupt(digitalPinToInterrupt(PluviINTERRUPT), interrupcao, FALLING); //Interrupção
  dht.begin(); //Inicia dht11
  pinMode(pinoChuva, INPUT);// Pino do sensor de chuva é entrada
}

void loop() {
  //Definir variáveis de leitura
  bool itsRaining = 0;
  float temperatura = 100;
  float umidade = -100;
  int pulseAnem = 0;
  int anemInit=0;

  //Pluviometro
  Serial.print(pulsePluvi/2*0.125);
  Serial.println(" mm");
  delay(1000);

  //Temperatura e umidade via DHT11
  while(isnan(dht.readTemperature()) or temperatura == -100 or umidade == -100){
    umidade = dht.readHumidity();
    temperatura = dht.readTemperature();
  }
  //Anemômetro
  anemInit=millis();
  while(millis()<(anemInit + 10000))
  {
    if (digitalRead(pinoEncoder)==HIGH)
    {
      pulseAnem++;
  }

  if (digitalRead(pinoChuva) == LOW)
  {
    itsRaining = true; 
  }
  wifi_set_sleep_type(tempo * 60000000);

}
