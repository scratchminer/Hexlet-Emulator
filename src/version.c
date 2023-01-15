/* Source file for Hexlet's versioning utilities */

#include <stdio.h>

#include "version.h"

char *ver_getVersionString(char *dest, ver_Version *version) {
	snprintf(dest, sizeof(dest), "%i.%i.%i",
		ver_MAJOR_VERSION(version->versionNumber),
		ver_MINOR_VERSION(version->versionNumber),
		ver_BUILD_VERSION(version->versionNumber)
	);
	return dest;
}

ver_Version *ver_getLatestVersion() {
	return &ver_emulatorVersions[ver_LATEST_VERSION - 1];
}