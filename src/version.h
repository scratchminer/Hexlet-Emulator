/* Internal header file for Hexlet's versioning utilities (should not be edited manually) */

#ifndef HEXLET_VER_H_INTERNAL
#define HEXLET_VER_H_INTERNAL

#include <hexlet_version.h>

/* Versions */
static ver_Version ver_version001 = {
	0x001,		/* Version number */
	0x00,		/* Maximum HiveCraft version */
	0x02		/* Maximum chip type enum value */
};

/*
*  Initialize the version database.
*  This has to be done at runtime because the compiler might not let it through otherwise.
*/
static inline void ver_initVersionDatabase() {
	ver_emulatorVersions[0x000] = ver_version001;
}

#endif