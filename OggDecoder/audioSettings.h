/*********************************************************************************************************
	* Author:	Oso, Oluwafemi Ebenezer
	* Date:		5th of June, 2014
	* Filename:	AudioSettings.h

	A generic audio settings common to all audio file format (samplerate, bitrate, number of channel, etc)
***********************************************************************************************************/
//Frequency
enum SampleRate
{
	OH_SR_8khz = 8000,
	OH_SR_11khz = 11025,
	OH_SR_12khz = 12000,
	OH_SR_16khz = 16000,
	OH_SR_22khz = 22050,
	OH_SR_24khz = 24000,
	OH_SR_32khz = 32000,
	OH_SR_44khz = 44100,
	OH_SR_48khz = 48000
};

enum Channel
{
	OH_Mono = 1,
	OH_Stereo = 2
};

enum Bitrate
{
	OH_BR_UNSET = -1,
	OH_BR_8kbps = 8000,
	OH_BR_16kbps = 16000,
	OH_BR_24kbps = 24000,
	OH_BR_32kbps = 32000,
	OH_BR_40kbps = 40000,
	OH_BR_48kbps = 48000,
	OH_BR_56kbps = 56000,
	OH_BR_64kbps = 64000,
	OH_BR_80kbps = 80000,
	OH_BR_96kbps = 96000,
	OH_BR_112kbps = 112000,
	OH_BR_128kbps = 128000,
	OH_BR_144kbps = 144000,
	OH_BR_160kbps = 160000,
	OH_BR_192kbps = 192000,
	OH_BR_224kbps = 224000,
	OH_BR_256kbps = 256000,
	OH_BR_320kbps = 320000
};

enum Encode_Mode
{
	VBR,
	ABR,
	CBR
};
