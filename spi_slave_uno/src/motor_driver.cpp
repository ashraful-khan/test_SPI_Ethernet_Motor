#include "motor_driver.h"

void spi1_init(){
    SPI1.begin();
    SPI1.beginTransaction(SPI1Settings(1000000, MSBFIRST, SPI_MODE0));
}

void Motor::setOperationMode(uint8_t OperationMode, uint8_t board_number){
	Tle_Board->sendMessageAndCheckAnswer(board_number, MODECONTROL+FOC);
}

void Motor::setMotorSpeed(float speed, uint8_t board_number){
	Tle_Board->sendMessageAndCheckAnswer(board_number, 0x0700);
	Tle_Board->sendMessageAndCheckAnswer(board_number, 0x0200);
}

void Motor::setAction(uint8_t ACTION, uint8_t board_number){
	Tle_Board->sendMessageAndCheckAnswer(board_number, MOTORCONTROL+ACTION);
}

void Motor::resetBoard(int ss){
    Serial.println(F("INFO: Sending reset request to boards"));
    Tle_Board->sendMessage(ss, MOTORCONTROL + STOP_MOTOR);
    delay(10);
    Tle_Board->sendMessage(ss, BOARDCONTROL + RESET);
	delay(10); // TODO reduce time, let slaves do reset
}

void Motor::resetAllBoard(){
	Serial.println("INFO: Sending reset request to boards");
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
	delayMicroseconds(100);
	SPI1.transfer16(0x0902);
	delayMicroseconds(100);
	digitalWrite(4, HIGH);
	digitalWrite(5, HIGH);
}

// functions for communication
void tle_board::sendMessage(int ss, uint16_t data)
{
	 //Serial.println("INFO: Sending reset request11111 to boards");
	digitalWrite(ss, LOW);  // take the SS pin low to select board
	delayMicroseconds(100);
   	SPI1.transfer16(data);
   	delayMicroseconds(100);
   	digitalWrite(ss, HIGH);  // take the SS pin high to de-select board
   	delayMicroseconds(100);
}

uint16_t tle_board::readAnswer(int ss)
{
	uint16_t receivedMessage = 0;
	delayMicroseconds(100);
	digitalWrite(ss, LOW);
	delayMicroseconds(100);
	receivedMessage = SPI1.transfer16(0x0000);
	delayMicroseconds(100);
	digitalWrite(ss, HIGH);

	return receivedMessage;
}

bool tle_board::sendMessageAndCheckAnswer(int ss, uint16_t command)
{
	sendMessage(ss, command);
	delayMicroseconds(10);
	uint16_t answer = readAnswer(ss);

	if(answer == (command + CONFIRM_OFFSET)) return true;
	else
	{
		Serial.print(F("WARNING: Board["));
		Serial.print(ss);
		Serial.print(F("]; Command["));
		Serial.print(command, HEX);
		Serial.print(F("]; Answer["));
		Serial.print(answer, HEX);
		Serial.println(F("]"));
		return false;
	}
}
