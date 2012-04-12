/*
 *  IRKeyboardEmu-Leopard.cpp
 *  IRKeyboardEmu
 *
 *  Created by Evan Schoenberg on 2/17/09.
 */

#include "IRKeyboardEmu.h"

/* 10.5+ */
void IRKeyboardEmu::issueFakeReport_leopard(int keyCode)
{
	IOMemoryDescriptor *mem;
	char (*keyPress)[2];
	int bits = 8;

	/*
	 Not implemented:

	 [NSNumber numberWithInt:kRemoteButtonPlay_Hold]	forKey:@"35_31_19_18_35_31_19_18_"];
	 [NSNumber numberWithInt:kRemoteControl_Switched]	forKey:@"19_"];			
	 */
	char menu[8][2] = //31_20_19_18_31_20_19_18_
	{
		/* Press */
		{ 0x1E, 0x00 },
		{ 0x13, 0x01 }, { 0x12, 0x01 },
		{ 0x11, 0x00 },
		
		/* Release */
		{ 0x1E, 0x00 },
		{ 0x13, 0x00 }, { 0x12, 0x00 },
		{ 0x11, 0x00 },
	};
	
	/* Menu-hold, pressed and released */
	//WRONG
#undef MENU_PRESS_BITS
#define MENU_PRESS_BITS 6
	char menu_hold[MENU_PRESS_BITS][2] = //31_19_18_31_19_18_
	{
		/* Press */
		{ 0x1E, 0x00 },
		{ 0x12, 0x01 },
		{ 0x11, 0x00 },
		
		/* Release */
		{ 0x1E, 0x00 },
		{ 0x12, 0x00 },
		{ 0x11, 0x00 },
	};
	
	char play[8][2] = //31_21_19_18_ 31_21_19_18_
	{
		/* Press */
		{ 0x1E, 0x00 },
		{ 0x14, 0x01 }, { 0x12, 0x01 },
		{ 0x11, 0x00 },
		
		/* Release */
		{ 0x1E, 0x00 },
		{ 0x14, 0x00 }, { 0x12, 0x00 },
		{ 0x11, 0x00 },
	};
	
	char right[8][2] = //31_22_19_18_31_22_19_18_ ** WORKS **
	{
		/* Press */
		{ 0x1E, 0x00 },
		{ 0x15, 0x02 }, { 0x12, 0x01 },
		{ 0x11, 0x00 },
		
		/* Release */
		{ 0x1E, 0x00 },
		{ 0x15, 0x00 }, { 0x12, 0x00 },
		{ 0x11, 0x00 },
	};
	
#ifdef USE_BROKEN_LEOPARD_FF_AND_REWIND
	/* Fast forward, pressed */
	char right_pressed[5][2] = //31_19_18_4_2_
	{
		{ 0x1E, 0x00 },
		{ 0x12, 0x01 }, { 0x11, 0x02 }, { 0x3, 0x02 },
		{ 0x11, 0x00 },
	};
	
	/* Fast forward, released */
	char right_released[5][2] = //31_19_18_4_2_
	{
		{ 0x1E, 0x00 },
		{ 0x12, 0x00 }, { 0x11, 0x00 }, { 0x3, 0x00 },
		{ 0x11, 0x00 },
	};
#else
	/* Fast forward, pressed */
	char right_pressed[5][2] = //14_6_4_2_
	{
		{ 0x0D, 0x00 },
		{ 0x05, 0x01 }, { 0x03, 0x01 }, { 0x01, 0x01 },
		{ 0x04, 0x00 }
	};
	
	/* Fast forward, released */
	char right_released[5][2] = //14_6_4_2_
	{
		{ 0x0D, 0x00 },
		{ 0x05, 0x00 }, { 0x03, 0x00 }, { 0x01, 0x00 },
		{ 0x04, 0x00 }
	};
#endif
	
	/* Left button, pressed then released */
	char left[8][2] = //31_23_19_18_31_23_19_18_
	{
		/* Press */
		{ 0x1E, 0x00 },
		{ 0x16, 0x01 }, { 0x12, 0x01 },
		{ 0x11, 0x00 },
		
		/* Release */
		{ 0x1E, 0x00 },
		{ 0x16, 0x00 }, { 0x12, 0x00 },
		{ 0x11, 0x00 },
	};
	
#ifdef USE_BROKEN_LEOPARD_FF_AND_REWIND
	/* Rewind, pressed */
	char left_pressed[5][2] = //31_19_18_3_2_
	{
		{ 0x1E, 0x00 },
		{ 0x12, 0x01 }, { 0x11, 0x02 }, { 0x2, 0x01 },
		{ 0x11, 0x00 },
	};
	
	/* Rewind, released */
	char left_released[5][2] = //31_19_18_3_2_
	{
		{ 0x1E, 0x00 },
		{ 0x12, 0x00 }, { 0x11, 0x00 }, { 0x2, 0x00 },
		{ 0x11, 0x00 },
	};
#else
	/* Rewind, pressed */
	char left_pressed[5][2] = //14_6_3_2_
	{
		{ 0x0D, 0x00 },
		{ 0x05, 0x01 }, { 0x02, 0x01 }, { 0x01, 0x01 },
		{ 0x04, 0x00 }
	};
	
	/* Rewind, released */
	char left_released[5][2] = //14_6_3_2_
	{
		{ 0x0D, 0x00 },
		{ 0x05, 0x00 }, { 0x02, 0x00 }, { 0x01, 0x00 },
		{ 0x04, 0x00 }
	};
#endif
	
#define UP_HALF_BITS 5
	char up_pressed[UP_HALF_BITS][2] = //31_29_28_19_18_ 
	{
		{ 0x1E, 0x00 },
		{ 0x1c, 0x01 }, { 0x1b, 0x01 },{ 0x12, 0x01 },
		{ 0x11, 0x00 },
	};
	char up_released[UP_HALF_BITS][2] = //31_29_28_19_18_
	{
		{ 0x1E, 0x00 },
		{ 0x1c, 0x00 }, { 0x1b, 0x00 }, { 0x12, 0x00 },
		{ 0x11, 0x00 },
	};
	
	/* Create 'up', which is up_pressed followed by up_released */
	char up[UP_HALF_BITS*2][2];
	for (int i = 0; i < UP_HALF_BITS; i++) {
		for (int j = 0; j < 2; j++) {
			up[i][j] = up_pressed[i][j];
			up[i+UP_HALF_BITS][j] = up_released[i][j];
		}
	}
	
	char down_pressed[5][2] = //31_30_28_19_18_
	{
		{ 0x1E, 0x00 },
		{ 0x1d, 0x01 }, { 0x1b, 0x01 }, { 0x12, 0x01 },
		{ 0x11, 0x00 } 
	};
	char down_released[5][2] = //31_30_28_19_18_
	{		
		{ 0x1E, 0x00 },
		{ 0x1d, 0x00 }, { 0x1b, 0x00 }, { 0x12, 0x00 },
		{ 0x11, 0x00 } 
	};
	
	/* Create 'down', which is down_pressed followed by down_released */
	char down[10][2];
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			down[i][j] = down_pressed[i][j];
			down[i+5][j] = down_released[i][j];
		}
	}
	
	switch(keyCode)
	{
		case UpKey:
			keyPress = up;
			bits = UP_HALF_BITS*2; 
			break;
		case PressedUp:
			keyPress = up_pressed;
			bits = UP_HALF_BITS;
			break;
		case ReleasedUp:
			keyPress = up_released;
			bits = UP_HALF_BITS;
			break;
		case DownKey:
			keyPress = down; //Point to down command report
			bits = 10;
			break;
		case PressedDown:
			keyPress = down_pressed;
			bits = 5;
			break;
		case ReleasedDown:
			keyPress = down_released;
			bits = 5;
			break;
		case MenuKey:
			keyPress = menu; //Point to menu command report
			bits = 8;
			break;
		case PressedMenu:
			keyPress = menu_hold;
			bits = MENU_PRESS_BITS;
			break;
		case ReleasedMenu:
			/* Don't need to do anything; PressedMenu did menu-held-pressed and menu-held-released */
			keyPress = NULL; 
			break;
		case PlayKey:
			keyPress = play; //Point to play command report
			bits = 8;
			break;
		case RightKey:
			keyPress = right; //Point to right command report
			break;
		case PressedRight:
			keyPress = right_pressed;
			bits = 5;
			break;
		case ReleasedRight:
			keyPress = right_released;
			bits = 5;			
			break;
		case LeftKey:
			keyPress = left; //Point to left command report
			break;
		case PressedLeft:
			keyPress = left_pressed;
			bits = 5;
			break;
		case ReleasedLeft:
			keyPress = left_released;
			bits = 5;			
			break;
		default:
			keyPress = NULL; //Oops! Somehow we got an invalid option. Bail!
			break;
	}
	
	if(keyPress != NULL) //If everything went ok create Memory Descriptors
	{
		for(int i = 0; i < bits; i++)
		{
			mem = IOMemoryDescriptor::withAddress(keyPress[i], sizeof(keyPress[i]), kIODirectionNone);
			handleReport(mem); //Send command report to HID System
			mem->release(); //Free memory
		}
		
	}
}
