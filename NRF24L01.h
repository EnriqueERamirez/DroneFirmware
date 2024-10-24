RF24 radio(9, 10); // CE, CSN

// Función para inicializar el módulo nRF24L01
void inicializarNRF24L01() {
  radio.begin();
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
  radio.openWritingPipe(0xF0F0F0F0E2LL);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();

}
