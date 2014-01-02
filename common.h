//  This file is part of par2cmdline (a PAR 2.0 compatible file verification and
//  repair tool). See https://github.com/JeanLouisFLOQUET/par2cmdline for details of PAR 2.0.
//
//  Copyright (c) 2014 Jean-Louis FLOQUET
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

#ifndef __COMMON_H__
	#define __COMMON_H__

	#define VERBOSE_HIDE_BANNER                      1

	#define VERBOSE_CREATE_HIDE_0BYTE                2
	#define VERBOSE_CREATE_HIDE_STATS                4
	#define VERBOSE_CREATE_HIDE_PROGRESS             8
	#define VERBOSE_CREATE_HIDE_OPENING_OK          16
	#define VERBOSE_CREATE_HIDE_PACKETS_RECOVERY    32
	#define VERBOSE_CREATE_HIDE_PACKETS_VERIF       64
	#define VERBOSE_CREATE_HIDE_PACKETS_DONE       128
	#define VERBOSE_CREATE_HIDE_PACKETS_WRITE      256
	#define VERBOSE_CREATE_RS_MATRIX_SILENT        512
	#define VERBOSE_CREATE_RS_MATRIX_DEBUG        1024
	#define VERBOSE_CREATE_RS_MATRIX_RESERVED     2048
	#define VERBOSE_CREATE_HIDE_ADJUST_NB_FILES   4096

	#define VERBOSE_VERIFY_LOAD_MAIN_PAR_SILENT      2
	#define VERBOSE_VERIFY_LOAD_MAIN_PAR_COMPACT     4

#endif // __COMMON_H__
