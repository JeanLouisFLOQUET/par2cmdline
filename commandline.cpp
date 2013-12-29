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
#include "config.h"

#ifdef _MSC_VER
	#ifdef _DEBUG
		#undef THIS_FILE
		static char THIS_FILE[]=__FILE__;
		#define new DEBUG_NEW
	#endif
#endif

CommandLine::ExtraFile::ExtraFile(void)
: filename()
, filesize(0) {
}

CommandLine::ExtraFile::ExtraFile(const CommandLine::ExtraFile &other)
: filename(other.filename)
, filesize(other.filesize) {
}

CommandLine::ExtraFile& CommandLine::ExtraFile::operator=(const CommandLine::ExtraFile &other) {
	filename = other.filename;
	filesize = other.filesize;
	return *this;
}

CommandLine::ExtraFile::ExtraFile(const string &name, u64 size)
: filename(name)
, filesize(size) {
}

CommandLine::CommandLine(void)
: operation(opNone)
, version(verUnknown)
, noiselevel(nlUnknown)
, blockcount(0)
, blocksize(0)
, firstblock(0)
, recoveryfilescheme(scUnknown)
, recoveryfilecount(0)
, recoveryblockcount(0)
, recoveryblockcountset(false)
, redundancy(0)
, redundancyset(false)
, parfilename()
, extrafiles()
, totalsourcesize(0)
, largestsourcesize(0)
, RecoveryFileCountAdjust(false)
, memorylimit(0)
, purgefiles(false)
, recursive(false)
, DisplayHelp(false)
, VerboseLevel(0)
{ }

void CommandLine::banner(void) {
	string version = PACKAGE_NAME " version " PACKAGE_VERSION;

	cout << version                                                                       << endl
	     << "Copyright (C) 2003-2005 Peter Brian Clements"                                << endl
	     << "Copyright (C) 2011-2012 Marcel Partap"                                       << endl
	     << "Copyright (C) 2012-2013 Ike Devolder"                                        << endl
	     << "Copyright (C) 2013      Jean-Louis FLOQUET"                                  << endl
	                                                                                      << endl
	     << "par2cmdline comes with ABSOLUTELY NO WARRANTY."                              << endl
	     << "This is free software, and you are welcome to redistribute it and/or modify" << endl
	     << "it under the terms of the GNU General Public License as published by the"    << endl
	     << "Free Software Foundation; either version 2 of the License, or (at your"      << endl
	     << "option) any later version. See COPYING for details."                         << endl;
}

void CommandLine::ParFileNotFound(string parfilename) {
	cerr << "\"" << parfilename << "\" not found" << endl;
}

void CommandLine::usage_BlockProperty(void) {
	cout <<
		"  Block property (choose only one) :"                                                 "\n"
		"     -b<n>  : Set the Block-Count"                                                    "\n"
		"     -s<n>  : Set the Block-Size"                                                     "\n";
}

void CommandLine::usage_Redundancy(void) {
	cout <<
		"  Redundancy (choose only one) :"                                                     "\n"
		"     -r<n>  : Level of Redundancy (%%)"                                               "\n"
		"     -c<n>  : Recovery block count"                                                   "\n";
}

void CommandLine::usage_RecoveryFilesMode(void) {
	cout <<
		"  Recovery files (choose only one) :"                                                 "\n"
		"     -u     : Uniform recovery file sizes"                                            "\n"
		"     -l     : Limit size of recovery files"                                           "\n";
}

void CommandLine::usage_RecoveryFilesNumber(void) {
	cout <<
		"  Number of recovery files"                                                           "\n"
		"     -n<n>  : hard fixed                                 (don't use with -l or -N)"   "\n"
		"     -N<n>  : can be reduced if not enough source blocks (don't use with -l or -n)"   "\n";
}

void CommandLine::usage_Operation(void) {
	cout <<
		"Usage: par2cmdline <Operation> [options] <par2 file> [files]" "\n"
		"  <Operation> is either :"                             "\n"
		"      c(reate) : Create PAR2 files"                    "\n"
		"      v(erify) : Verify files using PAR2 file"         "\n"
		"      r(epair) : Repair files using PAR2 files"        "\n";
}

void CommandLine::usage_HelpMain(void) {
	cout << "Specific options : par2cmdline <Operation> -h\n";
}

void CommandLine::usage_HelpCreate(void) {
	cout << "create\n";
}

void CommandLine::usage_HelpRepair(void) {
	cout << "repair\n";
}

void CommandLine::usage_HelpVerify(void) {
	cout <<
		"You can specifiy -v<xx> verbose value by adding needed sub-values :"      "\n"
		"   Do not display banner               :  1"                              "\n"
		"   Display loading main PAR2 (silent ) :  2"                              "\n"
		"   Display loading main PAR2 (compact) :  4"                              "\n"
  		"   DEBUG                               : xx"                              "\n"
		;
}

void CommandLine::usage(void) {
	                                          cout << "\n";
	CommandLine::usage_Operation          (); cout << "\n";
	CommandLine::usage_HelpMain           (); cout << "\nOptions:\n";
	CommandLine::usage_BlockProperty      ();
	CommandLine::usage_Redundancy         ();
	CommandLine::usage_RecoveryFilesMode  ();
	CommandLine::usage_RecoveryFilesNumber();
	cout <<
		"  Misc :"                                                                             "\n"
		"     -a<f>  : set the main par2 archive name"                                         "\n"
		"     -f<n>  : First Recovery-Block-Number"                                            "\n"
		"     -m<n>  : Memory (in MB) to use"                                                  "\n"
		"     -v [-v]: Be more verbose"                                                        "\n"
		"     -q [-q]: Be more quiet (-q -q gives silence)"                                    "\n"
		"     -p     : Purge backup/par files on successful/useless recovery"                  "\n"
		"     -R     : Recurse into subdirectories (only usefull on create)"                   "\n"
		"     --     : Treat all remaining CommandLine as filenames"                           "\n"
		                                                                                       "\n"
		"If you wish to create par2 files for a single source file, you may leave"             "\n"
		"out the name of the par2 file from the command line."                                 "\n";
}

bool CommandLine::Parse(int argc, char *argv[]) {
	if (argc<1) { return false; }

	// Split the program name into path and filename
	string path, name;
	DiskFile::SplitFilename(argv[0], path, name);
	argc--;
	argv++;

	// Strip ".exe" from the end
	if (name.size() > 4 && 0 == _stricmp(".exe", name.substr(name.length()-4).c_str())) {
		name = name.substr(0, name.length()-4);
	}

	// Check the resulting program name
	     if (0 == _stricmp("par2create", name.c_str())) { operation = opCreate; }
	else if (0 == _stricmp("par2verify", name.c_str())) { operation = opVerify; }
	else if (0 == _stricmp("par2repair", name.c_str())) { operation = opRepair; }

	// Have we determined what operation we want?
	if (operation == opNone) {
		if (argc<2) { cerr << "Not enough command line arguments." << endl; return false; }

		//Determine which operation is requested
		switch (tolower(argv[0][0])) {
			case 'c': if (argv[0][1] == 0 || 0 == _stricmp(argv[0], "create")) { operation = opCreate; } break;
			case 'v': if (argv[0][1] == 0 || 0 == _stricmp(argv[0], "verify")) { operation = opVerify; } break;
			case 'r': if (argv[0][1] == 0 || 0 == _stricmp(argv[0], "repair")) { operation = opRepair; } break;
		}

		//No operation selected => end of execution
		if (operation == opNone) { cerr << "Invalid operation specified: " << argv[0] << endl; return false; }

		argc--;
		argv++;
	}

	bool options = true;

	while (argc>0) {
		if (argv[0][0]) {
			//Non-valid switch (may be the file list, let's later)
			if (options && argv[0][0] != '-') {
				options = false;
			}

			if (options) {
				switch (argv[0][1]) {
				//==============================================================================================================================
				// Set the PAR2 archive name
				//==============================================================================================================================
				case 'a': {
						if (operation == opCreate) {
							string str = argv[0];
							if (str == "-a") { SetParFilename(argv[1]); argc--; argv++; }
							else             { SetParFilename(str.substr(2)); }
						}
					}
					break;
				//==============================================================================================================================
				// Set the block count
				//==============================================================================================================================
				case 'b': {
						if (operation != opCreate) { cerr << "Cannot specify block count unless creating." << endl; return false; }

						     if (blockcount > 0) { cerr << "Cannot specify block count twice."               << endl; return false; }
						else if (blocksize  > 0) { cerr << "Cannot specify both block count and block size." << endl; return false; }

						char *p = &argv[0][2];

						while (blockcount <= 3276 && *p && isdigit(*p)) {
							blockcount = blockcount * 10 + (*p - '0');
							p++;
						}
						if (0 == blockcount || blockcount > 32768 || *p) { cerr << "Invalid block count option: " << argv[0] << endl; return false; }
					}
					break;

				//==============================================================================================================================
				// Set the block size
				//==============================================================================================================================
				case 's': {
						if (operation != opCreate) { cerr << "Cannot specify block size unless creating." << endl; return false; }

						     if (blocksize >0) { cerr << "Cannot specify block size twice."                << endl; return false; }
						else if (blockcount>0) { cerr << "Cannot specify both block count and block size." << endl; return false; }

						char *p = &argv[0][2];
						while (blocksize <= 429496729 && *p && isdigit(*p)) {
							blocksize = blocksize * 10 + (*p - '0');
							p++;
						}
						if (*p || blocksize == 0) { cerr << "Invalid block size option: " << argv[0] << endl; return false; }
						if (      blocksize  & 3) { cerr << "Block size must be a multiple of 4."    << endl; return false; }
					}
					break;

				//==============================================================================================================================
				// Set the amount of redundancy required
				//==============================================================================================================================
				case 'r': {
						if (operation != opCreate)      { cerr << "Cannot specify redundancy unless creating."               << endl; return false; }
						if (redundancyset)              { cerr << "Cannot specify redundancy twice."                         << endl; return false; }
						else if (recoveryblockcountset) { cerr << "Cannot specify both redundancy and recovery block count." << endl; return false; }

						char *p = &argv[0][2];
						while (redundancy <= 10 && *p && isdigit(*p)) {
							redundancy = redundancy * 10 + (*p - '0');
							p++;
						}

						if (redundancy > 100 || *p)                   { cerr << "Invalid redundancy option: "        << argv[0] << endl; return false; }
						if (redundancy == 0 && recoveryfilecount > 0) { cerr << "Cannot set redundancy to 0 and file count > 0" << endl; return false; }
						redundancyset = true;
					}
					break;

				//==============================================================================================================================
				// Set the number of recovery blocks to create
				//==============================================================================================================================
				case 'c': {
						if (operation != opCreate) { cerr << "Cannot specify recovery block count unless creating."     << endl; return false; }
						if (recoveryblockcountset) { cerr << "Cannot specify recovery block count twice."               << endl; return false; }
						else if (redundancyset)    { cerr << "Cannot specify both recovery block count and redundancy." << endl; return false; }

						char *p = &argv[0][2];
						while (recoveryblockcount <= 32768 && *p && isdigit(*p)) {
							recoveryblockcount = recoveryblockcount * 10 + (*p - '0');
							p++;
						}
						if (recoveryblockcount > 32768 || *p)                 { cerr << "Invalid recoveryblockcount option: "        << argv[0] << endl; return false; }
						if (recoveryblockcount == 0 && recoveryfilecount > 0) { cerr << "Cannot set recoveryblockcount to 0 and file count > 0" << endl; return false; }
						recoveryblockcountset = true;
					}
					break;

				//==============================================================================================================================
				// Specify the First block recovery number
				//==============================================================================================================================
				case 'f': {
						if (operation != opCreate) { cerr << "Cannot specify first block number unless creating." << endl; return false; }
						if (firstblock > 0)        { cerr << "Cannot specify first block twice."                  << endl; return false; }

						char *p = &argv[0][2];
						while (firstblock <= 3276 && *p && isdigit(*p)) {
							firstblock = firstblock * 10 + (*p - '0');
							p++;
						}
						if (firstblock > 32768 || *p) { cerr << "Invalid first block option: " << argv[0] << endl; return false; }
					}
					break;

				//==============================================================================================================================
				// Specify uniformly sized recovery files
				//==============================================================================================================================
				case 'u': {
						if (operation != opCreate)           { cerr << "Cannot specify uniform files unless creating."  << endl; return false; }
						if (argv[0][2])                      { cerr << "Invalid option: "                    << argv[0] << endl; return false; }
						if (recoveryfilescheme != scUnknown) { cerr << "Cannot specify two recovery file size schemes." << endl; return false; }
						recoveryfilescheme = scUniform;
					}
					break;

				//==============================================================================================================================
				// Limit the size of the recovery files
				//==============================================================================================================================
				case 'l': {
						if (operation != opCreate          ) { cerr << "Cannot specify limit files unless creating."                       << endl; return false; }
						if (argv[0][2]                     ) { cerr << "Invalid option: "                                       << argv[0] << endl; return false; }
						if (recoveryfilescheme != scUnknown) { cerr << "Cannot specify two recovery file size schemes."                    << endl; return false; }
						if (recoveryfilecount > 0          ) { cerr << "Cannot specify limited size and number of files at the same time." << endl; return false; }
						recoveryfilescheme = scLimited;
					}
					break;

				//==============================================================================================================================
				// Specify the number of recovery files
				//==============================================================================================================================
				case 'n':
				case 'N': {
						if (operation != opCreate                           ) { cerr << "Cannot specify recovery file count unless creating."               << endl; return false; }
						if (recoveryfilecount > 0                           ) { cerr << "Cannot specify recovery file count twice."                         << endl; return false; }
						if (redundancyset && redundancy                 == 0) { cerr << "Cannot set file count when redundancy is set to 0."                << endl; return false; }
						if (recoveryblockcountset && recoveryblockcount == 0) { cerr << "Cannot set file count when recovery block count is set to 0."      << endl; return false; }
						if (recoveryfilescheme == scLimited                 ) { cerr << "Cannot specify limited size and number of files at the same time." << endl; return false; }

						char *p = &argv[0][2];
						while (*p && isdigit(*p)) {
							recoveryfilecount = recoveryfilecount * 10 + (*p - '0');
							p++;
						}
						if (recoveryfilecount == 0 || *p) { cerr << "Invalid recovery file count option: " << argv[0] << endl; return false; }

						if (argv[0][1]=='N') { RecoveryFileCountAdjust = 1; }
						if (argv[0][1]=='n') { RecoveryFileCountAdjust = 0; }
					}
					break;

				//==============================================================================================================================
				// Specify how much memory to use for output buffers
				//==============================================================================================================================
				case 'm': {
						if (memorylimit > 0) { cerr << "Cannot specify memory limit twice." << endl; return false; }

						char *p = &argv[0][2];
						while (*p && isdigit(*p)) {
							memorylimit = memorylimit * 10 + (*p - '0');
							p++;
						}
						if (memorylimit == 0 || *p) { cerr << "Invalid memory limit option: " << argv[0] << endl; return false; }
					}
					break;

				//==============================================================================================================================
				// Verbose
				//==============================================================================================================================
				case 'v': {
						char *p = &argv[0][2];
						while (*p && isdigit(*p)) {
							VerboseLevel = VerboseLevel * 10 + (*p - '0');
							p++;
						}
						/*
						switch (noiselevel) {
							case nlUnknown: {
									if (argv[0][2] == 'v') { noiselevel = nlDebug; }
									else                   { noiselevel = nlNoisy; }
								}
								break;
							case nlNoisy:
							case nlDebug: noiselevel = nlDebug;                                       break;
							default     : cerr << "Cannot use both -v and -q." << endl; return false; break;
						}
						*/
					}
					break;

				//==============================================================================================================================
				// Quiet
				//==============================================================================================================================
				case 'q': {
						switch (noiselevel) {
							case nlUnknown: {
									if (argv[0][2] == 'q') { noiselevel = nlSilent; }
									else                   { noiselevel = nlQuiet ; }
								}
								break;
							case nlQuiet :
							case nlSilent:
								noiselevel = nlSilent;
								break;
							default:
								cerr << "Cannot use both -v and -q." << endl;
								return false;
								break;
						}
					}
					break;

				//==============================================================================================================================
				// Purge backup/par files after usage
				//==============================================================================================================================
				case 'p': {
						if (operation != opRepair && operation != opVerify) {
							cerr << "Cannot specify purge unless repairing or verifying." << endl;
							return false;
						}
						purgefiles = true;
					}
					break;

				//==============================================================================================================================
				// Display help
				//==============================================================================================================================
				case 'h': {
						DisplayHelp = true;
						return false;
					}
					break;

				//==============================================================================================================================
				// Recursive search
				//==============================================================================================================================
				case 'R': {
						if (operation == opCreate) { recursive = true; }
						else                       { cerr << "Recursive has no impact except on creating." << endl; }
					}
					break;
				//==============================================================================================================================
				// Others
				//==============================================================================================================================
				case '-': { argc--; argv++; options = false; continue; } break;

				default: {
						cerr << "Invalid option specified: " << argv[0] << endl;
						return false;
					}
				}
			} else if (parfilename.length() == 0) {
				string filename = argv[0];
				string::size_type where;
				if ((where = filename.find_first_of('*')) != string::npos ||
				    (where = filename.find_first_of('?')) != string::npos) {
					cerr << "par2 file must not have a wildcard in it." << endl;
					return false;
				}

				SetParFilename(filename);
			} else {
				list<string> *filenames = new list<string>;

				// If the argument includes wildcard characters, search the disk for matching files
				if (strchr(argv[0], '*') || strchr(argv[0], '?')) {
					string path;
					string name;
					DiskFile::SplitFilename(argv[0], path, name);
					DiskFile::FindFiles(filenames, basepath, name, recursive);
				} else {
					filenames = new list<string>;
					filenames->push_back(argv[0]);
				}

				list<string>::iterator fn = filenames->begin();
				while (fn != filenames->end()) {
					// Convert filename from command line into a full path + filename
					string filename = DiskFile::GetCanonicalPathname(*fn);

					// If this is the first file on the command line, then it is the main PAR2 file.
					if (parfilename.length() == 0) {

						// If we are verifying or repairing, the PAR2 file must already exist
						if (operation != opCreate) {
							// Find the last '.' in the filename
							string::size_type where = filename.find_last_of('.');
							if (where != string::npos) {
								// Get what follows the last '.'
								string tail = filename.substr(where+1);

								if (0 == _stricmp(tail.c_str(), "par2")) {
									parfilename = filename;
									version     = verPar2;
								} else if (0 == _stricmp(tail.c_str(), "par") ||
												 (tail.size() == 3 &&
												 tolower(tail[0]) == 'p' &&
												 isdigit(tail[1]) &&
												 isdigit(tail[2]))) {
									parfilename = filename;
									version     = verPar1;
								}
							}

							// If we haven't figured out which version of PAR file we
							// are using from the file extension, then presumable the
							// files filename was actually the name of a data file.
							if (version == verUnknown) {
								// Check for the existence of a PAR2 of PAR file.
								     if (DiskFile::FileExists(filename + ".par2")) { version = verPar2; parfilename = filename + ".par2"; }
								else if (DiskFile::FileExists(filename + ".PAR2")) { version = verPar2; parfilename = filename + ".PAR2"; }
								else if (DiskFile::FileExists(filename + ".par" )) { version = verPar1; parfilename = filename + ".par" ; }
								else if (DiskFile::FileExists(filename + ".PAR" )) { version = verPar1; parfilename = filename + ".PAR" ; }
							} else {
								// Does the specified PAR or PAR2 file exist
								if (!DiskFile::FileExists(filename)) { version = verUnknown; }
							}

							if (version == verUnknown) {
								cerr << "The recovery file does not exist: " << filename << endl;
								return false;
							}
						} else {
							// We are creating a new file
							version     = verPar2 ;
							parfilename = filename;
						}
					} else {
						// Verify that full-filename is not too long
						if (filename.length()>_MAX_PATH) {
							cerr << "The source filename is too long (" << filename.length() << ") : \"" << filename << "\"" << endl;
							return false;
						} else 
						// All other files must exist
						if (!DiskFile::FileExists(filename)) {
							cerr << "The source file does not exist: " << filename << endl;
							return false;
						}

						u64 filesize = DiskFile::GetFileSize(filename);

						// Ignore all 0 byte files
						if (filesize > 0) {
							extrafiles.push_back(ExtraFile(filename, filesize));

							// Track the total size of the source files and how big the largest one is.
							totalsourcesize += filesize;
							if (largestsourcesize < filesize) { largestsourcesize = filesize; }
						} else {
							cout << "Skipping 0 byte file: " << filename << endl;
						}
					}

					++fn;
				}
				delete filenames;
			}
		}

		argc--;
		argv++;
	}

	if (parfilename.length() == 0) { cerr << "You must specify a Recovery file." << endl; return false; }

	// Default noise level
	if (noiselevel == nlUnknown) { noiselevel = nlNormal; }

	// If we a creating, check the other parameters
	if (operation == opCreate) {
		// If no recovery file size scheme is specified then use Variable
		if (recoveryfilescheme == scUnknown) {
			recoveryfilescheme = scVariable;
		}

		// If neither block count not block size is specified
		if (blockcount == 0 && blocksize == 0) {
			blockcount = 2000;                                  // Use a block count of 2000
		}

		// If we are creating, the source files must be given.
		if (extrafiles.size() == 0) {
			// Does the par filename include the ".par2" on the end?
			if (parfilename.length() > 5 && 0 == _stricmp(parfilename.substr(parfilename.length()-5, 5).c_str(), ".par2")) {
				// Yes it does.
				cerr << "You must specify a list of files when creating." << endl;
				return false;
			} else {
				// No it does not.

				// In that case check to see if the file exists, and if it does
				// assume that you wish to create par2 files for it.

				u64 filesize = 0;
				if (DiskFile::FileExists(parfilename) && (filesize = DiskFile::GetFileSize(parfilename)) > 0) {
					extrafiles.push_back(ExtraFile(parfilename, filesize));

					// track the total size of the source files and how
					// big the largest one is.
					totalsourcesize += filesize;
					if (largestsourcesize < filesize)
						largestsourcesize = filesize;
				} else {
					// The file does not exist or it is empty.
					cerr << "You must specify a list of files when creating." << endl;
					return false;
				}
			}
		}

		// Strip the ".par2" from the end of the filename of the main PAR2 file.
		if (parfilename.length() > 5 && 0 == _stricmp(parfilename.substr(parfilename.length()-5, 5).c_str(), ".par2")) {
			parfilename = parfilename.substr(0, parfilename.length()-5);
		}

		// Assume a redundancy of 5% if neither redundancy or recoveryblockcount were set.
		if (!redundancyset && !recoveryblockcountset) {
			redundancy = 5;
		}
	}

	// Assume a memory limit of 16MB if not specified.
	if (memorylimit == 0) {
		#ifdef WIN32
			u64 TotalPhysicalMemory = 0;

			HMODULE hLib = ::LoadLibraryA("kernel32.dll");
			if (NULL != hLib) {
				BOOL (WINAPI *pfn)(LPMEMORYSTATUSEX) = (BOOL (WINAPI*)(LPMEMORYSTATUSEX))::GetProcAddress(hLib, "GlobalMemoryStatusEx");

				if (NULL != pfn) {
					MEMORYSTATUSEX mse;
					mse.dwLength = sizeof(mse);
					if (pfn(&mse)) {
						TotalPhysicalMemory = mse.ullTotalPhys;
					}
				}

				::FreeLibrary(hLib);
			}

			if (TotalPhysicalMemory == 0) {
				MEMORYSTATUS ms;
				::ZeroMemory(&ms, sizeof(ms));
				::GlobalMemoryStatus(&ms);

				TotalPhysicalMemory = ms.dwTotalPhys;
			}

			if (TotalPhysicalMemory == 0) {
				TotalPhysicalMemory = 128 * 1048576; // Assume 128MB
			}

			// Half of total physical memory
			memorylimit = (size_t)(TotalPhysicalMemory / 1048576 / 2);
		#else
				memorylimit = 16;
		#endif
	}
	memorylimit *= 1048576;

	return true;
}

void CommandLine::SetParFilename(string filename) {
	parfilename = DiskFile::GetCanonicalPathname(filename);

	// If we are verifying or repairing, the PAR2 file must already exist
	if (operation != opCreate) {
		// Find the last '.' in the filename
		string::size_type where = filename.find_last_of('.');
		if (where != string::npos) {
			// Get what follows the last '.'
			string tail = filename.substr(where+1);

			if (0 == _stricmp(tail.c_str(), "par2")) {
				parfilename = filename;
				version = verPar2;
			} else if (0 == _stricmp(tail.c_str(), "par") ||
			           ( tail.size() == 3 &&
			             tolower(tail[0]) == 'p' &&
			             isdigit(tail[1]) &&
			             isdigit(tail[2])
			           )
			          ) {
				parfilename = filename;
				version = verPar1;
			}
		}

		// If we haven't figured out which version of PAR file we
		// are using from the file extension, then presumable the
		// files filename was actually the name of a data file.
		if (version == verUnknown) {
			// Check for the existence of a PAR2 of PAR file.
			     if (DiskFile::FileExists(filename + ".par2")) { version = verPar2; parfilename = filename + ".par2"; }
			else if (DiskFile::FileExists(filename + ".PAR2")) { version = verPar2; parfilename = filename + ".PAR2"; }
			else if (DiskFile::FileExists(filename + ".par" )) { version = verPar1; parfilename = filename + ".par" ; }
			else if (DiskFile::FileExists(filename + ".PAR" )) { version = verPar1; parfilename = filename + ".PAR" ; }
		} else {
			// Does the specified PAR or PAR2 file exist
			if (!DiskFile::FileExists(filename)) {
				CommandLine::ParFileNotFound(filename);
				version = verNotFound;
			}
		}
	} else {
		version = verPar2;
	}

	string dummy;
	DiskFile::SplitFilename(parfilename, basepath, dummy);
}
