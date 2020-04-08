/*
 *  IRKeyboardEmu-Tiger.c
 *  IRKeyboardEmu
 *
 *  Created by Evan Schoenberg on 2/17/09.
 *  Copyright 2009 Adium X / Saltatory Software. All rights reserved.
 *
 */

#include "IRKeyboardEmu.h"

#if __MAC_OS_X_VERSION_MIN_REQUIRED < 1040
/* 10.4 or - */
/*
 * Left, Right, Menu, and Play are determined to be up&down or held by the hardware remote.
 * There are therefore separate codes for up&down (which is sent together) and for the held version.
 * The held version uses one code for "pressed" and another code for "released."
 *
 * Menu-held sends pressed and released as a unit after 2 seconds of being held.
 *
 * Up and Down send press and release as separate codes at all times; there is no 'held' variant.
 *
 * We're sending the 10.4 infrared codes.  Most applications are fine with that. Some are only
 * looking for the 10.5 codes when running on 10.5. That's no good :(
 */	
void IRKeyboardEmu::issueFakeReport(int keyCode)
{
	IOMemoryDescriptor *mem;
	char (*keyPress)[2];
	int bits = 8;
	
	/*
	 Known 10.4 cookies we don't implement:
	 Play_Hold: 18_14_6_5_18_14_6_5_
	 Switched (?): 19_
	 */	
	
	char menu[8][2] = //14_7_6_5_14_7_6_5_ * Fixed *
	{
		/* Press */
		{ 0x0D, 0x00 },
		{ 0x06, 0x01 }, { 0x05, 0x01 },
		{ 0x04, 0x00 },
		
		/* Release */
		{ 0x0D, 0x00 },
		{ 0x06, 0x00 }, { 0x05, 0x00 },
		{ 0x04, 0x00 }
	};
	
	/* Menu-hold, pressed and released */
	char menu_hold[MENU_PRESS_BITS][2] = //14_6_5_14_6_5_
	{
		/* Press */
		{ 0x0D, 0x00 },
		{ 0x05, 0x02 },
		{ 0x04, 0x00 },
		
		/* Release */
		{ 0x0D, 0x00 },
		{ 0x05, 0x00 },
		{ 0x04, 0x00 },
	};
	
	char play[8][2] = //14_8_6_5_14_8_6_5_ * Fixed *
	{
		/* Press */
		{ 0x0D, 0x00 },
		{ 0x07, 0x01 }, { 0x05, 0x02 },
		{ 0x04, 0x00 },
		
		/* Release */
		{ 0x0D, 0x00 },
		{ 0x07, 0x00 }, { 0x05, 0x00 },
		{ 0x04, 0x00 }
	};
	
	char right[8][2] = //14_9_6_5_14_9_6_5_ * Fixed *
	{
		/* Press */
		{ 0x0D, 0x00 },
		{ 0x08, 0x01 }, { 0x05, 0x03 },
		{ 0x04, 0x00 },
		
		/* Release */
		{ 0x0D, 0x00 },
		{ 0x08, 0x00 }, { 0x05, 0x00 },
		{ 0x04, 0x00 }
	};
	
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
	
	/* Left button, pressed then released */
	char left[8][2] = //14_10_6_5_ 14_10_6_5_ * Fixed *
	{
		/* Press */
		{ 0x0D, 0x00 },
		{ 0x09, 0x01 }, { 0x05, 0x04 },
		{ 0x04, 0x00 },
		
		/* Release */
		{ 0x0D, 0x00 },
		{ 0x09, 0x00 }, { 0x05, 0x00 },
		{ 0x04, 0x00 }
	};
	
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
	
	char up_pressed[4][2] = //14_ 12_11_6_ 5_ * Fixed *
	{
		{ 0x0D, 0x00 },
		{ 0x0B, 0x01 }, { 0x0A, 0x01 },
		{ 0x05, 0x00 } 
	};

	char up_released[4][2] = //14_ 12_11_6_ 5_ * Fixed *
	{
		{ 0x0D, 0x00 },
		{ 0x0B, 0x00 }, { 0x0A, 0x00 },
		{ 0x05, 0x00 } 
	};
	
	/* Create 'up', which is up_pressed followed by up_released */
	char up[8][2];

    for (int i = 0; i < 4; i++)
    {
		for (int j = 0; j < 2; j++)
        {
			up[i][j] = up_pressed[i][j];
        }
    }

	for (int i = 0; i < 4; i++)
    {
		for (int j = 0; j < 2; j++)
        {
			up[i+4][j] = up_released[i][j];
        }
    }
	
	char down_pressed[4][2] = //14_13_11_6_5_ * Fixed *
	{
		{ 0x0D, 0x00 },
		{ 0x0C, 0x01 }, { 0x0A, 0x02 },
		{ 0x05, 0x00 }
	};

	char down_released[4][2] = 
	{		
		{ 0x0D, 0x00 },
		{ 0x0C, 0x00 }, { 0x0A, 0x00 },
		{ 0x05, 0x00 }
	};
	
	/* Create 'down', which is down_pressed followed by down_released */
	char down[8][2];

	for (int i = 0; i < 4; i++)
    {
		for (int j = 0; j < 2; j++)
        {
			down[i][j] = down_pressed[i][j];
        }
    }

	for (int i = 0; i < 4; i++)
    {
		for (int j = 0; j < 2; j++)
        {
			down[i+4][j] = down_released[i][j];
        }
    }
	
	switch(keyCode)
	{
		case UpKey:
			keyPress = up;
			bits = 8; 
			break;

		case PressedUp:
			keyPress = up_pressed;
			bits = 4;
			break;

		case ReleasedUp:
			keyPress = up_released;
			bits = 4;
			break;

		case DownKey:
			keyPress = down; //Point to down command report
			bits = 8;
			break;

		case PressedDown:
			keyPress = down_pressed;
			bits = 4;
			break;

		case ReleasedDown:
			keyPress = down_released;
			bits = 4;
			break;

		case MenuKey:
			keyPress = menu; //Point to menu command report
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
	
	if (keyPress != NULL) //If everything went ok create Memory Descriptors
	{
		for (int i = 0; i < bits; i++)
		{
			mem = IOMemoryDescriptor::withAddress(keyPress[i], sizeof(keyPress[i]), kIODirectionNone);

            if (mem != NULL)
            {
                handleReport(mem); //Send command report to HID System
                mem->release(); //Free memory
            }
		}		
	}
}
#endif /* __MAC_OS_X_VERSION_MIN_REQUIRED > 1040 */
