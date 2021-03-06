//  This file is part of par2cmdline (a PAR 2.0 compatible file verification and
//  repair tool). See http://parchive.sourceforge.net for details of PAR 2.0.
//
//  Copyright (c) 2003 Peter Brian Clements
//
//  par2cmdline is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  par2cmdline is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "par2cmdline.h"
#include "common.h"

#ifdef _MSC_VER
	#ifdef _DEBUG
		#undef THIS_FILE
		static char THIS_FILE[]=__FILE__;
		#define new DEBUG_NEW
	#endif
#endif

int main(int argc, char *argv[]) {
	#ifdef _MSC_VER
		// Memory leak checking
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | /*_CRTDBG_CHECK_CRT_DF | */_CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

//	printf("argc    = %d\n"  ,argc);
//	for (int i=0;i<argc;i++) { printf("argv[%d] = '%s'\n",i,argv[i]); }

	// Parse the command line
	CommandLine *commandline       = new CommandLine;
	bool         CommandLineResult = false;

	Result result     = eInvalidCommandLineArguments;
	CommandLineResult = commandline->Parse(argc, argv);

	if (commandline->GetDisplayHelp()) {
		switch (commandline->GetOperation()) {
			case CommandLine::opCreate : CommandLine::usage_HelpCreate(); break;
			case CommandLine::opRepair : CommandLine::usage_HelpRepair(); break;
			case CommandLine::opVerify : CommandLine::usage_HelpVerify(); break;
			default                    :                                  break;
		}
	} else if (!CommandLineResult) {
		CommandLine::banner();
		CommandLine::usage ();
	} else {
		if ((commandline->GetVerboseLevel() & VERBOSE_HIDE_BANNER)==0)
			CommandLine::banner();

		// Which operation was selected
		switch (commandline->GetOperation()) {
			//=========================================================================
			// Create recovery data
			//=========================================================================
			case CommandLine::opCreate: {
				Par2Creator *creator = new Par2Creator;
				result = creator->Process(*commandline);
				delete creator;
			}
			break;

			//=========================================================================
			// Verify damaged files
			//=========================================================================
			case CommandLine::opVerify: {
				switch (commandline->GetVersion()) {
					case CommandLine::verPar1: {
							Par1Repairer *repairer = new Par1Repairer;
							result = repairer->Process(*commandline, false);
							delete repairer;
						}
						break;

					case CommandLine::verPar2: {
							Par2Repairer *repairer = new Par2Repairer;
							result = repairer->Process(*commandline, false);
							delete repairer;
						}
						break;

					case CommandLine::opNone:
						break;
				}
			}
			break;

			//=========================================================================
			// Repair damaged files
			//=========================================================================
			case CommandLine::opRepair: {
				switch (commandline->GetVersion()) {
					case CommandLine::verPar1: {
						Par1Repairer *repairer = new Par1Repairer;
						result = repairer->Process(*commandline, true);
						delete repairer;
						}
						break;

					case CommandLine::verPar2: {
						Par2Repairer *repairer = new Par2Repairer;
						result = repairer->Process(*commandline, true);
						delete repairer;
						}
						break;

					case CommandLine::opNone:
						break;
				}
			}
			break;

			//=========================================================================
			// Nothing to do
			//=========================================================================
			case CommandLine::opNone:
				result = eSuccess;
				break;
		}
	}

	delete commandline;

	return result;
}
