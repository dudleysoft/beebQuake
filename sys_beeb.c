/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// sys_null.h -- null system driver to aid porting efforts

#include "quakedef.h"
#include "errno.h"

#include "beebScreen/beebScreen.h"
#include "bbckeycodes.h"

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES             10
FILE    *sys_handles[MAX_HANDLES];

int             findhandle (void)
{
	int             i;
	
	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength (FILE *f)
{
	int             pos;
	int             end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

char pathDest[256];

char *remapPath(char *src)
{
	char *dest = pathDest;
	int sinceDir=0;
	while(*src)
	{
		switch(*src)
		{
		case '\\':
		case '/':
			*dest++='.';
			sinceDir = 0;
			break;
		case '.':
			if (sinceDir<10)
			{
				*dest++='/';
				sinceDir++;
			}
			break;
		default:
			if (sinceDir<10)
			{
				*dest++=*src;
				sinceDir++;
			}
			break;
		}
		src++;
	}
	*dest=0;
	return pathDest;
}

int Sys_FileOpenRead (char *path, int *hndl)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(remapPath(path), "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;
	
	return filelength(f);
}

int Sys_FileOpenWrite (char *path)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(remapPath(path), "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));
	sys_handles[i] = f;
	
	return i;
}

void Sys_FileClose (int handle)
{
	fclose (sys_handles[handle]);
	sys_handles[handle] = NULL;
}

void Sys_FileSeek (int handle, int position)
{
	fseek (sys_handles[handle], position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
	return fread (dest, 1, count, sys_handles[handle]);
}

int Sys_FileWrite (int handle, void *data, int count)
{
	return fwrite (data, 1, count, sys_handles[handle]);
}

int     Sys_FileTime (char *path)
{
	FILE    *f;
	
	f = fopen(remapPath(path), "rb");
	if (f)
	{
		fclose(f);
		return 1;
	}
	
	return -1;
}

void Sys_mkdir (char *path)
{
}


/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{
}


void Sys_Error (char *error, ...)
{
	va_list         argptr;
	beebScreen_Quit();

	// _VDU(22);
	// _VDU(7);

	printf ("Sys_Error: ");   
	va_start (argptr,error);
	vprintf (error,argptr);
	va_end (argptr);
	printf ("\n");

	exit (1);
}

extern qboolean bsInit;

void Sys_Printf (char *fmt, ...)
{
	va_list         argptr;

	if (!bsInit)
	{
		va_start (argptr,fmt);
		vprintf (fmt,argptr);
		va_end (argptr);
	}
}

void Sys_Quit (void)
{
	if (bsInit)
	{
		beebScreen_Quit();
		_VDU(22);
		_VDU(7);
	}
	exit (0);
}

double Sys_FloatTime (void)
{
	static double t = 0;
	static int prevFrame = 0;

	int next = beebScreen_GetFrameCounter();
	int delta = next-prevFrame;
	if (next < prevFrame)
	{
		delta = (next+256)-prevFrame;
	}
	
	t += (float)delta/50.0;
	
	return t;
}

char *Sys_ConsoleInput (void)
{
	return NULL;
}

void Sys_Sleep (void)
{
}

#define KEY_COUNT 24

int keyMap[] = {
	BBC_ESC,	// Menu
	BBC_a,		// Strafe Left
	BBC_d,		// Strafe Right
	BBC_w,		// Forward
	BBC_s,		// Backwards
	BBC_SPACE,	// Jump
	BBC_SHIFT,	// Sprint
	BBC_RETURN,	// Fire
	BBC_TAB,	// Status
	BBC_1,		// First Weapon Select
	BBC_2,
	BBC_3,
	BBC_4,
	BBC_5,
	BBC_6,
	BBC_7,
	BBC_8,
	BBC_9,		// Last Weapon select
    BBC_HAT,
	BBC_CURL,
	BBC_CURR,
	BBC_CURU,
	BBC_CURD,
	BBC_COPY	// Pause
};

int keyEvent[] = 
{
	K_ESCAPE,
	'a',
	'd',
	'w',
	's',
	' ',
	K_SHIFT,
	K_CTRL,
	K_TAB,
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'`',
	K_LEFTARROW,
	K_RIGHTARROW,
	K_UPARROW,
	K_DOWNARROW,
	K_PAUSE
};

#define MENU_KEY_COUNT 48

int menuKeyMap[] = {
	BBC_ESC,
	BBC_CURL,
	BBC_CURR,
	BBC_CURU,
	BBC_CURD,
	BBC_RETURN,
	BBC_SPACE,
	BBC_SHIFT,
	BBC_CTRL,
	BBC_TAB,
	BBC_DELETE,
	BBC_SEMICOLON,
	BBC_1,
	BBC_2,
	BBC_3,
	BBC_4,
	BBC_5,
	BBC_6,
	BBC_7,
	BBC_8,
	BBC_9,
	BBC_0,
	BBC_a,
	BBC_b,
	BBC_c,
	BBC_d,
	BBC_e,
	BBC_f,
	BBC_g,
	BBC_h,
	BBC_i,
	BBC_j,
	BBC_k,
	BBC_l,
	BBC_m,
	BBC_n,
	BBC_o,
	BBC_p,
	BBC_q,
	BBC_r,
	BBC_s,
	BBC_t,
	BBC_u,
	BBC_v,
	BBC_w,
	BBC_x,
	BBC_y,
	BBC_z
};

int menuKeyEvent[] = {
	K_ESCAPE,
	K_LEFTARROW,
	K_RIGHTARROW,
	K_UPARROW,
	K_DOWNARROW,
	K_ENTER,
	' ',
	K_SHIFT,
	K_CTRL,
	K_TAB,
	K_BACKSPACE,
	';',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'a',
	'b',
	'c',
	'd',
	'e',
	'f',
	'g',
	'h',
	'i',
	'j',
	'k',
	'l',
	'm',
	'n',
	'o',
	'p',
	'q',
	'r',
	's',
	't',
	'u',
	'v',
	'w',
	'x',
	'y',
	'z'
};

qboolean keyDown[]={
    false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false
};

void ScanKeys(int *map, int *events,int count)
{
    for(int i = 0; i < count; ++i)
    {
        if (beebScreen_ScanKey(map[i]))
        {
            if (!keyDown[i])
            {
                Key_Event(events[i], true);
                keyDown[i] = true;
            }
        }
        else
        {
            if (keyDown[i])
            {
                Key_Event(events[i], false);
                keyDown[i] = false;
            }
        }
    }
}

extern keydest_t	key_dest;

void Sys_SendKeyEvents (void)
{
	if (key_dest == key_game)
	{
		ScanKeys(keyMap,keyEvent,KEY_COUNT);
	}
	else
	{
		ScanKeys(menuKeyMap,menuKeyEvent,MENU_KEY_COUNT);
	}
}

void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}

//=============================================================================

void main (int argc, char **argv)
{
	static quakeparms_t    parms;

	parms.memsize = 10*1024*1024;
	int j = COM_CheckParm("-mem");
	if (j)
	{
		float newSize=Q_atof(com_argv[j+1]);
		if (newSize >= 10.0)
		{
			parms.memsize = (int) (Q_atof(com_argv[j+1]) * 1024 * 1024);
		}
	}

	parms.membase = malloc (parms.memsize);
	parms.basedir = "@";

	COM_InitArgv (argc, argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	Sys_Printf ("Host_Init\n");
	Host_Init (&parms);
	float oldTime = Sys_FloatTime() - 0.1;
	while (1)
	{
		float newTime = Sys_FloatTime();
		float time = newTime-oldTime;
		oldTime = newTime;

		// Make sure long frame times don't cause the net icon to appear
		if (time> 0.25)
		{
			time = 0.25;
		}
		Host_Frame (time);
	}
}


