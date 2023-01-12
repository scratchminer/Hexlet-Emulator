/* Header file for version capabilities of present and past versions of Hexlet */

#ifndef HEXLET_VERSION_H
#define HEXLET_VERSION_H

#include <hexlet_ints.h>

typedef struct {	
	u16 versionNumber;			/* Version number */
	u8 maxHiveCraftVersion;		/* Maximum supported version of the HiveCraft */
	u8 maxChipType;				/* Maximum value of the ROM loader's ldr_ChipType enumeration */
} ver_Version;

const ver_Version *ver_emulatorVersions = {
	{
		/* version 0.0.1 */
		0x0001,
		0x00,
		0x02
	}
}
const u16 ver_versionCount = 0x001;

/*
*  Macros to help with version decoding.
*/
#define ver_MAJOR_VERSION(VERSION) (u8)(VERSION.versionNumber >> 8)
#define ver_MINOR_VERSION(VERSION) (u8)((VERSION.versionNumber >> 4) & 0xf)
#define ver_BUILD_VERSION(VERSION) (u8)(VERSION.versionNumber & 0xf)

#define ver_CURRENT_MAJOR_VERSION ver_MAJOR_VERSION(ver_getLatestVersion())
#define ver_CURRENT_MINOR_VERSION ver_MINOR_VERSION(ver_getLatestVersion())
#define ver_CURRENT_BUILD_VERSION ver_BUILD_VERSION(ver_getLatestVersion())

#define ver_CURRENT_VERSION_STRING ver_getVersionString(ver_getLatestVersion())

/*
*  Get the running version of Hexlet as a ver_Version.
*/
#define ver_getLatestVersion() ver_emulatorVersions[ver_versionCount - 1]

/*
*  Get the human-readable version string corresponding to the given ver_Version.
*/
const char *ver_getVersionString(ver_Version *version);

#endif