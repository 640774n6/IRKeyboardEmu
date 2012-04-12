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

#undef super
#define super IOHIDDevice

static int sysctl_sendIR SYSCTL_HANDLER_ARGS;
static unsigned int keyvar = 1;
IRKeyboardEmu *me;
static bool isOnLeopardOrBetter = false;

SYSCTL_PROC(_kern, OID_AUTO, sendIR, CTLTYPE_INT|CTLFLAG_RW, 
				&keyvar, 0, &sysctl_sendIR, "I", "");

OSDefineMetaClassAndStructors(IRKeyboardEmu, IOHIDDevice)

// Fake Properties

/*
 0 through 5: button down then button up immediately
 6 through 11: button down (or button-held in the case of Menu)
 12 through 17: button up
 
 See IRKeyboardEmu.h for the enumeration.
 */
static int sysctl_sendIR SYSCTL_HANDLER_ARGS {
	int error;
	error = sysctl_handle_int(oidp, oidp->oid_arg1, oidp->oid_arg2,  req);
	if(keyvar > 17)
		return 1;
	if (!error && req->newptr) 
	{
		if (isOnLeopardOrBetter)
			me->issueFakeReport_leopard(keyvar);
		else
			me->issueFakeReport_tiger(keyvar);
	}
	return error;
}

OSString * IRKeyboardEmu::newTransportString() const
{ return OSString::withCStringNoCopy("USB"); }

OSString * IRKeyboardEmu::newManufacturerString() const
{ return OSString::withCStringNoCopy("640774n6"); }

OSString * IRKeyboardEmu::newProductString() const
{ return OSString::withCStringNoCopy("IRKeyboardEmu"); }

OSNumber * IRKeyboardEmu::newVendorIDNumber() const
{ return OSNumber::withNumber(0x05AC, 16); } //Actual AppleIR VendorID 

OSNumber * IRKeyboardEmu::newProductIDNumber() const
{ return OSNumber::withNumber(0x8240, 16); } //Actual AppleIR ProductID

OSNumber * IRKeyboardEmu::newPrimaryUsageNumber() const
{ return OSNumber::withNumber(kHIDUsage_Csmr_ConsumerControl, 32); }

OSNumber * IRKeyboardEmu::newPrimaryUsagePageNumber() const
{ return OSNumber::withNumber(kHIDPage_Consumer, 32); }

static bool preLeopardOS()
{
	int preLeopardOS = 1;
	char buffer[128];
	size_t bufsize = sizeof(buffer);
	if (0 == sysctlbyname("kern.osrelease", buffer, &bufsize, NULL, 0)) {
		preLeopardOS = (strcmp(buffer, "9") < 0); // kern.osrevision = 8.8.1 on Tiger, 9.x on Leopard
	}

	return preLeopardOS;
}

bool IRKeyboardEmu::init(OSDictionary *properties)
{
    bool res = super::init(properties);
	me = this;
	sysctl_register_oid(&sysctl__kern_sendIR);

 	isOnLeopardOrBetter = !preLeopardOS();
	
	return res;
}

void IRKeyboardEmu::free()
{ 
	sysctl_unregister_oid(&sysctl__kern_sendIR);
	super::free(); 
}

void IRKeyboardEmu::handleStop( IOService * provider )
{ super::handleStop(provider); }

bool IRKeyboardEmu::handleStart( IOService * provider )
{
    if ( super::handleStart(provider) == false )
        return false;
    return true;
}

//---------------------------------------------------------------------------
// Report fake report descriptor and report data.

IOReturn IRKeyboardEmu::newReportDescriptor(IOMemoryDescriptor ** desc) const
{
	//The evil Apple Remote descriptor report (this was the hard part :) )
	//Create buffer memory to hold our descriptor

	if (isOnLeopardOrBetter)
		*desc = IOBufferMemoryDescriptor::withBytes(leopard_report, sizeof(leopard_report), kIODirectionNone);
	else
		*desc = IOBufferMemoryDescriptor::withBytes(tiger_report, sizeof(tiger_report), kIODirectionNone);

	return kIOReturnSuccess;
}
	
