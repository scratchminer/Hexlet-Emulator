/* Main source file for Hexlet's sample SDL2 driver */

#include <stdio.h>
#include <string.h>

#include <hexlet_ints.h>
#include <hexlet_bools.h>
#include <hexlet_driver.h>
#include <hexlet_emulate.h>
#include <hexlet_version.h>

#include "logger.h"

/* Various command line arguments */
boolean drv_nintendoControllerMap = FALSE;
u8 drv_displayScale = 1;
u8 drv_usedHiveCraftVersion;

static inline void drv_logDesc(void) {
	char version[128];
	char versionNum[16];
	
	snprintf(version, sizeof(version), "Hexlet v%s", ver_getVersionString(versionNum, ver_getLatestVersion()));
	
	log_printInfo("");
	log_printInfo(version);
	log_printInfo("A Hexheld emulator with a focus on performance and optimization");
	log_printInfo("By scratchminer (https://github.com/scratchminer)");
	log_printInfo("");
}

/*
*  Return a negative number on failure, 0 for an immediate exit, and a positive number on success.
*/
s32 drv_parseArgs(s32 argc, char **argv) {
	boolean parseVersion = FALSE;
	boolean parseScale = FALSE;
	s32 exitCode = 0;
	
	if(argc == 0) {
		drv_logDesc();
		
		log_printInfo("Usage: hexlet [options] [input file]");
		log_printInfo("");
		
		return 0;
	}
	
	for(s32 c = 0; c < argc; ++c) {
		char *arg = argv[c];
		
		if(parseVersion) {
			s32 version = emu_decodeConstant(arg);
			if(strcmp(emu_getError(), "") && !version || version > ver_emulatorVersions[drv_usedHiveCraftVersion].maxHiveCraftVersion) {
				log_printError("Invalid SoC version number.");
				exitCode = -1;
			}
			else {
				drv_usedHiveCraftVersion = (u8)version;
			}
			
			parseVersion = FALSE;
			continue;
		}
		
		if(parseScale) {
			s32 scale = emu_decodeConstant(arg);
			if(strcmp(emu_getError(), "") && !scale || scale > 4 || scale < 1) {
				log_printError("Invalid display scale (should be between 1 and 4).");
				exitCode = -1;
			}
			else {
				drv_displayScale = (u8)scale;
			}
			
			parseScale = FALSE;
			continue;
		}
		
		if(arg[0] == '-' && arg[1] != '-') ++arg;
		
		if(arg[0] == 'h' || !strcmp(arg, "--help")) {
			drv_logDesc();
			
			log_printInfo("Usage: hexlet [options] [input file]");
			log_printInfo("");
			
			log_printInfo("Command line options:");
			log_startTable(2);
			log_printTableRow("-");
			log_printTable("--help, -h", 		"Display this message and exit");
			log_printTable("--version, -v", 	"Display detailed version information and exit");
			log_printTable("--credits, -c", 	"Display credits and exit");
			log_printTable("--bindings, -b", 	"Display key and controller bindings and exit");
			log_printTable("--asm, -a", 		"Assemble the input file and print the output");
			log_printTable("--disasm, -d", 		"Disassemble the input file and print the output");
			log_printTable("--launch, -l", 		"Launch (assemble and run) the input file");
			log_printTableRow(" ");
			log_printTable("--soc <version>",	"Perform the task using the HiveCraft version specified");
			log_printTable("--scale 2, -2", 	"Upscale the display by a factor of 2");
			log_printTable("--scale 3, -3", 	"Upscale the display by a factor of 3");
			log_printTable("--scale 4, -4", 	"Upscale the display by a factor of 4");
			log_printTable("--ninmap, -n", 		"Make the controller bindings friendlier to Nintendo controllers");
			log_endTable();
			log_printInfo("");
			
			return 0;
		}
		else if(arg[0] == 'v' || !strcmp(arg, "--version")) {
			drv_logDesc();
			
			char majorVersion[128];
			char minorVersion[128];
			char buildVersion[128];
			char hiveCraftVersion[128];
			
			snprintf(majorVersion, sizeof(majorVersion), "Hexlet major version number: %i ($%01X)", ver_CURRENT_MAJOR_VERSION(), ver_CURRENT_MAJOR_VERSION());
			snprintf(minorVersion, sizeof(minorVersion), "Hexlet minor version number: %i ($%01X)", ver_CURRENT_MINOR_VERSION(), ver_CURRENT_MINOR_VERSION());
			snprintf(buildVersion, sizeof(buildVersion), "Hexlet build version number: %i ($%01X)", ver_CURRENT_BUILD_VERSION(), ver_CURRENT_BUILD_VERSION());
			snprintf(hiveCraftVersion, sizeof(hiveCraftVersion), "Latest HiveCraft version supported: %i (--soc $%02X)", ver_MAX_HIVECRAFT_VERSION(), ver_MAX_HIVECRAFT_VERSION());
			
			log_printInfo(majorVersion);
			log_printInfo(minorVersion);
			log_printInfo(buildVersion);
			log_printInfo("");
			
			log_printInfo(hiveCraftVersion);
			log_printInfo("");
			
			return 0;
		}
		else if(arg[0] == 'c' || !strcmp(arg, "--credits")) {
			drv_logDesc();
			
			log_printInfo("Hexheld fantasy console specification by:");
			log_printInfo("\tThe Beesh-Spweesh! (https://github.com/StinkerB06)");
			log_printInfo("\tjvsTSX (https://github.com/jvsTSX)");
			log_printInfo("\tKagamiin~ (https://github.com/Kagamiin)");
			log_printInfo("");
			
			return 0;
		}
		
		else if(arg[0] == 'n' || !strcmp(arg, "--ninmap")) {
			drv_nintendoControllerMap = TRUE;
			continue;
		}
		else if(arg[0] == 'b' || !strcmp(arg, "--bindings")) {
			drv_logDesc();
			
			log_printInfo("General Purpose Controls:");
			log_startTable(3);
			log_printTable("Keyboard",	"Controller",				"Action");
			log_printTableRow("-");
			log_printTable("6",		"Home or Guide",			"Toggle Display Mode (horiz/vert)");
			log_printTable("Esc",		"+ or Start",				"Pause button");
			log_printTable("Space",		"- or Back",				"Select button");
			log_printTable("-",		"L, L1, or LB",				"- button");
			log_printTable("=",		"R, R1, or RB",				"+ button");
			log_printTable("0",		"Capture, Mic, Share, or touchpad", 	"Adjust button");
			log_endTable();
			log_printInfo("");
			
			log_printInfo("Left Controller:");
			log_startTable(3);
			log_printTable("Keyboard",	"Controller",					"Action");
			log_printTableRow("-");
			log_printTable("T",		"Left Stick Press",				"Cycle controller type");
			log_printTable("Q",		"ZL, L2, or LT",				"Trigger button (all controllers)");
			
			log_printTable("W/A/S/D",	"Left Stick or Up/Left/Down/Right",		"D-pad (D-pad Controller)");
			
			if(!drv_nintendoControllerMap) {
				log_printTable("Z/X/A/S",	"Down/Right/Left/Up",				"A/B/1/2 buttons (Button Controller)");
			}
			else {
				log_printTable("Z/X/A/S",	"Right/Down/Up/Left",				"A/B/1/2 buttons (Button Controller)");
			}
	
			log_printTable("A/S",		"Left/Up or rotate Left Stick",			"Paddle CCW/CW (Paddle Controller)");
			log_printTable("Z/X",		"Down/Right",					"A/B buttons (Paddle Controller)");
			log_printTable("Z/X",		"Right/Down",					"A/B buttons (Paddle Controller)");
			log_endTable();
			log_printInfo("");
			
			log_printInfo("Right Controller:");
			log_startTable(3);
			log_printTable("Keyboard",	"Controller",					"Action");
			log_printTableRow("-");
			log_printTable("Y",		"Right Stick Press",				"Cycle controller type");
			log_printTable("P",		"ZR, R2, or RT",				"Trigger button (all controllers)");
			
			log_printTable("I/J/K/L",	"Right Stick or Up/Left/Down/Right",		"D-pad (D-pad Controller)");
			
			if(!drv_nintendoControllerMap) {
				log_printTable(",/./K/L",	"A/B/X/Y (or X/O/Square/Triangle)",		"A/B/1/2 buttons (Button Controller)");
			}
			else {
				log_printTable(",/./K/L",	"A/B/X/Y",					"A/B/1/2 buttons (Button Controller)");
			}
			
			if(!drv_nintendoControllerMap) {
				log_printTable("K/L",		"X/Y (or Square/Triangle)",		"Paddle CCW/CW (Paddle Controller)");
				log_printTable("",		"Rotate Left Stick",			"");
			}
			else {
				log_printTable("K/L",		"Y/X or rotate Left Stick",		"Paddle CCW/CW (Paddle Controller)");
			}
			log_printTable(",/.",		"A/B",						"A/B buttons (Paddle Controller)");
			log_endTable();
			log_printInfo("");
			
			return 0;
		}
		else if(arg[0] == 'a' || !strcmp(arg, "--asm")) {
			
			return 0;
		}
		else if(arg[0] == 'd' || !strcmp(arg, "--disasm")) {
			
			return 0;
		}
		else if(arg[0] == 'l' || !strcmp(arg, "--launch")) {
			
			return 1;
		}
		else if(!strcmp(arg, "--soc")) {
			parseVersion = TRUE;
			continue;
		}
		else if(!strcmp(arg, "--scale")) {
			parseScale = TRUE;
			continue;
		}
		else if(arg[0] == '2') {
			drv_displayScale = 2;
			continue;
		}
		else if(arg[0] == '3') {
			drv_displayScale = 3;
			continue;
		}
		else if(arg[0] == '4') {
			drv_displayScale = 4;
			continue;
		}
	}
	
	return exitCode;
}

int main(int argc, char **argv) {
	drv_usedHiveCraftVersion = ver_MAX_HIVECRAFT_VERSION();
	
	u32 exitCode = drv_parseArgs((s32)argc, argv);
	if(!exitCode) return 0;
	else if(exitCode < 0) {
		log_printError("Command line argument parsing failed.");
		return -1;
	}
	
	// gfx_initDriver(drv_displayScale);
	
	return 0;
}