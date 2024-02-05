/**********************************************************************************************
	* Author:	Oso, Oluwafemi Ebenezer
	* Date:		13th of September, 2014
	* Filename:	oggHelper.h

	A wrapper for the OGG Vorbis library
***********************************************************************************************/
#include <stdio.h>
#include <time.h> //Needed just for the time() function
#include <vorbis/vorbisenc.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <math.h> //Needed for the floor function in decode

#include "bool.h"
#include "vorbisSettings.h"
#include "wavHeader.h"


class oggHelper; //oggHelper prototype, needed because of struct StaticParam_t

//Use to hold parameters for the thread(async) function
// struct OGG_StaticParam_t
// {
// 	char* pcm;
// 	char* ogg;
// 	EncodeSetting settings;
// 	VorbisComment comments;
// 	/*WNDPROC callback_proc;*/
// 	oggHelper* oggObj;
// };

struct WAVHEADER {
  // RIFF Header
  char riff_header[4]; // Contains "RIFF"
  int wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
  char wave_header[4]; // Contains "WAVE"

  // Format Header
  char fmt_header[4]; // Contains "fmt " (includes trailing space)
  int fmt_chunk_size; // Should be 16 for PCM
  short audio_format; // Should be 1 for PCM. 3 for IEEE Float
  short num_channels;
  int sample_rate;
  int byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
  short sample_alignment; // num_channels * Bytes Per Sample
  short bit_depth; // Number of bits per sample

  // Data
  char data_header[4]; // Contains "data"
  int data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
  // uint8_t bytes[]; // Remainder of wave file is bytes
};


class oggHelper
{
private:
	char ErrMsg[1000];
	void ErrorReport(char* ErrMsg);

	#define READ 1024

	// #define OH_STARTED	WM_USER+1
	// #define OH_COMPUTED	WM_USER+2
	// #define OH_DONE		WM_USER+3
	// #define OH_ERROR	WM_USER+4

	//#define MAX_THREAD_COUNT 5
	//HANDLE hThread[MAX_THREAD_COUNT];
	//OGG_StaticParam_t* hSParam[MAX_THREAD_COUNT];

	//The static member from which we can start the thread
	//static int encode_s(void* param);
	//static int decode_s(void* param);

	//The actual encode function
	//BOOL encode_x(const char* file_in, const char* file_out, EncodeSetting es, VorbisComment ivc/*, WNDPROC callbackproc*/);
	//The actual decode function
	//BOOL decode_x(const char* file_in, const char* file_out/*, WNDPROC callbackproc*/);
public:
	oggHelper();
	~oggHelper();

    _BOOL encode(const char* file_in, const char* file_out, EncodeSetting es, VorbisComment ivc/*, WNDPROC callbackproc*/);
	//The actual decode function
    _BOOL decode(const char* file_in, const char* file_out/*, WNDPROC callbackproc*/);

	//Encode PCM to OGG
	/*BOOL Encode(char* file_in, char* file_out);
	BOOL Encode(char* file_in, char* file_out, EncodeSetting es);
	BOOL Encode(char* file_in, char* file_out, EncodeSetting es, VorbisComment ivc);
	BOOL Encode(char* file_in, char* file_out, EncodeSetting es, VorbisComment ivc, WNDPROC callbackproc);
	void* Encode(char* file_in, char* file_out, EncodeSetting es, VorbisComment ivc, WNDPROC callbackproc, BOOL async);

	//Decode OGG to PCM (with a WAVE header)
	BOOL Decode(char* file_in, char* file_out);
	BOOL Decode(char* file_in, char* file_out, WNDPROC callbackproc);
	void* Decode(char* file_in, char* file_out, WNDPROC callbackproc, BOOL async);*/

};
