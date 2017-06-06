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

static const char *kextKBLGraphicsFramebuffer[] {
    "/System/Library/Extensions/AppleIntelSKLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelSKLGraphicsFramebuffer" };

static KernelPatcher::KextInfo kextList[] {
    { "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", kextBDWGraphicsFramebuffer, 1, true, {}, KernelPatcher::KextInfo::Unloaded },
    { "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", kextSKLGraphicsFramebuffer, 1, true, {}, KernelPatcher::KextInfo::Unloaded },
    { "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", kextSKLGraphicsFramebuffer, 1, true, {}, KernelPatcher::KextInfo::Unloaded },
};

static size_t kextListSize {3};

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
                    /*if (KernelCheck == KernelVersion::Yosemite){
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
                    }*/
                    const uint8_t find[]    = {0x00, 0x02, 0x02, 0x02, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x20, 0x02};
                    const uint8_t replace[] = {0x00, 0x02, 0x02, 0x02, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch {
                        {&kextList[i], find, replace, sizeof(find), 2},
                        KernelVersion::Yosemite, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Broadwell - 00020202 00002002 00002002 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find1[]    = {0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xF0, 0x00};
                    const uint8_t replace1[] = {0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch1 {
                        {&kextList[i], find1, replace1, sizeof(find1), 7},
                        KernelVersion::Yosemite, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch1, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Broadwell - 00030303 00000001 0000F000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find2[]    = {0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x50, 0x01};
                    const uint8_t replace2[] = {0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch2 {
                        {&kextList[i], find2, replace2, sizeof(find2), 2},
                        KernelVersion::Yosemite, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch2, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Broadwell - 00030303 00002002 00005001 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find3[]    = {0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x60, 0x02, 0x00, 0x00, 0x60, 0x02};
                    const uint8_t replace3[] = {0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch3 {
                        {&kextList[i], find3, replace3, sizeof(find3), 1},
                        KernelVersion::Yosemite, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch3, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Broadwell - 00030303 00006002 00006002 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find4[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x50, 0x01};
                    const uint8_t replace4[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch4 {
                        {&kextList[i], find4, replace4, sizeof(find4), 7},
                        KernelVersion::Yosemite, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch4, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Broadwell - 01030303 00002002 00005001 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find5[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x60, 0x02, 0x00, 0x00, 0x50, 0x01};
                    const uint8_t replace5[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch5 {
                        {&kextList[i], find5, replace5, sizeof(find5), 2},
                        KernelVersion::Yosemite, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch5, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Broadwell - 01030303 00006002 00005001 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find6[]    = {0x01, 0x03, 0x04, 0x03, 0x00, 0x00, 0x60, 0x02, 0x00, 0x00, 0x50, 0x01};
                    const uint8_t replace6[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch6 {
                        {&kextList[i], find6, replace6, sizeof(find6), 1},
                        KernelVersion::Yosemite, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch6, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Broadwell - 01030403 00002002 00005001 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                }
                else if (!(progressState & ProcessingState::GraphicsFramebufferPatched) && !strcmp(kextList[i].id, "com.apple.driver.AppleIntelSKLGraphicsFramebuffer")) {
                    DBGLOG("igdvmt @ found com.apple.driver.AppleIntelSKLGraphicsFramebuffer");
                    /*if (KernelCheck == KernelVersion::ElCapitan){
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
                    }*/
                    const uint8_t find[]    = {0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x60, 0x02, 0x00, 0x00, 0x00, 0x00};
                    const uint8_t replace[] = {0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch {
                        {&kextList[i], find, replace, sizeof(find), 1},
                        KernelVersion::ElCapitan, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Skylake - 01010101 00006002 00000000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find1[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00};
                    const uint8_t replace1[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch1 {
                        {&kextList[i], find1, replace1, sizeof(find1), 2},
                        KernelVersion::ElCapitan, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch1, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Skylake - 01030303 00002002 00000000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find2[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x50, 0x01};
                    const uint8_t replace2[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch2 {
                        {&kextList[i], find2, replace2, sizeof(find2), 7},
                        KernelVersion::ElCapitan, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch2, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Skylake - 01030303 00002002 00005001 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find3[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00};
                    const uint8_t replace3[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch3 {
                        {&kextList[i], find3, replace3, sizeof(find3), 1},
                        KernelVersion::ElCapitan, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch3, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Skylake - 01030303 00008002 00000000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find4[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x90, 0x03, 0x00, 0x00, 0x00, 0x00};
                    const uint8_t replace4[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch4 {
                        {&kextList[i], find4, replace4, sizeof(find4), 3},
                        KernelVersion::ElCapitan, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch4, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Skylake - 01030303 00009003 00000000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find5[]    = {0x01, 0x03, 0x04, 0x04, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00};
                    const uint8_t replace5[] = {0x01, 0x03, 0x04, 0x04, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch5 {
                        {&kextList[i], find5, replace5, sizeof(find5), 1},
                        KernelVersion::ElCapitan, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch5, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Skylake - 01030404 00002002 00000000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                }
                else if (!(progressState & ProcessingState::GraphicsFramebufferPatched) && !strcmp(kextList[i].id, "com.apple.driver.AppleIntelKBLGraphicsFramebuffer")) {
                    DBGLOG("igdvmt @ found com.apple.driver.AppleIntelKBLGraphicsFramebuffer");
                    /*if (KernelCheck == KernelVersion::Sierra){
                     const uint8_t find[]    = {0x4C, 0x89, 0x5D, 0xC0, 0x76, 0x46};
                     const uint8_t replace[] = {0x4C, 0x89, 0x5D, 0xC0, 0xEB, 0x46};
                     KextPatch kext_patch {
                     {&kextList[i], find, replace, sizeof(find), 1},
                     KernelVersion::Sierra, KernelVersion::Sierra
                     };
                     applyPatches(patcher, index, &kext_patch, 1);
                     progressState |= ProcessingState::GraphicsFramebufferPatched;
                     DBGLOG("igdvmt @ Kabylake - 10.13 :: DVMT patches applied");
                     }*/
                    const uint8_t find[]    = {0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x60, 0x02, 0x00, 0x00, 0x00, 0x00};
                    const uint8_t replace[] = {0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch {
                        {&kextList[i], find, replace, sizeof(find), 1},
                        KernelVersion::Sierra, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Kabylake - 01010101 00006002 00000000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find1[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00};
                    const uint8_t replace1[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch1 {
                        {&kextList[i], find1, replace1, sizeof(find1), 2},
                        KernelVersion::Sierra, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch1, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Kabylake - 01030303 00002002 00000000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find2[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x60, 0x02, 0x00, 0x00, 0x00, 0x00};
                    const uint8_t replace2[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch2 {
                        {&kextList[i], find2, replace2, sizeof(find2), 2},
                        KernelVersion::Sierra, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch2, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Kabylake - 01030303 00006002 00000000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find3[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x60, 0x02, 0x00, 0x00, 0x50, 0x01};
                    const uint8_t replace3[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch3 {
                        {&kextList[i], find3, replace3, sizeof(find3), 1},
                        KernelVersion::Sierra, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch3, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Kabylake - 01030303 00006002 00005001 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
                    
                    
                    const uint8_t find4[]    = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x90, 0x03, 0x00, 0x00, 0x00, 0x00};
                    const uint8_t replace4[] = {0x01, 0x03, 0x03, 0x03, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x90, 0x00};
                    KextPatch kext_patch4 {
                        {&kextList[i], find4, replace4, sizeof(find4), 2},
                        KernelVersion::Sierra, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &kext_patch4, 1);
                    progressState |= ProcessingState::GraphicsFramebufferPatched;
                    DBGLOG("igdvmt @ Kabylake - 01030303 00009003 00000000 :: minStolenSize patch with 32mb DVMT-prealloc was applied");
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

