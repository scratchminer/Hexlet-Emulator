/* Header file for version capabilities of present and past versions of Hexlet */

#ifndef HEXLET_VER_H
#define HEXLET_VER_H

#include <hexlet_ints.h>

#define ver_LATEST_VERSION 0x001

typedef struct {	
	u16 versionNumber;			/* Version number */
	u8 maxHiveCraftVersion;			/* Maximum supported version of the HiveCraft */
	u8 maxChipType;				/* Maximum value of the ROM loader's ldr_ChipType enumeration */
} ver_Version;

ver_Version ver_emulatorVersions[ver_LATEST_VERSION];

/*
*  Get the human-readable version string corresponding to the given ver_Version.
*/
char *ver_getVersionString(ver_Version *version);

/*
*  Get the running version of Hexlet as a ver_Version.
*/
ver_Version *ver_getLatestVersion();

/*
*  Macros to help with Hexlet version decoding.
*/
#define ver_MAJOR_VERSION(VERSION_NUMBER) (u8)(VERSION_NUMBER >> 8)
#define ver_MINOR_VERSION(VERSION_NUMBER) (u8)((VERSION_NUMBER >> 4) & 0xf)
#define ver_BUILD_VERSION(VERSION_NUMBER) (u8)(VERSION_NUMBER & 0xf)

/* 
*  Macro to help with HiveCraft version decoding.
*/
#define ver_MAX_HIVECRAFT_VERSION() ver_getLatestVersion()->maxHiveCraftVersion

/*
*  Macros to get the current version numbers.
*/
#define ver_CURRENT_MAJOR_VERSION() ver_MAJOR_VERSION(ver_getLatestVersion()->versionNumber)
#define ver_CURRENT_MINOR_VERSION() ver_MINOR_VERSION(ver_getLatestVersion()->versionNumber)
#define ver_CURRENT_BUILD_VERSION() ver_BUILD_VERSION(ver_getLatestVersion()->versionNumber)
#define ver_CURRENT_VERSION_STRING() ver_getVersionString(ver_getLatestVersion())

#endif