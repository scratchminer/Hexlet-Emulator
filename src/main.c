/* Main source file for Hexlet */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <hexlet_ints.h>
#include <hexlet_bools.h>

#include <hexlet_driver.h>

#include "assembler.h"
//#include "logger.h"
#include "version.h"

/* Various command line arguments */
boolean main_nintendoControllerMap = FALSE;
u8 main_displayScale = 1;
u8 main_usedHiveCraftVersion;

void main_logDesc(void) {
	log_printInfo("Hexlet v%s", ver_CURRENT_VERSION_STRING());
	log_printInfo("A Hexheld emulator with a focus on performance and optimization");
	log_printInfo("By scratchminer (https://github.com/scratchminer)");
	log_printInfo("");
}

/*
*  Return a negative number on failure, 0 for an immediate exit, and a positive number on success.
*/
s32 main_parseArgs(s32 argc, char **argv) {
	boolean parseVersion = FALSE;
	boolean parseScale = FALSE;
	s32 exitCode = 0; 
	
	for(s32 c = 0; c < argc; ++c) {
		char *arg = argv[c];
		
		if(parseVersion) {
			s32 version = asm_decodeConstant(arg);
			if(strcmp(asm_getError(), "") && !version || version > ver_emulatorVersions[main_usedHiveCraftVersion].maxHiveCraftVersion) {
				log_printError("Invalid SoC version number.");
				exitCode = -1;
			}
			else {
				main_usedHiveCraftVersion = (u8)version;
			}
			
			parseVersion = FALSE;
			continue;
		}
		
		if(parseScale) {
			s32 scale = asm_decodeConstant(arg);
			if(strcmp(asm_getError(), "") && !scale || scale > 4 || scale < 1) {
				log_printError("Invalid display scale (should be between 1 and 4).");
				exitCode = -1;
			}
			else {
				main_displayScale = (u8)scale;
			}
			
			parseScale = FALSE;
			continue;
		}
		
		if(arg[0] == '-' && arg[1] != '-') ++arg;
		
		if(arg[0] == 'h' || !strcmp(arg, "--help")) {
			main_logDesc();
			
			log_printInfo("Usage: hexlet [options] [input file]");
			log_printInfo("");
			
			log_printInfo("Command line options:");
			log_startTable(2);
			log_printTable("--help, -h", 		"Display this message and exit");
			log_printTable("--version, -v", 	"Display detailed version information and exit");
			log_printTable("--credits, -c", 	"Display credits and exit");
			log_printTable("--bindings, -b", 	"Display key and controller bindings and exit");
			log_printTable("--asm, -a", 		"Assemble the input file and print the output");
			log_printTable("--disasm, -d", 		"Disassemble the input file and print the output");
			log_printTable("--launch, -l", 		"Launch (assemble and run) the input file");
			log_printTableRow(" ");
			log_printTable("--soc <version>"	"Perform the task using the HiveCraft system-on-chip version specified");
			log_printTable("--scale 2, -2", 	"Upscale the display by a factor of 2");
			log_printTable("--scale 3, -3", 	"Upscale the display by a factor of 3");
			log_printTable("--scale 4, -4", 	"Upscale the display by a factor of 4");
			log_printTable("--ninmap, -n", 		"Make the controller bindings friendlier to Nintendo controllers (-n -b for altered bindings)");
			log_endTable();
			log_printInfo("");
			
			return 0;
		}
		else if(arg[0] == 'v' || !strcmp(arg, "--version")) {
			main_logDesc();
			
			log_printInfo("Hexlet major version number: %i ($%1X)", ver_CURRENT_MAJOR_VERSION(), ver_CURRENT_MAJOR_VERSION());
			log_printInfo("Hexlet minor version number: %i ($%1X)", ver_CURRENT_MINOR_VERSION(), ver_CURRENT_MINOR_VERSION());
			log_printInfo("Hexlet build version number: %i ($%1X)", ver_CURRENT_BUILD_VERSION(), ver_CURRENT_BUILD_VERSION());
			log_printInfo("");
			
			log_printInfo("Latest HiveCraft version supported: %i (--soc $%2X)", ver_MAX_HIVECRAFT_VERSION(), ver_MAX_HIVECRAFT_VERSION());
			log_printInfo("");
			
			return 0;
		}
		else if(arg[0] == 'c' || !strcmp(arg, "--credits")) {
			main_logDesc();
			
			log_printInfo("Hexheld fantasy console specification by:");
			log_indent();
			log_printInfo("The Beesh-Spweesh! (https://github.com/StinkerB06)");
			log_printInfo("jvsTSX (https://github.com/jvsTSX)");
			log_printInfo("Kagamiin~ (https://github.com/Kagamiin)");
			log_dedent();
			log_printInfo("");
			
			return 0;
		}
		
		else if(arg[0] == 'n' || !strcmp(arg, "--ninmap")) {
			main_nintendoControllerMap = TRUE;
			continue;
		}
		else if(arg[0] == 'b' || !strcmp(arg, "--bindings")) {
			main_logDesc();
			
			log_printInfo("General Purpose Controls:");
			log_startTable(3);
			log_printTable("Keyboard",	"Controller",				"Action");
			log_printTableRow("-");
			log_printTable("6",		"Home or Guide",			"Toggle Display Mode (horizontal/vertical)");
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
			log_printTable("T",		"Left Stick Press",				"Cycle controller (none/D-pad/Buttons/Paddle)");
			log_printTable("Q",		"ZL, L2, or LT",				"Trigger button (on all controllers)");
			log_printTableRow(" ");
			
			log_printTable("W/A/S/D",	"Left Stick or Up/Left/Down/Right",		"D-pad (D-pad Controller)");
			log_printTableRow(" ");
			
			if(!main_nintendoControllerMap) {
				log_printTable("Z/X/A/S",	"Down/Right/Left/Up",				"A/B/1/2 buttons (Button Controller)");
			}
			else {
				log_printTable("Z/X/A/S",	"Right/Down/Up/Left",				"A/B/1/2 buttons (Button Controller)");
			}
			log_printTableRow(" ");
	
			log_printTable("A/S",		"Press Left/Up or rotate Left Stick CCW/CW",	"Paddle rotate CCW/CW (Paddle Controller)");
			log_printTable("Z/X",		"Down/Right",					"A/B buttons (Paddle Controller)");
			log_printTable("Z/X",		"Right/Down",					"A/B buttons (Paddle Controller)");
			log_endTable();
			log_printInfo("");
			
			log_printInfo("Right Controller:");
			log_startTable(3);
			log_printTable("Keyboard",	"Controller",					"Action");
			log_printTableRow("-");
			log_printTable("Y",		"Right Stick Press",				"Cycle controller (none/D-pad/Buttons/Paddle)");
			log_printTable("P",		"ZR, R2, or RT",				"Trigger button (on all controllers)");
			log_printTableRow(" ");
			
			log_printTable("I/J/K/L",	"Right Stick or Up/Left/Down/Right",		"D-pad (D-pad Controller)");
			log_printTableRow(" ");
			
			if(!main_nintendoControllerMap) {
				log_printTable(",/./K/L",	"A/B/X/Y (or X/O/Square/Triangle)",		"A/B/1/2 buttons (Button Controller)");
			}
			else {
				log_printTable(",/./K/L",	"A/B/X/Y",					"A/B/1/2 buttons (Button Controller)");
			}
			log_printTableRow(" ");
			
			if(!main_nintendoControllerMap) {
				log_printTable("K/L",		"Press X/Y (or Square/Triangle)",		"Paddle rotate CCW/CW (Paddle Controller)");
				log_printTable("",		"Rotate Left Stick CCW/CW",			"");
			}
			else {
				log_printTable("K/L",		"Press Y/X or rotate Left Stick CCW/CW",	"Paddle rotate CCW/CW (Paddle Controller)");
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
			main_displayScale = 2;
			continue;
		}
		else if(arg[0] == '3') {
			main_displayScale = 3;
			continue;
		}
		else if(arg[0] == '4') {
			main_displayScale = 4;
			continue;
		}
	}
	
	return exitCode;
}

int main(int argc, char **argv) {
	main_usedHiveCraftVersion = ver_MAX_HIVECRAFT_VERSION();
	
	u32 exitCode = main_parseArgs((s32)argc, argv);
	if(!exitCode) return 0;
	else if(exitCode < 0) {
		log_printError("Command line argument parsing failed.");
		return
	}
	
}