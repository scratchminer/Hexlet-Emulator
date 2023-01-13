#include "version.h"

/* version 0.0.1 */
static ver_Version ver_version001 = {
	0x0001,
	0x00,
	0x02
};

static inline void ver_initVersionDatabase() {
	ver_emulatorVersions[0] = ver_version001;
}