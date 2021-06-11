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
// in_null.c -- for systems without a mouse

#include "quakedef.h"

#include "beebScreen/beebScreen.h"

int mouse_x;
int mouse_y;
int mouse_b;
int old_mouse_b = 0;

extern void IN_MLookDown();

void CenterMouse()
{
    // Write to the host's memory at the location used to store the mouse coordinate
    _VDU(BS_CMD_SEND_USER1);
    // WriteByteToIo((void*)0xda6,0x80);
    // WriteByteToIo((void*)0xda7,0x02);
    // WriteByteToIo((void*)0xda8,0x00);
    // WriteByteToIo((void*)0xda9,0x02);
}

void IN_Init (void)
{
    // Make sure the mouse is centered
    CenterMouse();

    // Turn mouse look on permantently
    IN_MLookDown();
}

void IN_Shutdown (void)
{
}

void IN_Commands (void)
{
    int x,y;
    beebScreen_GetMouse(&x, &y, &mouse_b);
    Con_DPrintf("Mouse: %d,%d,%d\n",x, y, mouse_b);
    
    int mouse_b_delta = mouse_b ^ old_mouse_b;
    if (mouse_b_delta & 1)
    {
        Key_Event(K_MOUSE1, (mouse_b & 1) ? true: false);
    }
    if (mouse_b_delta & 2)
    {
        Key_Event(K_MOUSE2, (mouse_b & 2) ? true: false);
    }
    if (mouse_b_delta & 4)
    {
        Key_Event(K_MOUSE3, (mouse_b & 4) ? true: false);
    }
    old_mouse_b = mouse_b;
    mouse_x = (x - 640.0) * cl_yawspeed.value;
    mouse_y = (512.0 - y) * cl_pitchspeed.value;

    // Reset the mouse centre position
    CenterMouse();
}

void IN_MouseMove(usercmd_t *cmd)
{
    mouse_x *= sensitivity.value;
	mouse_y *= sensitivity.value;

// add mouse X/Y movement to cmd
	if ( (in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1) ))
		cmd->sidemove += m_side.value * mouse_x * host_frametime;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x * host_frametime;
	
	if (in_mlook.state & 1)
		V_StopPitchDrift ();
		
	if ( (in_mlook.state & 1) && !(in_strafe.state & 1))
	{
		cl.viewangles[PITCH] += m_pitch.value * mouse_y * host_frametime;
		if (cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if (cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	}
	else
	{
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * mouse_y * host_frametime;
		else
			cmd->forwardmove -= m_forward.value * mouse_y * host_frametime;
	}
}

void IN_Move (usercmd_t *cmd)
{
    IN_MouseMove(cmd);
}

