/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: tp_font.c 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================ 
DESCRIPTION
Sample program for demonstration of the D1MX GFX features
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2014
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing, not for mass production

DISCLAIMER 

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.
****************************************************************************
*/

/***********************************************************
  Title: Write Module 
  
  This module contains the write functions for bitmap fonts.
*/


/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"         /* Renesas basic types, e.g. uint32_t */
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "img_format.h"
#include "font.h"
#include "font_data.h"
//#include "main.h"

#include "tp_font.h"

/***********************************************************
  Variable: locCurrentFont

  Font to be used by text operations.
*/

static const Font_t *locCurrentFont;
static int32_t locFontHeight;

static r_drw2d_Rect_t rect;
static r_drw2d_Rect_t src_rect;
static r_drw2d_Texture_t tex;

/***********************************************************
  Function: locGetCharRemapping
*/

static int locGetCharRemapping(unsigned char Character)
{
    int c;
    
    /* Character remapping */
    if ((Character >= 32) && (Character < 128))
    {
        /* visible ASCII characters start with 32 (blank)
           not visible chars 0-31 and will be omitted */
        c = (Character - 32);
    }
    else
    {
        switch (Character)
        {
            case 0xB0:
            	  /* special remapping for non ASCII character 'degree' (° or 0xB0)
            	     non visible char DEL (127) will be used for 'degree' */
                c = (127 - 32);
                break;
            default:
                /* use blank as default */
                c = 0;
                break;
        }
    }
    return c;
}


/***********************************************************
  Function: locWriteChar

  Write a single character.

  Use the currently selected buffer, font and color to draw
  a character at the given position.

  Parameters:
  Dev        - R_DRW2D device structure
  PosX       - Display X position (not turned)
  PosY       - Display Y position (not turned)
  Character  - ASCII value of the character

  Returns:
  void
*/

static void locWriteChar(r_drw2d_Device_t Dev, int PosX, int PosY, unsigned char Character)
{
    int       xsrc;
    int       wsrc;
    char      c;

    c = locGetCharRemapping(Character);
    xsrc  = locCurrentFont->Pos[c].Offset;
    wsrc  = locCurrentFont->Pos[c].Width;

    rect.Pos.X = R_DRW2D_2X(PosX);
    rect.Pos.Y = R_DRW2D_2X(PosY);
    rect.Size.Height = R_DRW2D_2X(locFontHeight);
    rect.Size.Width = R_DRW2D_2X(wsrc);

    src_rect.Pos.X = R_DRW2D_2X(xsrc);
    src_rect.Pos.Y = R_DRW2D_2X(0);
    src_rect.Size.Height = R_DRW2D_2X(locCurrentFont->Height);
    src_rect.Size.Width = R_DRW2D_2X(wsrc);

    R_DRW2D_TextureBlit(Dev, &src_rect, &rect);
}

/***********************************************************
  Section: Global Functions

  Description see: <write.h>
*/

/***********************************************************
  Function: SelectFont
*/

void TP_FONT_SelectFont(const Font_t *Font)
{
    locCurrentFont = Font;
    locFontHeight = locCurrentFont->Height;
}


/***********************************************************
  Function: GetFontHeight
*/

int TP_FONT_GetFontHeight(void)
{
    return locCurrentFont->Height;
}


void TP_FONT_SetFontHeight(int32_t Height)
{
    locFontHeight = Height;
}


/***********************************************************
  Function: GetCharWidth
*/

int TP_FONT_GetCharWidth(char Character)
{
    int c;

    c = locGetCharRemapping(Character);
    if ((c < 0) || (c > 95))
    {
        return 0;
    }
    return locCurrentFont->Pos[c].Width;
}


/***********************************************************
  Function: GetTextWidth
*/

int TP_FONT_GetTextWidth(char *Text)
{
    int w;

    w = 0;
    while (*Text != 0)
    {
        w += TP_FONT_GetCharWidth(*Text);
        Text++;
    }
    return w;
}

/***********************************************************
  Function: GetTextHeight
*/

int TP_FONT_GetTextHeight(char *Text)
{
    int h;

    h = 1; /* To include the last line */
    while (*Text != 0)
    {
        if (*Text == '\n')
        {
            h += TP_FONT_GetFontHeight();
        }
        Text++;
    }
    return h;
}





/***********************************************************
  Function: Write

  The color of the rendered text is given by the the
  current foreground color.
*/

void TP_FONT_Print(r_drw2d_Device_t Dev, int PosX, int PosY, r_drw2d_Color_t  Color, char *Text)
{
    int                   startx;
    r_drw2d_EffectStage_t effect[2];

    tex.Buffer.Data = *locCurrentFont->Texture->Data;
    tex.Buffer.Pitch = locCurrentFont->Texture->Width;
    tex.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_AL8;
    tex.Buffer.Size.Height = locCurrentFont->Texture->Height;
    tex.Buffer.Size.Width = locCurrentFont->Texture->Width;
    tex.Flags = R_DRW2D_TEX_BILINEAR;
    tex.Handle = 0;

    R_DRW2D_CtxIdentity(Dev);
    R_DRW2D_CtxTextureIdentity(Dev);

    R_DRW2D_CtxBlendMode(Dev, R_DRW2D_BLENDMODE_SRC_OVER);
    R_DRW2D_CtxFillMode(Dev, R_DRW2D_FILLMODE_TEXTURE);

    effect[0].Name = R_DRW2D_EFFECT_MODULATE;
    effect[0].Args[0].Source = R_DRW2D_EFFECT_SOURCE_TEXTURE_UNIT;
    effect[0].Args[0].Param.Color.Source.TextureUnit = 0;
    effect[0].Args[0].Param.Color.Operand = R_DRW2D_EFFECT_COLOR_OPERAND_RGBA;
    effect[0].Args[1].Source = R_DRW2D_EFFECT_SOURCE_CONSTANT_COLOR;
    effect[0].Args[1].Param.Color.Source.ConstantColor = Color;
    effect[0].Args[1].Param.Color.Operand = R_DRW2D_EFFECT_COLOR_OPERAND_RGBA;

    R_DRW2D_CtxEffectsSet(Dev, effect, 1);
    R_DRW2D_CtxTextureSet(Dev, 0, &tex);

    startx = PosX;
    while (*Text != 0)
    {
        if (*Text == '\n')
        {
            PosX  = startx;
            PosY += locCurrentFont->Height;
        }
        else
        {
            locWriteChar(Dev, PosX, PosY, *Text);
            PosX += TP_FONT_GetCharWidth(*Text);
        }
        Text++;
    } 

    R_DRW2D_CtxEffectsDelete(Dev);
}

