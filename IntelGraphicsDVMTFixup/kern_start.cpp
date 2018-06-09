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
    LiluAPI::AllowNormal | LiluAPI::AllowInstallerRecovery,
	bootargOff,
	arrsize(bootargOff),
	bootargDebug,
	arrsize(bootargDebug),
	bootargBeta,
	arrsize(bootargBeta),
	KernelVersion::Yosemite,
	KernelVersion::Mojave,
	[]() {
		igdvmt.init();
	}
};
