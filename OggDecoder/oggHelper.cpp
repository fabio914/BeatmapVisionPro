/**********************************************************************************************
	* Author:	Oso, Oluwafemi Ebenezer
	* Date:		13th of September, 2014
	* Filename:	oggHelper.cpp
	* Edited:	22nd of October, 2014

	A wrapper for the OGG Vorbis library
***********************************************************************************************/
#include "oggHelper.h"

//Constructor
oggHelper::oggHelper() {}

//Destructor
oggHelper::~oggHelper() {}


/*The actual Encode function*/
_BOOL oggHelper::encode(const char* file_in, const char* file_out, EncodeSetting es, VorbisComment ivc/*, WNDPROC callbackproc*/)
{
	/*if(callbackproc != NULL)
	{
		callbackproc((HWND)GetModuleHandle(NULL), OH_STARTED, NULL, NULL);
	}*/


    _BOOL hresult = TRUE;
	//WaveFileHeader* wfh = new WaveFileHeader();
	WAVHEADER wfh;
	FILE* f_in = fopen(file_in, "rb");
	FILE* f_out = fopen(file_out, "wb");

	long cumulative_read = 0;
	long PCM_total_size = 0;

	if(!f_in)
	{
		sprintf(ErrMsg, "Couldn't open %s for read! Aborting!", file_in);
		ErrorReport(ErrMsg);
		hresult = FALSE;
		/*if(callbackproc != NULL)
		{
			callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -1, NULL);
		}*/
		goto done;
	}

	if(!f_out)
	{
		sprintf(ErrMsg, "Couldn't open %s for write! Aborting!", file_out);
		ErrorReport(ErrMsg);
		hresult = FALSE;
		/*if(callbackproc != NULL)
		{
			callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -3, NULL);
		}*/
		goto done;
	}

	//Set wav header
	fread(&wfh, sizeof(WAVHEADER), 1, f_in);


	fseek(f_in, 0, SEEK_END);
	PCM_total_size = ftell(f_in);
	fseek(f_in, 0, SEEK_SET);

	/*if(!wfh->IsWAVE(f_in, wfh))
	{
		sprintf(ErrMsg, "The input file %s is not a valid wave(PCM) file or is not a currently supported format! Aborting!", file_in);
		ErrorReport(ErrMsg);
		hresult = FALSE;

		goto done;
	}*/

	//The wave header already read in IsWave()
	//cumulative_read += 44;
	cumulative_read += sizeof(WAVHEADER);



	ogg_stream_state os;
	ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */

	vorbis_info      vi; /* struct that stores all the static vorbis bitstream settings */
	vorbis_comment   vc; /* struct that stores all the user comments */

	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */
	int result;

	//Encode setup
	vorbis_info_init(&vi);
	switch(es.encode_mode)
	{
	case VBR:
		{
			if(es.vbr_quality > 1)
				es.vbr_quality = 1;

			result = vorbis_encode_init_vbr(&vi, es.channel, wfh.sample_rate, es.vbr_quality);
		}
		break;
	case ABR:
		{
			if(es.max_abr_br != -1)
			{
				result = (vorbis_encode_setup_managed(&vi , es.channel, wfh.sample_rate, es.min_abr_br, es.abr_br, es.max_abr_br) ||
					vorbis_encode_ctl(&vi, OV_ECTL_RATEMANAGE2_SET, NULL) || vorbis_encode_setup_init(&vi));
			}
			else
			{
				result = vorbis_encode_init(&vi, es.channel, wfh.sample_rate, -1, es.abr_br, -1);
			}
		}
		break;
	case CBR:
		result = vorbis_encode_init(&vi, es.channel, wfh.sample_rate, es.cbr_br, es.cbr_br, es.cbr_br);
		break;
	default:
		//Error
		{
			sprintf(ErrMsg, "Error in selecting the proper encoding mode! Aborting!");
			ErrorReport(ErrMsg);
			hresult = FALSE;
			/*if(callbackproc != NULL)
			{
				callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -4, NULL);
			}*/
			goto done;
		}
		break;
	}

	//Do not continue if setup failed; this can happen if we ask for a mode that libVorbis does not support
	//(eg, too low a bitrate, etc, will return 'OV_EIMPL')
	if(result < 0)
	{
		sprintf(ErrMsg, "An error occurred while setting up libVorbis encode environment! Aborting!");
		ErrorReport(ErrMsg);
		hresult = FALSE;
		/*if(callbackproc != NULL)
		{
			callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -5, NULL);
		}*/
		goto done;
	}

	//Add a comment
	vorbis_comment_init(&vc);
	vorbis_comment_add_tag(&vc, "ALBUM", ivc.ALBUM);
	vorbis_comment_add_tag(&vc, "ARTIST", ivc.ARTIST);
	vorbis_comment_add_tag(&vc, "CONTACT", ivc.CONTACT);
	vorbis_comment_add_tag(&vc, "COPYRIGHT", ivc.COPYRIGHT);
	vorbis_comment_add_tag(&vc, "DATE", ivc.DATE);
	vorbis_comment_add_tag(&vc, "DESCRIPTION", ivc.DESCRIPTION);
	vorbis_comment_add_tag(&vc, "GENRE", ivc.GENRE);
	vorbis_comment_add_tag(&vc, "ISRC", ivc.ISRC);
	vorbis_comment_add_tag(&vc, "LICENSE", ivc.LICENSE);
	vorbis_comment_add_tag(&vc, "LOCATION", ivc.LOCATION);
	vorbis_comment_add_tag(&vc, "ORGANISATION", ivc.ORGANISATION);
	vorbis_comment_add_tag(&vc, "PERFORMER", ivc.PERFORMER);
	vorbis_comment_add_tag(&vc, "TITLE", ivc.TITLE);
	vorbis_comment_add_tag(&vc, "TRACKNUMBER", ivc.TRACKNUMBER);
	vorbis_comment_add_tag(&vc, "VERSION", ivc.VERSION);


	//Set up the analysis state and auxiliary encoding storage
	vorbis_analysis_init(&vd, &vi);
	vorbis_block_init(&vd, &vb);

	/*Set up our packet->stream encoder
	pick a random serial number; that way we can more likely build chained streams just by concatenation */
	srand(time(NULL));
	ogg_stream_init(&os, rand());

	 {
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;

		vorbis_analysis_headerout(&vd,&vc,&header,&header_comm,&header_code);
		ogg_stream_packetin(&os,&header); /* automatically placed in its own page */
		ogg_stream_packetin(&os,&header_comm);
		ogg_stream_packetin(&os,&header_code);

		//This ensures the actual audio data will start on a new page, as per spec
		while(true)
		{
			int result = ogg_stream_flush(&os, &og);
			if(result==0) break;
			fwrite(og.header,1,og.header_len, f_out);
			fwrite(og.body,1,og.body_len, f_out);
		}
	}

	 signed char readbuffer[(READ * 4)];
	 while(true)
	 {
		 long i;
		 long bytes = fread(readbuffer, 1, (READ*4), f_in);
		 cumulative_read += bytes;

		 //Percentage done
		 /*if(callbackproc != NULL)
		 {
			 int percentage = ((float)cumulative_read/PCM_total_size)*100;
			 callbackproc((HWND)GetModuleHandle(NULL), OH_COMPUTED, percentage, NULL);
		 }*/

		 if(bytes == 0)
		 {
			 vorbis_analysis_wrote(&vd, 0);
			 break;
		 }
		 else
		 {
			 //data to encode
			 //expose the buffer to submit data
			 float** buffer = vorbis_analysis_buffer(&vd, READ);
			 if(wfh.num_channels == 2 && es.channel == OH_Mono)
			 {
				 //Stereo input, mono output
				 for(i = 0; i < bytes/4; i++)
				 {
					 //Get left and right channels and half them
					 buffer[0][i] = ((((readbuffer[i*4+1]<<8) | (0x00ff & (int)readbuffer[i*4])) / 32768.f) + (((readbuffer[i*4+3]<<8) | (0x00ff & (int)readbuffer[i*4+2])) / 32768.f)) * 0.5f;
				 }
			 }
			 else if(wfh.num_channels == 2 && es.channel == OH_Stereo)
			 {
				 //deinterleave samples
				 for(i = 0; i < bytes/4; i++)
				 {
					buffer[0][i] = ((readbuffer[i*4+1]<<8) | (0x00ff & (int)readbuffer[i*4])) / 32768.f;
					buffer[1][i] = ((readbuffer[i*4+3]<<8) | (0x00ff & (int)readbuffer[i*4+2])) / 32768.f;
				 }
			 }
			 else if(wfh.num_channels == 1 && es.channel == OH_Stereo)
			 {
				 //get the mono channel and add it to both left and right
				 for(i = 0; i < bytes/2; i++)
				 {
					float monoChl = ((readbuffer[i*2+1]<<8) | (0x00ff & (int)readbuffer[i*2])) / 32768.f;
					buffer[0][i] = monoChl;
					buffer[1][i] = monoChl;
				 }
			 }
			  else if(wfh.num_channels == 1 && es.channel == OH_Mono)
			 {
				 //get the mono channel
				 for(i = 0; i < bytes/2; i++)
				 {
					buffer[0][i] = ((readbuffer[i*2+1]<<8) | (0x00ff & (int)readbuffer[i*2])) / 32768.f;
				 }
			 }
			 else
			 {
				 sprintf(ErrMsg, "The input channel mix is currently not supported! Aborting!");
				 ErrorReport(ErrMsg);
				 hresult = FALSE;
				 /*if(callbackproc != NULL)
				 {
					 callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -6, NULL);
				 }*/
				 goto done;
			 }
			//tell the library how much we actually submitted
			vorbis_analysis_wrote(&vd, i);
		 }

		 //vorbis does some data preanalysis, then divvies up blocks for more involved (potentially parallel) processing.
		 //Get a single block for encoding now
		 while(vorbis_analysis_blockout(&vd, &vb) == 1)
		 {
			 //analysis, assume we want to use bitrate management
			 vorbis_analysis(&vb, NULL);
			 vorbis_bitrate_addblock(&vb);
			 while(vorbis_bitrate_flushpacket(&vd, &op))
			 {
				 //weld the packet into the bitstream
				 ogg_stream_packetin(&os, &op);

				 //write out pages (if any)
				 while(true)
				 {
					 int result = ogg_stream_pageout(&os, &og);
					 if(result == 0)break;
					 fwrite(og.header, 1, og.header_len, f_out);
					 fwrite(og.body, 1, og.body_len, f_out);
					 //this could be set above, but for illustrative purposes, I do it here (to show that vorbis does know where the stream ends)
					 if(ogg_page_eos(&og))break;
				 }
			 }
		 }
	 }

	 /*if(callbackproc != NULL)
	 {
		 callbackproc((HWND)GetModuleHandle(NULL), OH_DONE, NULL, NULL);
	 }*/

	 //clean up and exit. vorbis_info_clear() must be called last
	 ogg_stream_clear(&os);
	 vorbis_block_clear(&vb);
	 vorbis_dsp_clear(&vd);
	 vorbis_comment_clear(&vc);
	 vorbis_info_clear(&vi);
done:
	//if(wfh != NULL)
	//	delete wfh;
	if(f_in != NULL)
		fclose(f_in);
	if(f_out != NULL)
		fclose(f_out);
	return hresult;
}


//*****************************DECODING*************************************************************
//****************************************************************************************************


//The actual decode function
_BOOL oggHelper::decode(const char* file_in, const char* file_out/*, WNDPROC callbackproc*/)
{
	//printf("%s\n", "oog decode run...");

    _BOOL hresult = TRUE;
	wavHeader h;

	FILE* f_in = fopen(file_in, "rb");
	FILE* f_out = fopen(file_out, "wb");

	if(!f_in)
	{
		sprintf(ErrMsg, "Couldn't open %s for read! Aborting!", file_in);
		ErrorReport(ErrMsg);
		hresult = FALSE;
		/*if(callbackproc != NULL)
		{
			callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -1, NULL);
		}*/
		if(f_in != NULL)
			fclose(f_in);

		return hresult;
	}

	//printf("%s\n", "1");

	if(!f_out)
	{
		sprintf(ErrMsg, "Couldn't open %s for write! Aborting!", file_out);
		ErrorReport(ErrMsg);
		hresult = FALSE;
		/*if(callbackproc != NULL)
		{
			callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -3, NULL);
		}*/
		if(f_in != NULL)
			fclose(f_in);
		if(f_out != NULL)
			fclose(f_out);

		return hresult;
	}

	//printf("%s\n", "2");

	//Check
	OggVorbis_File vf;
	if(ov_open_callbacks(f_in, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
      //fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
      //exit(1);
		printf("%s\n", "Input does not appear to be an Ogg bitstream.");
		hresult = FALSE;
    fclose(f_in);
    fclose(f_out);
    return hresult;
  }

	//Write wav header
	vorbis_info *info=ov_info(&vf,-1);
  hresult = h.write_prelim_header(f_out, info->channels, info->rate);
  if (hresult != TRUE) {
  	printf("%s\n", "cant write prelim header");
    fclose(f_in);
    fclose(f_out);
    return hresult;
  }

  //printf("%s\n", "3");


	long cumulative_read = 0;
	long ogg_total_size = 0;
	fseek(f_in, 0, SEEK_END);
	ogg_total_size = ftell(f_in);
	fseek(f_in, 0, SEEK_SET);


	ogg_int16_t convbuffer[4096]; /* take 8k out of the data segment, not the stack */
	int convsize=4096;

	long byteWritten = 0;
	ogg_sync_state   oy; /* sync and verify incoming physical bitstream */
	ogg_stream_state os; /* take physical pages, weld into a logical
                          stream of packets */
	ogg_page         og; /* one Ogg bitstream page. Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */

	vorbis_info      vi; /* struct that stores all the static vorbis bitstream
                          settings */

	vorbis_comment   vc; /* struct that stores all the bitstream user comments */
	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */


	char *buffer;
	int  bytes;

	/********** Decode setup ************/
	ogg_sync_init(&oy); /* Now we can read pages */

	while(1)
	{
		/* we repeat if the bitstream is chained */
		int eos=0;
		int i;


		/* grab some data at the head of the stream. We want the first page
       (which is guaranteed to be small and only contain the Vorbis
       stream initial header) We need the first page to get the stream
       serialno. */

		/* submit a 4k block to libvorbis' Ogg layer */
		buffer=ogg_sync_buffer(&oy, 4096);
		bytes = fread(buffer, 1, 4096, f_in);
		ogg_sync_wrote(&oy, bytes);

		cumulative_read += bytes;
		//Percentage done
		/*if(callbackproc != NULL)
		{
			 int percentage = ((float)cumulative_read/ogg_total_size)*100;
			 callbackproc((HWND)GetModuleHandle(NULL), OH_COMPUTED, percentage, NULL);
		}*/

		/* Get the first page. */
		if(ogg_sync_pageout(&oy,&og)!= 1)
		{
			/* have we simply run out of data?  If so, we're done. */
			if(bytes<4096)
				break;

			/* error case.  Must not be Vorbis data */
			sprintf(ErrMsg, "Input does not appear to be an Ogg bitstream! Aborting!");
			ErrorReport(ErrMsg);
			hresult = FALSE;
			/*if(callbackproc != NULL)
			{
				callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -10, NULL);
			}*/
			goto done;
		}

		/* Get the serial number and set up the rest of decode. */
		/* serialno first; use it to set up a logical stream */
		ogg_stream_init(&os, ogg_page_serialno(&og));

		/* extract the initial header from the first page and verify that the
			Ogg bitstream is in fact Vorbis data */

		 /* I handle the initial header first instead of just having the code
		 read all three Vorbis headers at once because reading the initial
		 header is an easy way to identify a Vorbis bitstream and it's
		 useful to see that functionality seperated out. */


		vorbis_info_init(&vi);
		vorbis_comment_init(&vc);

		if(ogg_stream_pagein(&os,&og)<0)
		{
			/* error; stream version mismatch perhaps */
			sprintf(ErrMsg, "Error reading first page of Ogg bitstream data! Aborting!");
			ErrorReport(ErrMsg);
			hresult = FALSE;
			/*if(callbackproc != NULL)
			{
				callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -11, NULL);
			}*/
			goto done;
		}


		if(ogg_stream_packetout(&os,&op)!=1)
		{
			/* no page? must not be vorbis */
			sprintf(ErrMsg, "Error reading initial header packet! Aborting!");
			ErrorReport(ErrMsg);
			hresult = FALSE;
			/*if(callbackproc != NULL)
			{
				callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -12, NULL);
			}*/
			goto done;
		}

		if(vorbis_synthesis_headerin(&vi,&vc,&op)<0)
		{
			/* error case; not a vorbis header */
			sprintf(ErrMsg, "This Ogg bitstream does not contain Vorbis audio data! Aborting!");
			ErrorReport(ErrMsg);
			hresult = FALSE;
			/*if(callbackproc != NULL)
			{
				callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -13, NULL);
			}*/
			goto done;
		}


		/* At this point, we're sure we're Vorbis. We've set up the logical
       (Ogg) bitstream decoder. Get the comment and codebook headers and
       set up the Vorbis decoder */


		/* The next two packets in order are the comment and codebook headers.
       They're likely large and may span multiple pages. Thus we read
       and submit data until we get our two packets, watching that no
       pages are missing. If a page is missing, error out; losing a
       header page is the only place where missing data is fatal. */

		i=0;
		while(i<2)
		{
			while(i<2)
			{
				int result=ogg_sync_pageout(&oy,&og);
				if(result==0)break; /* Need more data */

				/* Don't complain about missing or corrupt data yet. We'll catch it at the packet output phase */
				if(result==1)
				{
					ogg_stream_pagein(&os,&og); /* we can ignore any errors here
                                         as they'll also become apparent
                                         at packetout */
					while(i<2)
					{

						result=ogg_stream_packetout(&os,&op);
						if(result==0)break;
						if(result<0)
						{
							/* Uh oh; data at some point was corrupted or missing! We can't tolerate that in a header.  Die. */
							sprintf(ErrMsg, "Corrupt secondary header! Aborting!");
							ErrorReport(ErrMsg);
							hresult = FALSE;
							/*if(callbackproc != NULL)
							{
								callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -14, NULL);
							}*/
							goto done;
						}
						result=vorbis_synthesis_headerin(&vi,&vc,&op);

						if(result<0)
						{
							sprintf(ErrMsg, "Corrupt secondary header! Aborting!");
							ErrorReport(ErrMsg);
							hresult = FALSE;
							/*if(callbackproc != NULL)
							{
								callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -15, NULL);
							}*/
							goto done;
						}
						i++;
					}
				}
			}

			/* no harm in not checking before adding more */
			buffer=ogg_sync_buffer(&oy,4096);
			bytes=fread(buffer, 1, 4096, f_in);

			cumulative_read += bytes;
			//Percentage done
			/*if(callbackproc != NULL)
			{
				int percentage = ((float)cumulative_read/ogg_total_size)*100;
				callbackproc((HWND)GetModuleHandle(NULL), OH_COMPUTED, percentage, NULL);
			}*/

			if(bytes==0 && i<2)
			{
				sprintf(ErrMsg, "End of file before finding all Vorbis headers! Aborting!");
				ErrorReport(ErrMsg);
				hresult = FALSE;
				/*if(callbackproc != NULL)
				{
					callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -16, NULL);
				}*/
				goto done;
			}
			ogg_sync_wrote(&oy,bytes);
		}


		/* Throw the comments plus a few lines about the bitstream we're decoding */
		/*{
			char **ptr=vc.user_comments;
			while(*ptr)
			{
				fprintf(stderr,"%s\n",*ptr);
				++ptr;
			}
			fprintf(stderr,"\nBitstream is %d channel, %ldHz\n",vi.channels,vi.rate);
			fprintf(stderr,"Encoded by: %s\n\n",vc.vendor);
		}*/

		convsize=4096/vi.channels;


		/* OK, got and parsed all three headers. Initialize the Vorbis packet->PCM decoder. */
		if(vorbis_synthesis_init(&vd,&vi)==0)
		{ /* central decode state */
			vorbis_block_init(&vd, &vb); /* local state for most of the decode so multiple block decodes can
										proceed in parallel. We could init
										multiple vorbis_block structures for vd here */

			/* The rest is just a straight decode loop until end of stream */
			while(!eos)
			{
				while(!eos)
				{
					int result=ogg_sync_pageout(&oy,&og);
					if(result==0)
						break; /* need more data */

					if(result<0)
					{
						/* missing or corrupt data at this page position */
						sprintf(ErrMsg, "Corrupt or missing data in bitstream! Continuing...");
						ErrorReport(ErrMsg);
					}
					else
					{
						ogg_stream_pagein(&os,&og); /* can safely ignore errors at this point */

						while(1)
						{
							result=ogg_stream_packetout(&os,&op);
							if(result==0)
								break; /* need more data */

							if(result<0)
							{ /* missing or corrupt data at this page position */
								/* no reason to complain; already complained above */
							}
							else
							{
								/* we have a packet.  Decode it */
								float **pcm;
								int samples;
								if(vorbis_synthesis(&vb,&op)==0) /* test for success! */
									vorbis_synthesis_blockin(&vd,&vb);

								/* **pcm is a multichannel float vector.  In stereo, for
									example, pcm[0] is left, and pcm[1] is right.  samples is
									the size of each channel.  Convert the float values
									(-1.<=range<=1.) to whatever PCM format and write it out */


								while((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0)
								{
									int j;
									int clipflag=0;
									int bout=(samples<convsize?samples:convsize);

									/* convert floats to 16 bit signed ints (host order) and interleave */
									for(i=0;i<vi.channels;i++)
									{
										ogg_int16_t *ptr=convbuffer+i;
										float  *mono=pcm[i];
										for(j=0;j<bout;j++)
										{
											int val=floor(mono[j]*32767.f+.5f);

											/* might as well guard against clipping */
											if(val>32767)
											{
												val=32767;
												clipflag=1;
											}
											if(val<-32768)
											{
												val=-32768;
												clipflag=1;
											}
											*ptr=val;
											ptr+=vi.channels;
										}
									}

									//if(clipflag)
										//fprintf(stderr,"Clipping in frame %ld\n",(long)(vd.sequence));

									int written = fwrite(convbuffer, 2*vi.channels, bout, f_out);
									byteWritten += (written*2*vi.channels);

									vorbis_synthesis_read(&vd, bout); /* tell libvorbis how many samples we actually consumed */
								}
							}
						}
						if(ogg_page_eos(&og))
							eos=1;
					}
				}

				if(!eos)
				{
					buffer=ogg_sync_buffer(&oy,4096);
					bytes=fread(buffer,1,4096, f_in);

					cumulative_read += bytes;
					//Percentage done
					/*if(callbackproc != NULL)
					{
						int percentage = ((float)cumulative_read/ogg_total_size)*100;
						callbackproc((HWND)GetModuleHandle(NULL), OH_COMPUTED, percentage, NULL);
					}*/

					ogg_sync_wrote(&oy,bytes);
					if(bytes==0)
						eos=1;
				}
			}

			/* ogg_page and ogg_packet structs always point to storage in libvorbis.  They're never freed or manipulated directly */
			vorbis_block_clear(&vb);
			vorbis_dsp_clear(&vd);

			//Seek back and write the WAV header
			//WaveFileHeader wfh = WaveFileHeader(vi.rate, 16, vi.channels, byteWritten);
			//fseek(f_out, 0, SEEK_SET);
			//fwrite(&wfh, sizeof(wfh), 1, f_out);

			hresult = h.rewrite_header(f_out, byteWritten);
			if (hresult != TRUE) {
				printf("%s\n", "cant rewrite header");
		    fclose(f_out);
		    fclose(f_in);
		    return hresult;
		  }


		}
		else
		{
			sprintf(ErrMsg, "Error: Corrupt header during playback initialization! Aborting!");
			ErrorReport(ErrMsg);
			hresult = FALSE;
			/*if(callbackproc != NULL)
			{
				callbackproc((HWND)GetModuleHandle(NULL), OH_ERROR, -17, NULL);
			}*/
		}

		/* clean up this logical bitstream; before exit we see if we're followed by another [chained] */
		ogg_stream_clear(&os);
		vorbis_comment_clear(&vc);
		vorbis_info_clear(&vi);  /* must be called last */
	}

	/* OK, clean up the framer */
	ogg_sync_clear(&oy);

	//printf("%s\n", "final");

done:
	if(f_in != NULL)
		fclose(f_in);
	if(f_out != NULL)
		fclose(f_out);

	return hresult;
}



void oggHelper::ErrorReport(char* ErrMsg)
{
	printf("\n%s\n", ErrMsg);
}
