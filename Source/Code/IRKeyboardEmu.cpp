/*
	File:		IRKeyboardEmu.cpp
	Version:	0.5
	Creater:	Brandon M. Holland, bholland@brandon-holland.com
	Purpose:	AppleIRController emulation via the keyboard and HIDUsagePage injection.
 
	Modified by Evan Schoenberg, 2/2009:
		* Added support for button holds
		* Added detection of the Mac OS X version:
			* Use the 10.5 report descriptor on 10.5
			* Send the new 10.5 cookies on 10.5
		* Added comments, including report descriptor analysis
*/

/*
 Manual debugging notes:

 Before building:
 sudo kextunload IRKeyboardEmu.kext; sudo rm -rf IRKeyboardEmu.kext
 
 After building:
 sudo chown -R root:wheel IRKeyboardEmu.kext/; sudo chmod -R 755 IRKeyboardEmu.kext/; sudo kextload IRKeyboardEmu.kext
 */

#include "IRKeyboardEmu.h"

#include <IOKit/hidsystem/IOHIDTypes.h>
#include <IOKit/hidsystem/IOHIDParameter.h>
#include <IOKit/IOLib.h>
#include <IOKit/IOPlatformExpert.h>
#include <IOKit/hidsystem/IOHidUsageTables.h>
#include <IOKit/IOReturn.h>

#include <mach/mach_types.h>

#include <sys/systm.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#include "ReportDescriptor-Tiger.h"
#include "ReportDescriptor-Leopard.h"

#undef  super
#define super IOHIDDevice

static int sysctl_sendIR SYSCTL_HANDLER_ARGS;
static unsigned int keyvar = 1;
IRKeyboardEmu *me = NULL;

SYSCTL_PROC(_kern, OID_AUTO, sendIR, CTLTYPE_INT|CTLFLAG_RW|CTLFLAG_ANYBODY,
            &keyvar, 0, &sysctl_sendIR, "I", "");

OSDefineMetaClassAndStructors(IRKeyboardEmu, IOHIDDevice)

// Fake Properties
/*
 0 through 5: button down then button up immediately
 6 through 11: button down (or button-held in the case of Menu)
 12 through 17: button up
 
 See IRKeyboardEmu.h for the enumeration.
 */
static int sysctl_sendIR SYSCTL_HANDLER_ARGS
{
	int error = sysctl_handle_int(oidp, oidp->oid_arg1, oidp->oid_arg2,  req);

	if(keyvar > 17)
    {
		return 1;
    }

	if ((error == 0) && (req->newptr != (user_addr_t)0))
	{
        me->issueFakeReport(keyvar);
	}

	return error;
}

IOReturn IRKeyboardEmu::setProperties(OSObject *properties)
{
    (void)properties;

    return kIOReturnSuccess;
}

//Actual Transport Type
OSString *IRKeyboardEmu::newTransportString(void) const
{
    return OSString::withCStringNoCopy("USB");
}

//Actual AppleIR Manufacturer String
OSString *IRKeyboardEmu::newManufacturerString(void) const
{
    return OSString::withCStringNoCopy("640774n6");
}

//Emulator Product String
OSString *IRKeyboardEmu::newProductString(void) const
{
    return OSString::withCStringNoCopy("IRKeyboardEmu");
}

//Actual AppleIR Vendor ID
OSNumber *IRKeyboardEmu::newVendorIDNumber(void) const
{
    return OSNumber::withNumber(APPLE_VENDOR_ID, 16);
}

//Actual AppleIR Product ID
OSNumber *IRKeyboardEmu::newProductIDNumber(void) const
{
    return OSNumber::withNumber(APPLE_USBIRD_ID, 16);
}

OSNumber *IRKeyboardEmu::newPrimaryUsageNumber(void) const
{
    return OSNumber::withNumber(kHIDUsage_Csmr_ConsumerControl, 32);
}

OSNumber * IRKeyboardEmu::newPrimaryUsagePageNumber(void) const
{
    return OSNumber::withNumber(kHIDPage_Consumer, 32);
}

bool IRKeyboardEmu::init(OSDictionary *properties)
{
    bool res = super::init(properties);

    if (res == true)
    {
        me = this;
        sysctl_register_oid(&sysctl__kern_sendIR);
    }
	
	return res;
}

void IRKeyboardEmu::free(void)
{ 
	sysctl_unregister_oid(&sysctl__kern_sendIR);

	super::free(); 
}

void IRKeyboardEmu::handleStop(IOService *provider)
{
    super::handleStop(provider);
}

bool IRKeyboardEmu::handleStart(IOService *provider)
{
    return super::handleStart(provider);
}

//---------------------------------------------------------------------------
// Report fake report descriptor and report data.
//---------------------------------------------------------------------------
IOReturn IRKeyboardEmu::newReportDescriptor(IOMemoryDescriptor **desc) const
{
	//The evil Apple Remote descriptor report (this was the hard part :) )
	//Create buffer memory to hold our descriptor
    if (desc == NULL)
    {
        return kIOReturnError;
    }

    *desc = IOBufferMemoryDescriptor::withBytes(report_descriptor,
                                                sizeof(report_descriptor),
                                                kIODirectionNone);

    if (*desc == NULL)
    {
        return kIOReturnError;
    }

	return kIOReturnSuccess;
}
	
