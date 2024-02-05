/**********************************************************************************************
	* Author:	Oso, Oluwafemi Ebenezer
	* Date:		5th of June, 2014
	* Filename:	OggHelper_VorbisSettings.h

	Vorbis settings (comments, encode mode, etc) used for OggHelper (Vorbis Wrapper by Osofem)
***********************************************************************************************/
#include "audioSettings.h"

#define VBR_Quality float // Range between -0.1 to 1

//Encoding setting
struct EncodeSetting
{
	Channel channel;
	Encode_Mode encode_mode;
	Bitrate min_abr_br;
	Bitrate max_abr_br;
	Bitrate abr_br;
	Bitrate cbr_br;
	VBR_Quality vbr_quality;

	//The constructor: used to set default values
	EncodeSetting();
};

//Standard comment supported for vorbis file
struct VorbisComment
{
	char* TITLE;
	char* VERSION;
	char* ALBUM;
	char* TRACKNUMBER;
	char* ARTIST;
	char* PERFORMER;
	char* COPYRIGHT;
	char* LICENSE;
	char* ORGANISATION;
	char* DESCRIPTION;
	char* GENRE;
	char* DATE;
	char* LOCATION;
	char* CONTACT;
	char* ISRC;

	//The constructor: used to set default values
	VorbisComment();
};
