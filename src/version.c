/* Source file for Hexlet's versioning utilities */

#include "version.h"

ver_Version *ver_getLatestVersion() {
	return &ver_emulatorVersions[ver_LATEST_VERSION - 1];
}

/* more to come? */