/* Internal header file for Hexlet's versioning utilities (should not be edited manually) */

#ifndef HEXLET_VER_H_INTERNAL
#define HEXLET_VER_H_INTERNAL

#include <hexlet_version.h>

/* Versions */
ver_Version ver_emulatorVersions[ver_LATEST_VERSION] = {
	{
		0x001,		/* Version number */
		0x00,		/* Maximum HiveCraft version */
		0x02		/* Maximum chip type enum value */
	},
};

#endif