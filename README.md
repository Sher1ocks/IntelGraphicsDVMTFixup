IntelGraphicsDVMTFixup
===================

A common problem with Broadwell/Skylake/KabyLake is relatively small DVMT-prealloc setting by PC OEMs. 
The Apple framebuffer kexts generally assume 64mb or larger, and most PC OEMs use only 32mb.
And often, there is no way to change it easily due to limited BIOS, locked down BIOS, etc.


#### Features
- For users who have no way to change it easily due to limited BIOS, locked down BIOS, etc.
- Fixes an issue related DVMT panic When entering the installation screen.
- No need "FakeID = 0x12345678".


#### Credits
- [Apple](https://www.apple.com) for macOS  
- [vit9696](https://github.com/vit9696) for [Lilu.kext](https://github.com/vit9696/Lilu) & for patch
- [Pike R. Alpha](https://github.com/Piker-Alpha) for patch
- [lvs1974](https://applelife.ru/members/lvs1974.53809/) for original source code and idea
- [Austere.J](http://www.insanelymac.com/forum/user/1432100-austerej/) for original binary patch
- [Rehabman](http://www.insanelymac.com/forum/user/1031260-rehabman/) for maintaining binary patch and [new way patch](https://www.tonymacx86.com/threads/guide-alternative-to-the-minstolensize-patch-with-32mb-dvmt-prealloc.221506/)
- [Sherlocks](http://www.insanelymac.com/forum/user/980913-sherlocks/) for writing the software and maintaining it
