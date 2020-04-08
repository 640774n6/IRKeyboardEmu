/*
	File:		IRKeyboardEmu.h
	Version:	0.4
	Creater:	Brandon M. Holland, bholland@brandon-holland.com
	Purpose:	AppleIRController emulation via the keyboard and HIDUsagePage injection.
*/

#include <Availability.h>

#include <IOKit/hid/IOHIDDevice.h>

#include <IOKit/IOBufferMemoryDescriptor.h>
#include <IOKit/IOLib.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOTimerEventSource.h>

#include <sys/kern_control.h>

#ifndef APPLE_VENDOR_ID
#define APPLE_VENDOR_ID 0x05AC
#endif /* APPLE_VENDOR_ID */

#ifndef APPLE_USBIRD_ID
#define APPLE_USBIRD_ID 0x8240
#endif /* APPLE_USBIR_ID */

#ifndef MENU_PRESS_BITS
#define MENU_PRESS_BITS 6
#endif /* MENU_PRESS_BITS */

#ifndef APPLE_KEXT_OVERRIDE
#define APPLE_KEXT_OVERRIDE
#endif /* APPLE_KEXT_OVERRIDE */

typedef enum _IRKeyboardKey
{
	UpKey = 0,
	DownKey,
	MenuKey,
	PlayKey,
	RightKey,
	LeftKey,
	
	PressedUp, /* 6 */
	PressedDown,
	PressedMenu, /* Implies menu-held; no ReleasedMenu needed */
	PressedPlay, /* Not implemented! */
	PressedRight, /*  10 - fast forward */
	PressedLeft, /* 11 - rewind */
	
	ReleasedUp, /* 12 */
	ReleasedDown,
	ReleasedMenu, /* No-op; included for numerical consistency */
	ReleasedPlay,
	ReleasedRight,  /*  16 - fast forward */
	ReleasedLeft /* 17 - rewind */
} IRKeyboardKey;

class IRKeyboardEmu : public IOHIDDevice
{
    OSDeclareDefaultStructors(IRKeyboardEmu)

public:
    virtual void issueFakeReport(int inputCode);

    virtual void free(void) APPLE_KEXT_OVERRIDE;
	virtual bool init(OSDictionary* properties) APPLE_KEXT_OVERRIDE;

    virtual bool handleStart(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void handleStop(IOService *provider) APPLE_KEXT_OVERRIDE;

	virtual IOReturn setProperties(OSObject *properties) APPLE_KEXT_OVERRIDE;
    virtual IOReturn newReportDescriptor(IOMemoryDescriptor **descriptor) const APPLE_KEXT_OVERRIDE;

    virtual OSString *newTransportString(void) const APPLE_KEXT_OVERRIDE;
    virtual OSString *newManufacturerString(void) const APPLE_KEXT_OVERRIDE;
    virtual OSString *newProductString(void) const APPLE_KEXT_OVERRIDE;

    virtual OSNumber *newVendorIDNumber(void) const APPLE_KEXT_OVERRIDE;
    virtual OSNumber *newProductIDNumber(void) const APPLE_KEXT_OVERRIDE;
    virtual OSNumber *newPrimaryUsageNumber(void) const APPLE_KEXT_OVERRIDE;
    virtual OSNumber *newPrimaryUsagePageNumber(void) const APPLE_KEXT_OVERRIDE;
};
