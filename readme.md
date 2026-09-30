# EaglePatch+AC1

EaglePatch+AC1 is an ASI plugin that fixes a number of issues in Assassin's Creed: Director's Cut Edition

**Plugin version:** 1.4

<img src="/assets/EaglePatch+AC1.png" alt="EaglePatch+AC1">

## Features
- Improved shadow map size from 1024 to 4096 (only when shadow quality is maxed out in game's settings; can be disabled in ini)
- Added high-precision framerate limiter (`FramerateLimit`) with optimized CPU sleep thresholds to prevent physics glitches on high refresh rate monitors (>60 FPS)
- Unlocked multisampling on high resolutions
- Added support of all XInput controllers, including PS4/PS5 controllers via standard wrappers/Steam Input (can be disabled in ini)
- Added dynamic 4-slot XInput auto-reconnect scanning for seamless controller hotplugging
- Robust controller hotplugging with null-safety protection (prevents crashes on controller disconnect/reconnect)
- Removed support of DInput controllers (can be disabled in ini; FPS limiter can remain active independently)
- Removed telemetry (improved bypass in version 1.2)
- Gamepad and kb/mouse controls work at the same time (read note below)
- Added ini setting for enabling PS3/PS4/PS5-like controls (swapping bumpers and triggers)
- Added ini setting to skip intro videos (without having to rename files)
- Added ini setting to limit CPU cores the game can use (fixes game crashing at startup when using a CPU with a high core count)
- Improved thread-safety in memory patching with instruction cache coherence (prevents random startup crashes)
- Full compatibility and cross-compilation support on Linux / Wine / Steam Deck (Proton)
- [DX10 only] Fixed doubling of resolution modes in settings
- Added optional diagnostic logging (`EnableLogging`) to troubleshoot startup and controller detection

Note: switching controls layout in game settings has no effect anymore, if you didn't use KeyboardMouse2 layout for keyboard, you need to change KeyboardLayout setting in EaglePatchAC1.ini

## Installation
1. Download latest release of EaglePatch+AC1.zip: https://github.com/Rodrigo-Matsuura/EaglePatch/releases
2. Download Ultimate ASI Loader: https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/ OBS: Download the Ultimate-ASI-Loader.zip
3. Put dinput8.dll from Ultimate ASI Loader into your Assassin's Creed: Director's Cut Edition directory
4. Extract the contents of EaglePatch+AC1.zip directly into your Assassin's Creed: Director's Cut Edition directory
5. Done! You may want to check scripts/EaglePatchAC1.ini for any settings you want to enable or disable

## Confirmed supported exes
#### AssassinsCreed_Dx9.exe (GOG/Steam)
CRC32: E8936C99  
MD5: 8E72C3333743780E43BC2C34BBF625F9  
SHA-1: A9EA476E70F076709CCA59A802E26EC894432120  

#### AssassinsCreed_Dx10.exe (GOG/Steam)
CRC32: 3AF8F9D0  
MD5: CA87753255E2D14B1F18BB737C643792  
SHA-1: 601475FD5EFA02BBAE80D1C06DFA25A0A1E7FD2F  

# EaglePatch+AC2

EaglePatch+AC2 is an ASI plugin that fixes a number of issues in Assassin's Creed II

**Plugin version:** 1.4

<img src="/assets/EaglePatch+AC2.png" alt="EaglePatch+AC2">

## Features
- Improved shadow map size from 1024 to 4096 (only when shadow quality is maxed out in game's settings; can be disabled in ini)
- Draw distance maxed out for buildings, peds and flags (can be disabled in ini)
- Added high-precision framerate limiter (`FramerateLimit`) with optimized CPU sleep thresholds to prevent physics glitches on high refresh rate monitors (>60 FPS)
- Added borderless windowed mode option (`BorderlessWindow`) with frame refresh in ini (currently supported on AC2 Digital UPlay)
- Enables UPlay bonuses - Auditore Crypt, Altair robes, extra knives, bonus dye (can be disabled in ini)
- Added support of all XInput controllers, including PS4/PS5 controllers via standard wrappers/Steam Input (can be disabled in ini)
- Added dynamic 4-slot XInput auto-reconnect scanning for seamless controller hotplugging
- Robust controller hotplugging with null-safety protection (prevents crashes on controller disconnect/reconnect)
- Removed support of DInput controllers (can be disabled in ini; FPS limiter can remain active independently)
- Gamepad and kb/mouse controls work at the same time (read note below)
- Added ini setting for enabling PS3/PS4/PS5-like controls (swapping bumpers and triggers)
- Added ini setting to skip intro videos (without having to rename files)
- Added ini setting to limit CPU cores the game can use (fixes game crashing at startup when using a CPU with a high core count)
- Improved thread-safety in memory patching with instruction cache coherence (prevents random startup crashes)
- Full compatibility and cross-compilation support on Linux / Wine / Steam Deck (Proton)
- Added optional diagnostic logging (`EnableLogging`) to troubleshoot startup and controller detection

Note: switching controls layout in game settings has no effect anymore, if you didn't use KeyboardMouse2 layout for keyboard, you need to change KeyboardLayout setting in EaglePatchAC2.ini

## Installation
1. Download latest release of EaglePatch+AC2.zip: https://github.com/Rodrigo-Matsuura/EaglePatch/releases
2. Download Ultimate ASI Loader: https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/ OBS: Download the Ultimate-ASI-Loader.zip
3. Put dinput8.dll from Ultimate ASI Loader into your Assassin's Creed II directory
4. Extract the contents of EaglePatch+AC2.zip directly into your Assassin's Creed II directory
5. Done! You may want to check scripts/EaglePatchAC2.ini for any settings you want to enable or disable

## Supported game versions
- Digital UPlay
- Retail Akella v1.01

These plugins are using a lightweight win32 patcher by withmorten: https://github.com/withmorten/patcher

---

This project is a fork of the original work by Sergeanur: https://github.com/Sergeanur/EaglePatch

**Project Maintainer (Fork):**
Github: https://github.com/Rodrigo-Matsuura/
