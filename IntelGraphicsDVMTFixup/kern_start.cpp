//
//  kern_start.cpp
//  IntelGraphicsDVMTFixup
//
//  Copyright © 2017 lvs1974. All rights reserved.
//
//  This kext is made based on lvs1974's NvidiaGraphicsFixup.
//

#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>

#include "kern_igdvmt.hpp"

static IGDVMT igdvmt;

static const char *bootargOff[] {
	"-igdvmtoff"
};

static const char *bootargDebug[] {
	"-igdvmtdbg"
};

static const char *bootargBeta[] {
	"-igdvmtbeta"
};

PluginConfiguration ADDPR(config) {
	xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
	bootargOff,
	sizeof(bootargOff)/sizeof(bootargOff[0]),
	bootargDebug,
	sizeof(bootargDebug)/sizeof(bootargDebug[0]),
	bootargBeta,
	sizeof(bootargBeta)/sizeof(bootargBeta[0]),
	KernelVersion::Yosemite,
	KernelVersion::Sierra,
	[]() {
		igdvmt.init();
	}
};





