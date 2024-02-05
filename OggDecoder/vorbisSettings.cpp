/**********************************************************************************************
	* Author:	Oso, Oluwafemi Ebenezer
	* Date:		5th of June, 2014
	* Filename:	OggHelper_VorbisSettings.cpp

	Vorbis settings (comments, encode mode, etc) used for OggHelper (Vorbis Wrapper by Osofem)
***********************************************************************************************/
#include "vorbisSettings.h"

VorbisComment::VorbisComment()
{
	TITLE = "";
	VERSION = "";
	ALBUM = "";
	TRACKNUMBER = "";
	ARTIST = "";
	PERFORMER = "";
	COPYRIGHT = "";
	LICENSE = "";
	ORGANISATION = "";
	DESCRIPTION = "";
	GENRE = "";
	DATE = "";
	LOCATION = "";
	CONTACT = "";
	ISRC = "";
}

EncodeSetting::EncodeSetting()
{
	channel = OH_Stereo;
	encode_mode = VBR;
	min_abr_br = OH_BR_UNSET;
	max_abr_br = OH_BR_UNSET;
	abr_br = OH_BR_128kbps;
	vbr_quality = 0.5;
	cbr_br = OH_BR_128kbps;
}
