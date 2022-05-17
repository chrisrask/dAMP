/*
 * functions.h
 *
 *  Created on: Feb 11, 2022
 *      Author: pvh318
 */

#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_



#endif /* INC_FUNCTIONS_H_ */
//
// definitions
//
#ifndef TC74_I2C_ADDR
#define TC74_I2C_ADDR	(0x4B << 1)
#endif

#define HIGH 1
#define LOW 0



//
// Variables
//
int aState, aLastState, gain, i, cGain, sGain, counterGain, counterImp, counterStab, cStab, counterFan, fanSel;
int counter, relay;
int selector;
float cImpedance, impedance;



//
// Function defintions
//
int countGain(TIM_HandleTypeDef *htim);
float countImpedance(TIM_HandleTypeDef *htim);
int pbSelection(void);
void print(char gain[2], char impedance[20], char relayStatusS[5], char stabS[2], char fanSpdS[3], int selection, char temp_cS[3]);
int amplify(int gain);
void outImp(float impedance);
int countStab(TIM_HandleTypeDef *htim);
int fanCtrl(TIM_HandleTypeDef *htim, int temp_c);
int fanSet(TIM_HandleTypeDef *htim);
int tempWarning(int temp_c);

// Bitmap

extern unsigned char afry_128x64 [];
extern unsigned char pepe_128x64 [];
