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
// vid_null.c -- null video driver to aid porting efforts

#include "quakedef.h"
#include "d_local.h"
#include "beebScreen/beebScreen.h"

extern viddef_t	vid;				// global video state

#define	BASEWIDTH	320
#define	BASEHEIGHT	200

byte	vid_buffer[BASEWIDTH*BASEHEIGHT];
short	zbuffer[BASEWIDTH*BASEHEIGHT];
byte	surfcache[256*1024];
static byte	backingbuf[48*24];

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];

void (*palExtractor)(int v,int *r,int *g,int *b) = beebScreen_extractBGR888;

int nulaPal[16];
int beebPal[256 + 16];
unsigned char palMap[256];
qboolean useNula = false;
qboolean useHdmi = false;
qboolean usePi = false;
qboolean isGreyscale = false;

#define GET_R(v) ((v) & 0x000f)
#define GET_G(v) (((v)>> 12) & 0x000f)
#define GET_B(v) (((v)>> 8) & 0x000f)

#include "beebScreen/bagiCode.c"

byte beebGamma[] = {4,7,9,11,13,15,17,19,21,22,24,26,27,29,30,32,33,35,36,38,39,40,42,
     43,45,46,47,48,50,51,52,54,55,56,57,59,60,61,62,63,65,66,67,68,69,
     70,72,73,74,75,76,77,78,79,80,82,83,84,85,86,87,88,89,90,91,92,93,
     94,95,96,97,98,100,101,102,103,104,105,106,107,108,109,110,111,112,
     113,114,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,
     129,130,131,132,133,133,134,135,136,137,138,139,140,141,142,143,144,
     144,145,146,147,148,149,150,151,152,153,153,154,155,156,157,158,159,
     160,160,161,162,163,164,165,166,166,167,168,169,170,171,172,172,173,
     174,175,176,177,178,178,179,180,181,182,183,183,184,185,186,187,188,
     188,189,190,191,192,193,193,194,195,196,197,197,198,199,200,201,201,
     202,203,204,205,206,206,207,208,209,210,210,211,212,213,213,214,215,
     216,217,217,218,219,220,221,221,222,223,224,224,225,226,227,228,228,
     229,230,231,231,232,233,234,235,235,236,237,238,238,239,240,241,241,
     242,243,244,244,245,246,247,247,248,249,250,251,251,252,253,254,254,
     255};

void extractBGR888x64(int v,int *r,int *g,int *b)
{
    *b = (v >> 4) & 0x0c;
    *g = (v >> 12) & 0x0c;
    *r = (v >> 20) & 0x0c;
}

void extractBGR888x512(int v,int *r,int *g,int *b)
{
    *b = (v >> 4) & 0x0e;
    *g = (v >> 12) & 0x0e;
    *r = (v >> 20) & 0x0e;
}

int greyPal[]= {0x0000,0x1111,0x2222,0x3333,0x4444,0x5555,0x6666,0x7777,0x8888,0x9999,0xaaaa,0xbbbb,0xcccc,0xdddd,0xeeee,0xffff};

//extern int viewheight;
// int lastPalIndices[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

qboolean setPal = false;

int rLookup[]= {0,0,0,0,0,8,8,8,8,8,8,16,16,16,16,16};

int partitionColor(int pal)
{
	int r = rLookup[pal &0x000f];
	int g = (pal>>13)&0x6;
	int b = (pal>>11)&0x1;

	return r+g+b;
}

int calculateBrightness()
{
	int counts[256];
	memset(counts,0,sizeof(counts));

	int total = 0;

	for(int i=0;i<BASEWIDTH * BASEHEIGHT; ++i)
	{
		counts[palMap[vid_buffer[i]]]++;
	}

	for(int i=0;i<256;++i)
	{
		if (counts[i])
		{
			int b = GET_R(beebPal[i]) * 3 + GET_G(beebPal[i]) * 5 + GET_B(beebPal[i]);
			total += b * counts[i];
		}
	}
	return total / (BASEWIDTH * BASEHEIGHT);
}

void	VID_SetPalette (unsigned char *palette)
{
	int tempPal[256];

	for(int i = 0 ;i < 256; ++i)
	{
		// if (useNula)
		// 	tempPal[i]=(palette[0]<<16)+(palette[1]<<8)+(palette[2]);
		// else
		tempPal[i]=(beebGamma[palette[0]]<<16)+(beebGamma[palette[1]]<<8)+(beebGamma[palette[2]]);
		palette+=3;
	}
	beebScreen_SetNulaPal(tempPal, beebPal, 256, palExtractor);
	setPal = true;
	if (useNula)
	{
		beebScreen_CreatePalMap(beebPal, 256, palMap);

		if (isGreyscale)
		{
			beebScreen_CreateRemapColours(beebPal, greyPal, 16, 256);
		}
	}
	else
	{
		beebScreen_SendPal(beebPal, 256);
	}
}

void	VID_ShiftPalette (unsigned char *palette)
{
	VID_SetPalette(palette);
}

qboolean bsInit = false;

void flipCallback()
{
	beebScreen_VSync();
	beebScreen_SendPal(nulaPal,16);
}

void	VID_Init (unsigned char *palette)
{
	vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
	vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = vid_buffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;
	
	d_pzbuffer = zbuffer;
	D_InitCaches (surfcache, sizeof(surfcache));

	int flags = BS_INIT_DOUBLE_BUFFER | BS_INIT_MOUSE;

	if (COM_CheckParm("-nula"))
	{
		useNula = true;
	}

	if (COM_CheckParm("-pi"))
	{
		usePi = true;
	}

	if (COM_CheckParm("-hdmi"))
	{
		useHdmi = true;
		useNula = true;
	}

	if (COM_CheckParm("-grey"))
	{
		isGreyscale = true;
		useNula = true;
	}

	if (COM_CheckParm("-st") || COM_CheckParm("-9bitcolor"))
	{
		palExtractor = extractBGR888x512;
		useNula = true;
		isGreyscale = false;
	}
	else if (COM_CheckParm("-ega") || COM_CheckParm("-6bitcolor"))
	{
		palExtractor = extractBGR888x64;
		useNula = true;
		isGreyscale = false;
	}

	if (COM_CheckParm("-nomouse"))
	{
		flags &= ~BS_INIT_MOUSE;
	}

	if (useHdmi)
	{
		flags |= BS_INIT_RGB2HDMI;
	}
	else if (useNula)
	{
		flags |= BS_INIT_NULA;
	}
	else if (usePi)
	{
		flags |= BS_INIT_PIVDU;
		flags &= ~BS_INIT_DOUBLE_BUFFER;
	}

	qboolean initVid = true;

	if (COM_CheckParm("-novid"))
	{
		initVid = false;
	}

	if (initVid)
	{
		beebScreen_Init(usePi ? 13 : 2, flags);

		bsInit = true;

		if (usePi)
		{
			beebScreen_SetGeometry(320,200,TRUE);
		}
		else
		{
			beebScreen_SetGeometry(128,192,TRUE);
		}

		beebScreen_SetBuffer(vid_buffer,BS_BUFFER_FORMAT_8BPP, BASEWIDTH, BASEHEIGHT);

		if (!usePi)
		{
			beebScreen_UseDefaultScreenBases();

			// Clear any unused screen memory (this is slow :'( )
			beebScreen_ClearScreens(true);
		}

		if (flags & BS_INIT_MOUSE && !usePi)
		{
			beebScreen_InjectCode(bagiCode_bin,bagiCode_bin_len,0x900);
			beebScreen_SetUserVector(BS_VECTOR_USER1,0x900);
		}

		if (useNula)
		{
			beebScreen_SetDefaultNulaRemapColours();

			if (isGreyscale)
			{
				beebScreen_SendPal(greyPal,16);
			}
			else
			{
				// Flip callback to send the palette as soon as possible after vsync
				beebScreen_FlipCallback(flipCallback);
			}
		}
	}
}

void	VID_Shutdown (void)
{
	if (bsInit)
	{
		beebScreen_Quit();
		_VDU(22);
		_VDU(7);
	}
}

void rescalePal(int* inPal,int *outPal,int gamma)
{
	for(int i =0;i<256;++i)
	{
		int r=GET_R(inPal[i]) * gamma;
		int g=GET_G(inPal[i]) * gamma;
		int b=GET_B(inPal[i]) * gamma;
		if (r > 127)
		{
			r=127;
		}
		if (g > 127)
		{
			g=127;
		}
		if (b > 127)
		{
			b= 127;
		}

		outPal[i]=MAPRGB(r>>3,g>>3,b>>3);
	}
}

int gammaTab[]={20,18,16,15,14,13,13,12,11,11,10,9,8,8,8};

void	VID_Update (vrect_t *rects)
{
	if (useNula && !isGreyscale)
	{
		beebScreen_CreateDynamicPalette(beebPal,palMap,256,nulaPal,16);
	}
	if (!useNula && !usePi)
	{
		// TODO - Dynamic brightness
		int averageBright = calculateBrightness();
		int newPal[256];
		rescalePal(beebPal, newPal, gammaTab[averageBright>>4]);
		beebScreen_SendPal(newPal,256);
	}

	beebScreen_Flip();

	if (useNula && !isGreyscale)
	{
		beebScreen_SendPal(nulaPal,16);
	}
}

/*
================
D_BeginDirectRect
================
*/
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
	int		i;

	for (i=0 ; i< height ; i ++)
	{
		memcpy (&backingbuf[(i) * 24],
				vid.buffer + x + (y + i) * vid.rowbytes,
				width);
		memcpy (vid.buffer + x + (y + i) * vid.rowbytes,
				&pbitmap[i * width],
				width);
	}

	VID_Update(NULL);

}


/*
================
D_EndDirectRect
================
*/
void D_EndDirectRect (int x, int y, int width, int height)
{
	for (int i=0 ; i< height ; i ++)
	{
		memcpy (vid.buffer + x + (y + i) * vid.rowbytes,
				&backingbuf[(i) * 24],
				width);
	}

	VID_Update(NULL);
}


