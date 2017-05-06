//
//  kern_igdvmt.cpp
//  IntelGraphicsDVMTFixup
//
//  Copyright © 2017 lvs1974. All rights reserved.
//
//  This kext is made based on lvs1974's NvidiaGraphicsFixup.
//

#include <Headers/kern_api.hpp>
#include <Headers/kern_util.hpp>
#include <Library/LegacyIOService.h>

#include <mach/vm_map.h>
#include <IOKit/IORegistryEntry.h>

#include "kern_igdvmt.hpp"


KernelVersion KernelCheck = getKernelVersion();


static const char *kextBDWGraphicsFramebuffer[] {
    "/System/Library/Extensions/AppleIntelBDWGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelBDWGraphicsFramebuffer" };
    
static const char *kextSKLGraphicsFramebuffer[] {
    "/System/Library/Extensions/AppleIntelSKLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelSKLGraphicsFramebuffer" };

static KernelPatcher::KextInfo kextList[] {
    { "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", kextBDWGraphicsFramebuffer, 1, true, {}, KernelPatcher::KextInfo::Unloaded },
    { "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", kextSKLGraphicsFramebuffer, 1, true, {}, KernelPatcher::KextInfo::Unloaded },
};

static size_t kextListSize {2};

bool IGDVMT::init() {
	LiluAPI::Error error = lilu.onKextLoad(kextList, kextListSize,
	[](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
		IGDVMT *igdvmt = static_cast<IGDVMT *>(user);
		igdvmt->processKext(patcher, index, address, size);
	}, this);
	
	if (error != LiluAPI::Error::NoError) {
		SYSLOG("igdvmt @ failed to register onPatcherLoad method %d", error);
		return false;
	}
	
	return true;
}

void IGDVMT::deinit() {
}

void IGDVMT::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    if (progressState != ProcessingState::EverythingDone) {
        for (size_t i = 0; i < kextListSize; i++) {
            if (kextList[i].loadIndex == index) {
                if (!(progressState & ProcessingState::GraphicsFramebufferPatched) && !strcmp(kextList[i].id, "com.apple.driver.AppleIntelBDWGraphicsFramebuffer")) {
                    DBGLOG("igdvmt @ found com.apple.driver.AppleIntelBDWGraphicsFramebuffer");
                    if (KernelCheck == KernelVersion::Yosemite){
                        const uint8_t find[]    = {0x39, 0xCF, 0x76, 0x3C};
                        const uint8_t replace[] = {0x39, 0xCF, 0xEB, 0x3C};
                        KextPatch kext_patch {
                            {&kextList[i], find, replace, sizeof(find), 1},
                            KernelVersion::Yosemite, KernelVersion::Yosemite
                        };
                        applyPatches(patcher, index, &kext_patch, 1);
                        progressState |= ProcessingState::GraphicsFramebufferPatched;
                        DBGLOG("igdvmt @ Broadwell - 10.10 :: DVMT patches applied");
                    }
                    else if (KernelCheck == KernelVersion::ElCapitan){
                        const uint8_t find[]    = {0x41, 0x39, 0xC4, 0x76, 0x3E};
                        const uint8_t replace[] = {0x41, 0x39, 0xC4, 0xEB, 0x3E};
                        KextPatch kext_patch {
                            {&kextList[i], find, replace, sizeof(find), 1},
                            KernelVersion::ElCapitan, KernelVersion::ElCapitan
                        };
                        applyPatches(patcher, index, &kext_patch, 1);
                        progressState |= ProcessingState::GraphicsFramebufferPatched;
                        DBGLOG("igdvmt @ Broadwell - 10.11 :: DVMT patches applied");
                    }
                    else if (KernelCheck == KernelVersion::Sierra){
                        const uint8_t find[]    = {0x89, 0x45, 0xC8, 0x39, 0xC7, 0x76, 0x4F};
                        const uint8_t replace[] = {0x89, 0x45, 0xC8, 0x39, 0xC7, 0xEB, 0x4F};
                        KextPatch kext_patch {
                            {&kextList[i], find, replace, sizeof(find), 1},
                            KernelVersion::Sierra, KernelVersion::Sierra
                        };
                        applyPatches(patcher, index, &kext_patch, 1);
                        progressState |= ProcessingState::GraphicsFramebufferPatched;
                        DBGLOG("igdvmt @ Broadwell - 10.12 :: DVMT patches applied");
                    }
                }
                else if (!(progressState & ProcessingState::GraphicsFramebufferPatched) && !strcmp(kextList[i].id, "com.apple.driver.AppleIntelSKLGraphicsFramebuffer")) {
                    DBGLOG("igdvmt @ found com.apple.driver.AppleIntelSKLGraphicsFramebuffer");
                    if (KernelCheck == KernelVersion::ElCapitan){
                        const uint8_t find[]    = {0x41, 0x39, 0xC4, 0x76, 0x2A};
                        const uint8_t replace[] = {0x41, 0x39, 0xC4, 0xEB, 0x2A};
                        KextPatch kext_patch {
                            {&kextList[i], find, replace, sizeof(find), 1},
                            KernelVersion::ElCapitan, KernelVersion::ElCapitan
                        };
                        applyPatches(patcher, index, &kext_patch, 1);
                        progressState |= ProcessingState::GraphicsFramebufferPatched;
                        DBGLOG("igdvmt @ Skylake - 10.11 :: DVMT patches applied");
                    }
                    else if (KernelCheck == KernelVersion::Sierra){
                        const uint8_t find[]    = {0x89, 0x45, 0xC8, 0x39, 0xC6, 0x76, 0x51};
                        const uint8_t replace[] = {0x89, 0x45, 0xC8, 0x39, 0xC6, 0xEB, 0x51};
                        KextPatch kext_patch {
                            {&kextList[i], find, replace, sizeof(find), 1},
                            KernelVersion::Sierra, KernelVersion::Sierra
                        };
                        applyPatches(patcher, index, &kext_patch, 1);
                        progressState |= ProcessingState::GraphicsFramebufferPatched;
                        DBGLOG("igdvmt @ Skylake - 10.12 :: DVMT patches applied");
                    }
                }
            }
        }
    }
	
	// Ignore all the errors for other processors
	patcher.clearError();
}

void IGDVMT::applyPatches(KernelPatcher &patcher, size_t index, const KextPatch *patches, size_t patchNum) {
    DBGLOG("igdvmt @ applying patches for %zu kext", index);
    for (size_t p = 0; p < patchNum; p++) {
        auto &patch = patches[p];
        if (patch.patch.kext->loadIndex == index) {
            if (patcher.compatibleKernel(patch.minKernel, patch.maxKernel)) {
                DBGLOG("igdvmt @ applying %zu patch for %zu kext", p, index);
                patcher.applyLookupPatch(&patch.patch);
                // Do not really care for the errors for now
                patcher.clearError();
            }
        }
    }
}

