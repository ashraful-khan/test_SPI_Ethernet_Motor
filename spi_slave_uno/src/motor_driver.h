/**
 * @brief 
 * You are writing custom library for arduino for their usage.
 * 
 */
#include <avr/io.h>
#include <Arduino.h>
#include <SPI1.h>

#define ETHERNET        1
#define ETHERNET_PING   1
#define MOTOR_DRIVE     1
#define MOTOR_DRIVE_1   0
#define MOTOR_DRIVE_2   0
#define MOTOR_DRIVE_TEST 0
#define NTP             0


// Constructor
#define BOARD1 		1
#define BOARD2 		2
#define BOARD3 		3
#define BOARD4 		4
#define ALL_BOARDS	5


// common
#define CONFIRM_OFFSET 		0x1000
#define SLAVESELECT_BOARD1 	4
#define SLAVESELECT_BOARD2 	5
#define SLAVESELECT_BOARD3 	6
#define SLAVESELECT_BOARD4 	7

// 0x01: modeControl
#define MODECONTROL 		0x0100
#define BOOTLOADER 			0x00
#define BEMF 				0x01
#define HALL 				0x02
#define FOC 				0x03
#define GETCURRENTMODE 		0x10

// 0x07: setMotorspeed
#define SETMOTORSPEED				0x0700


// 0x08: motorControl
#define MOTORCONTROL 				0x0800
#define START_MOTOR 				0x00
#define STOP_MOTOR 					0x01

// 0x09: boardControl
#define BOARDCONTROL 				0x0900
#define AUTOADDRESSING 				0x00
#define BOARD_AVAILABLE 			0x01
#define RESET 						0x02

void spi1_init();

class  tle_board
{
public:
void sendMessage(int ss, uint16_t);
uint16_t readAnswer(int ss);
bool sendMessageAndCheckAnswer(int ss, uint16_t);
};

class Motor{
    private:
    int number_of_motors;

    public:
    void setOperationMode(uint8_t, uint8_t);
    void setMotorSpeed(float, uint8_t);
    void setAction(uint8_t, uint8_t);
    tle_board *Tle_Board;
    void resetBoard(int ss);    
    void resetAllBoard();
};
