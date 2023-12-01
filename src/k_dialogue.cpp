// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) by Sonic Team Junior
// Copyright (C) by Kart Krew
// Copyright (C) by Sally "TehRealSalt" Cochenour
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_dialogue.cpp
/// \brief Basic text prompts

#include "k_dialogue.hpp"
#include "k_dialogue.h"

#include <string>
#include <algorithm>

#include "info.h"
#include "sounds.h"
#include "g_game.h"
#include "v_video.h"
#include "r_draw.h"
#include "m_easing.h"
#include "r_skins.h"
#include "s_sound.h"
#include "z_zone.h"
#include "k_hud.h"
#include "p_tick.h" // P_LevelIsFrozen

#include "v_draw.hpp"

#include "acs/interface.h"

using srb2::Dialogue;

void Dialogue::Init(void)
{
	active = true;
	syllable = true;
}

void Dialogue::SetSpeaker(void)
{
	// Unset speaker
	speaker.clear();

	portrait = nullptr;
	portraitColormap = nullptr;

	voiceSfx = sfx_ktalk;
}

void Dialogue::SetSpeaker(std::string skinName, int portraitID)
{
	Init();

	// Set speaker based on a skin
	int skinID = -1;
	if (!skinName.empty())
	{
		skinID = R_SkinAvailable(skinName.c_str());
	}

	if (skinID >= 0 && skinID < numskins)
	{
		const skin_t *skin = &skins[skinID];
		const spritedef_t *sprdef = &skin->sprites[SPR2_TALK];

		if (sprdef->numframes > 0)
		{
			portraitID %= sprdef->numframes;

			const spriteframe_t *sprframe = &sprdef->spriteframes[portraitID];

			portrait = static_cast<patch_t *>( W_CachePatchNum(sprframe->lumppat[0], PU_CACHE) );
			portraitColormap = R_GetTranslationColormap(skinID, static_cast<skincolornum_t>(skin->prefcolor), GTC_CACHE);
		}
		else
		{
			portrait = nullptr;
			portraitColormap = nullptr;
		}

		speaker = skin->realname;

		voiceSfx = skin->soundsid[ S_sfx[sfx_ktalk].skinsound ];
	}
	else
	{
		SetSpeaker();
	}
}

void Dialogue::SetSpeaker(std::string name, patch_t *patch, UINT8 *colormap, sfxenum_t voice)
{
	Init();

	// Set custom speaker
	speaker = name;

	if (speaker.empty())
	{
		portrait = nullptr;
		portraitColormap = nullptr;
		voiceSfx = sfx_ktalk;
		return;
	}

	portrait = patch;
	portraitColormap = colormap;

	voiceSfx = voice;
}

void Dialogue::NewText(std::string newText)
{
	Init();

	newText = V_ScaledWordWrap(
		290 << FRACBITS,
		FRACUNIT, FRACUNIT, FRACUNIT,
		0, HU_FONT,
		newText.c_str()
	);

	text.clear();

	textDest = newText;
	std::reverse(textDest.begin(), textDest.end());

	textTimer = kTextPunctPause;
	textSpeed = kTextSpeedDefault;
	textDone = false;
}

bool Dialogue::Active(void)
{
	return active;
}

bool Dialogue::TextDone(void)
{
	return textDone;
}

bool Dialogue::Dismissable(void)
{
	return dismissable;
}

void Dialogue::SetDismissable(bool value)
{
	dismissable = value;
}

void Dialogue::WriteText(void)
{
	bool voicePlayed = false;

	textTimer -= textSpeed;

	while (textTimer <= 0 && !textDest.empty())
	{
		char c = textDest.back(), nextc = '\n';
		text.push_back(c);

		textDest.pop_back();

		if (c & 0x80)
		{
			// Color code support
			continue;
		}

		if (!textDest.empty())
			nextc = textDest.back();

		if (voicePlayed == false
			&& std::isprint(c)
			&& c != ' ')
		{
			if (syllable)
			{
				S_StopSoundByNum(voiceSfx);
				S_StartSound(nullptr, voiceSfx);
			}

			syllable = !syllable;
			voicePlayed = true;
		}

		if (std::ispunct(c)
			&& std::isspace(nextc))
		{
			// slow down for punctuation
			textTimer += kTextPunctPause;
		}
		else
		{
			textTimer += FRACUNIT;
		}
	}

	textDone = (textTimer <= 0 && textDest.empty());
}

bool Dialogue::Held(void)
{
	return ((players[serverplayer].cmd.buttons & BT_VOTE) == BT_VOTE);
}

bool Dialogue::Pressed(void)
{
	return (
		((players[serverplayer].cmd.buttons & BT_VOTE) == BT_VOTE) &&
		((players[serverplayer].oldcmd.buttons & BT_VOTE) == 0)
	);
}

void Dialogue::CompleteText(void)
{
	while (!textDest.empty())
	{
		text.push_back( textDest.back() );
		textDest.pop_back();
	}

	textTimer = 0;
	textDone = true;
}

void Dialogue::Tick(void)
{
	if (Active())
	{
		if (slide < FRACUNIT)
		{
			slide += kSlideSpeed;
		}
	}
	else
	{
		if (slide > 0)
		{
			slide -= kSlideSpeed;

			if (slide <= 0)
			{
				Unset();
			}
		}
	}

	slide = std::clamp(slide, 0, FRACUNIT);

	if (slide != FRACUNIT)
	{
		return;
	}

	WriteText();

	if (Dismissable() == true)
	{
		if (Pressed() == true)
		{
			if (TextDone())
			{
				Dismiss();
			}
			else
			{
				CompleteText();
			}
		}
	}
}

INT32 Dialogue::SlideAmount(fixed_t multiplier)
{
	if (slide == 0)
		return 0;
	if (slide == FRACUNIT)
		return multiplier;
	return Easing_OutCubic(slide, 0, multiplier);
}

void Dialogue::Draw(void)
{
	if (slide == 0)
	{
		return;
	}

	const UINT8 bgcol = 1, darkcol = 235;

	const fixed_t height = 78 * FRACUNIT;

	INT32 speakernameedge = -6;

	srb2::Draw drawer = 
		srb2::Draw(
			BASEVIDWIDTH, BASEVIDHEIGHT - FixedToFloat(SlideAmount(height) - height)
		).flags(V_SNAPTOBOTTOM);

	// TODO -- hack, change when dialogue is made per-player/netsynced
	UINT32 speakerbgflags = (players[consoleplayer].nocontrol == 0 && P_LevelIsFrozen() == false)
		? (V_ADD|V_30TRANS)
		: 0;

	drawer
		.flags(speakerbgflags|V_VFLIP|V_FLIP)
		.patch("TUTDIAGA");

	drawer
		.flags(V_VFLIP|V_FLIP)
		.patch("TUTDIAGB");

	if (portrait != nullptr)
	{
		drawer
			.flags(V_VFLIP|V_FLIP)
			.patch("TUTDIAGC");

		drawer
			.xy(-10-32, -41-32)
			.colormap(portraitColormap)
			.patch(portrait);

		speakernameedge -= 39; // -45
	}

	const char *speakername = speaker.c_str();

	const INT32 arrowstep = 8; // width of TUTDIAGD

	if (speakername && speaker[0])
	{
		INT32 speakernamewidth = V_StringWidth(speakername, 0);
		INT32 existingborder = (portrait == nullptr ? -4 : 3);

		INT32 speakernamewidthoffset = (speakernamewidth + (arrowstep - existingborder) - 1) % arrowstep;
		if (speakernamewidthoffset)
		{
			speakernamewidthoffset = (arrowstep - speakernamewidthoffset);
			speakernamewidth += speakernamewidthoffset;
		}

		if (portrait == nullptr)
		{
			speakernameedge -= 3;
			speakernamewidth += 3;
			existingborder += 2;
			drawer
				.xy(speakernameedge, -36)
				.width(2)
				.height(3+11)
				.fill(bgcol);
		}

		if (speakernamewidth > existingborder)
		{
			drawer
				.x(speakernameedge - speakernamewidth)
				.width(speakernamewidth - existingborder)
				.y(-36-3)
				.height(3)
				.fill(bgcol);

			drawer
				.x(speakernameedge - speakernamewidth)
				.width(speakernamewidth - existingborder)
				.y(-38-11)
				.height(11)
				.fill(darkcol);
		}

		speakernameedge -= speakernamewidth;

		drawer
			.xy(speakernamewidthoffset + speakernameedge, -39-9)
			.font(srb2::Draw::Font::kConsole)
			.text(speakername);

		speakernameedge -= 5;

		drawer
			.xy(speakernameedge, -36)
			.flags(V_VFLIP|V_FLIP)
			.patch("TUTDIAGD");

		drawer
			.xy(speakernameedge, -36-3-11)
			.width(5)
			.height(3+11)
			.fill(bgcol);

		drawer
			.xy(speakernameedge + 5, -36)
			.flags(V_VFLIP|V_FLIP)
			.patch("TUTDIAGF");
	}

	while (speakernameedge > -142) // the left-most edge
	{
		speakernameedge -= arrowstep;

		drawer
			.xy(speakernameedge, -36)
			.flags(V_VFLIP|V_FLIP)
			.patch("TUTDIAGD");
	}

	drawer
		.xy(speakernameedge - arrowstep, -36)
		.flags(V_VFLIP|V_FLIP)
		.patch("TUTDIAGE");

	drawer
		.xy(10 - BASEVIDWIDTH, -3-32)
		.font(srb2::Draw::Font::kConsole)
		.text( text.c_str() );

	if (Dismissable())
	{
		if (TextDone())
		{
			drawer
				.xy(-14, -7-5)
				.patch("TUTDIAG2");
		}

		drawer
			.xy(17-14 - BASEVIDWIDTH, -39-16)
			.button(srb2::Draw::Button::z, Held());
	}
}

void Dialogue::Dismiss(void)
{
	active = false;
	text.clear();
	textDest.clear();
}

void Dialogue::Unset(void)
{
	Dismiss();
	SetSpeaker();
	slide = 0;
}

/*
	Ideally, the Dialogue class would be on player_t instead of in global space
	for full multiplayer compatibility, but right now it's only being used for
	the tutorial, and I don't feel like writing network code. If you feel like
	doing that, then you can remove g_dialogue entirely.
*/

Dialogue g_dialogue;

void K_UnsetDialogue(void)
{
	g_dialogue.Unset();
}

void K_DrawDialogue(void)
{
	g_dialogue.Draw();
}

void K_TickDialogue(void)
{
	g_dialogue.Tick();
}

INT32 K_GetDialogueSlide(fixed_t multiplier)
{
	return g_dialogue.SlideAmount(multiplier);
}
