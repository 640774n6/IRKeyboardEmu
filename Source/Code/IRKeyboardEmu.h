/*
	File:		IRKeyboardEmu.h
	Version:	0.4
	Creater:	Brandon M. Holland, bholland@brandon-holland.com
	Purpose:	AppleIRController emulation via the keyboard and HIDUsagePage injection.
*/

#include <IOKit/usb/IOUSBInterface.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#include <IOKit/IOLib.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOTimerEventSource.h>
#include <sys/kern_control.h>

typedef enum {
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
private:
public:
    void issueFakeReport_leopard(int inputCode);
    void issueFakeReport_tiger(int inputCode);

	char *reportDescriptor_leopard();

    virtual void free();
	virtual bool init(OSDictionary* properties);

    virtual bool handleStart(IOService *provider);
    virtual void handleStop(IOService *provider);

	void readInput(void);
	virtual IOReturn setProperties(OSObject *properties);
    virtual IOReturn newReportDescriptor(IOMemoryDescriptor **descriptor) const;
    virtual OSString * newTransportString() const;
    virtual OSString * newManufacturerString() const;
    virtual OSString * newProductString() const;
    virtual OSNumber * newVendorIDNumber() const;
    virtual OSNumber * newProductIDNumber() const;
    virtual OSNumber * newPrimaryUsageNumber() const;
    virtual OSNumber * newPrimaryUsagePageNumber() const;
};
