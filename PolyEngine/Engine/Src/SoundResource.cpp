#include "EnginePCH.hpp"

#include <al.h>
#include <ogg/ogg.h>
#include <vorbis\vorbisfile.h>
#include <fstream>

#include "SoundResource.hpp"

using namespace Poly;

SoundResource::SoundResource(const String& path)
{
	alGenBuffers(1, &BufferID);

	// Declarations and loading file to buffer.

	BinaryBuffer* data = LoadBinaryFile(path);
	BinaryBuffer* dupa = new BinaryBuffer(0);

	ogg_sync_state   syncState;		/* sync and verify incoming physical bitstream */
	ogg_stream_state streamState;	/* take physical pages, weld into a logicalstream of packets */
	ogg_page         page;			/* one Ogg bitstream page. Vorbis packets are inside */
	ogg_packet       packet;		/* one raw packet of data for decode */

	vorbis_info      vorbisInfo;	/* struct that stores all the static vorbis bitstream settings */
	vorbis_comment   vorbisComment; /* struct that stores all the bitstream user comments */
	vorbis_dsp_state vorbisDSPState;/* central working state for the packet->PCM decoder */
	vorbis_block     vorbisBlock;	/* local working space for packet->PCM decode */

	ogg_int16_t convbuffer[4096];	/* take 8k out of the data segment, not the stack */
	int convsize = 4096;

	// Checking vorbis headers

	ogg_sync_init(&syncState);

	while (true)
	{
		int eos = 0;
		int i = 0;
		char* buffer;
		size_t bytesRead = 0;

		buffer = ogg_sync_buffer(&syncState, 4096);
		memcpy(buffer, reinterpret_cast<void*>(reinterpret_cast<size_t>(data->GetBegin()) + bytesRead), 4096);
		bytesRead += 4096;
		if (bytesRead > data->GetSize()) break;
		else ogg_sync_wrote(&syncState, 4096);

		if (ogg_sync_pageout(&syncState, &page) != 1)
		{
			gConsole.LogDebug("Input does not appear to be an Ogg bitstream.");
			throw OggDecoderException();
		}

		ogg_stream_init(&streamState, ogg_page_serialno(&page));

		vorbis_info_init(&vorbisInfo);
		vorbis_comment_init(&vorbisComment);
		if (ogg_stream_pagein(&streamState, &page) < 0)
		{
			gConsole.LogDebug("Error reading first page of Ogg bitstream data.");
			throw OggDecoderException();
		}

		if (ogg_stream_packetout(&streamState, &packet) != 1)
		{ 
			gConsole.LogDebug("Error reading initial header packet.");
			throw OggDecoderException();
		}

		if (vorbis_synthesis_headerin(&vorbisInfo, &vorbisComment, &packet) < 0)
		{ 
			gConsole.LogDebug("This Ogg bitstream does not contain Vorbis audio data.");
			throw OggDecoderException();
		}

		while (i < 2)
		{
			while (i < 2)
			{
				int result = ogg_sync_pageout(&syncState, &page);

				if (result == 0) break; 
				if (result == 1)
				{
					ogg_stream_pagein(&streamState, &page); 
					while (i < 2){
						result = ogg_stream_packetout(&streamState, &packet);
						if (result == 0) break;
						if (result < 0)
						{ 
							gConsole.LogDebug("Corrupt secondary header.  Exiting.");
							throw OggDecoderException();
						}

						result = vorbis_synthesis_headerin(&vorbisInfo, &vorbisComment, &packet);
						if (result < 0)
						{
							gConsole.LogDebug("Corrupt secondary header.  Exiting.");
							throw OggDecoderException();
						}

						i++;
					}
				}
			}

			buffer = ogg_sync_buffer(&syncState, 4096);
			memcpy(buffer, reinterpret_cast<void*>(reinterpret_cast<size_t>(data->GetBegin()) + bytesRead), 4096);
			bytesRead += 4096;
			if (bytesRead > data->GetSize() && i < 2)
			{
				gConsole.LogDebug("End of file before finding all Vorbis headers!");
				throw OggDecoderException();
			}
			else ogg_sync_wrote(&syncState, 4096);
		}

		// stats for nerds
		{
			char** ptr = vorbisComment.user_comments;
			while(*ptr)
			{
				gConsole.LogDebug("{}", *ptr);
				++ptr;
			}
			gConsole.LogDebug("Bitstream is {} channel, {}Hz", vorbisInfo.channels, vorbisInfo.rate);
			gConsole.LogDebug("Encoded by: {}", vorbisComment.vendor);
		}

		// vorbis headers checked; time to initialize "vorbis data -> PCM" decoder

		convsize = 4096 / vorbisInfo.channels;

		if (vorbis_synthesis_init(&vorbisDSPState, &vorbisInfo) == 0) 
		{										  
			vorbis_block_init(&vorbisDSPState, &vorbisBlock);

			// Decoder initialized; decoding until end of stream (eos)

			while (!eos)
			{
				while (!eos)
				{
					int result = ogg_sync_pageout(&syncState, &page);
					if (result == 0) break; 
					if (result< 0) gConsole.LogDebug("Corrupt or missing data in bitstream; continuing...");
					else
					{
						ogg_stream_pagein(&streamState, &page); 
						while (true)
						{
							result = ogg_stream_packetout(&streamState, &packet);

							if (result == 0) break; 
							if (result < 0) continue;
							else
							{
								float** pcm;
								int samples;

								if (vorbis_synthesis(&vorbisBlock, &packet) == 0)
									vorbis_synthesis_blockin(&vorbisDSPState, &vorbisBlock);

								while ((samples = vorbis_synthesis_pcmout(&vorbisDSPState, &pcm)) > 0)
								{
									int j;
									int clipflag = 0;
									int bout = (samples < convsize ? samples : convsize);

									for (i = 0; i < vorbisInfo.channels; i++)
									{
										ogg_int16_t* ptr = convbuffer + i;
										float* mono = pcm[i];
										for (j = 0; j < bout; j++)
										{
											int val = floor(mono[j] * 32767.f+.5f);

											if (val > 32767)
											{
												val = 32767;
												clipflag = 1;
											}

											if (val < -32768)
											{
												val = -32768;
												clipflag = 1;
											}

											*ptr = val;
											ptr += vorbisInfo.channels;
										}
									}

									if (clipflag)
										gConsole.LogDebug("Clipping in frame {}", (long)(vorbisDSPState.sequence));

									dupa->AddData(convbuffer, 2 * vorbisInfo.channels * bout);

									vorbis_synthesis_read(&vorbisDSPState, bout); 
								}            
							}
						}
						if (ogg_page_eos(&page)) eos = 1;
					}
					// End of page; load next page
				}
				// End of pages; load data with new pages
				if (!eos)
				{
					if (bytesRead > data->GetSize())
					{
						eos = 1;
						break;
					}

					buffer = ogg_sync_buffer(&syncState, 4096);
					memcpy(buffer, reinterpret_cast<void*>(reinterpret_cast<size_t>(data->GetBegin()) + bytesRead), bytesRead <= data->GetSize() ? 4096 : bytesRead - data->GetSize());
					bytesRead += 4096;
					if (bytesRead > data->GetSize()) ogg_sync_wrote(&syncState, bytesRead - data->GetSize());
					else ogg_sync_wrote(&syncState, 4096);

				}
			}

			vorbis_block_clear(&vorbisBlock);
			vorbis_dsp_clear(&vorbisDSPState);
		}
		else gConsole.LogDebug("Error: Corrupt header during playback initialization.");

		// TODO: loading chained sounds;
		alBufferData(BufferID, AL_FORMAT_STEREO16, dupa->GetBegin(), dupa->GetSize(), vorbisInfo.rate);

		ogg_stream_clear(&streamState);
		vorbis_comment_clear(&vorbisComment);
		vorbis_info_clear(&vorbisInfo);

		if (bytesRead > data->GetSize()) break;
	}

	ogg_sync_clear(&syncState);

	delete dupa;
	delete data;

	//********************************************************************************************

#if  false
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
	size_t bytesRead = 0;
	ogg_int16_t convbuffer[4096]; /* take 8k out of the data segment, not the stack */
	int convsize = 4096;

	/********** Decode setup ************/

	ogg_sync_init(&oy); /* Now we can read pages */

	while (true) /* we repeat if the bitstream is chained */
	{ 
		int eos = 0;
		int i;

		/* grab some data at the head of the stream. We want the first page
		(which is guaranteed to be small and only contain the Vorbis
		stream initial header) We need the first page to get the stream
		serialno. */

		/* submit a 4k block to libvorbis' Ogg layer */
		buffer = ogg_sync_buffer(&oy, 4096);
		memcpy(buffer, reinterpret_cast<void*>(reinterpret_cast<size_t>(data->GetBegin()) + bytesRead), 4096);
		bytesRead += 4096;
		if (bytesRead > data->GetSize()){
			 break;
		}
		else ogg_sync_wrote(&oy, 4096);

		/* Get the first page. */
		if (ogg_sync_pageout(&oy, &og) != 1)
		{
			/* have we simply run out of data?  If so, we're done. */
			if(bytesRead > data->GetSize()) break;

			/* error case.  Must not be Vorbis data */
			gConsole.LogDebug("Input does not appear to be an Ogg bitstream.");
			throw OggDecoderException();
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
		if (ogg_stream_pagein(&os, &og) < 0)
		{
			/* error; stream version mismatch perhaps */
			gConsole.LogDebug("Error reading first page of Ogg bitstream data.");
			throw OggDecoderException();
		}

		if (ogg_stream_packetout(&os, &op) != 1)
		{ 
			/* no page? must not be vorbis */
			gConsole.LogDebug("Error reading initial header packet.");
			throw OggDecoderException();
		}

		if (vorbis_synthesis_headerin(&vi, &vc, &op) < 0)
		{ 
			/* error case; not a vorbis header */
			gConsole.LogDebug("This Ogg bitstream does not contain Vorbis audio data.");
			throw OggDecoderException();
		}

		/* At this point, we're sure we're Vorbis. We've set up the logical
		(Ogg) bitstream decoder. Get the comment and codebook headers and
		set up the Vorbis decoder */

		/* The next two packets in order are the comment and codebook headers.
		They're likely large and may span multiple pages. Thus we read
		and submit data until we get our two packets, watching that no
		pages are missing. If a page is missing, error out; losing a
		header page is the only place where missing data is fatal. */

		i = 0;
		while (i < 2)
		{
			while (i < 2)
			{
				int result = ogg_sync_pageout(&oy, &og);
				if (result == 0) break; /* Need more data */
										/* Don't complain about missing or corrupt data yet. We'll
										catch it at the packet output phase */
				if (result == 1)
				{
					ogg_stream_pagein(&os, &og); /* we can ignore any errors here
											   	 as they'll also become apparent
												 at packetout */
					while (i < 2){
						result = ogg_stream_packetout(&os, &op);
						if (result == 0) break;
						if (result < 0)
						{ 
							/* Uh oh; data at some point was corrupted or missing!
							We can't tolerate that in a header.  Die. */
							gConsole.LogDebug("Corrupt secondary header.  Exiting.");
							throw OggDecoderException();
						}
						result = vorbis_synthesis_headerin(&vi, &vc, &op);
						if (result < 0)
						{
							gConsole.LogDebug("Corrupt secondary header.  Exiting.");
							throw OggDecoderException();
						}
						i++;
					}
				}
			}
			/* no harm in not checking before adding more */
			buffer = ogg_sync_buffer(&oy, 4096);
			memcpy(buffer, reinterpret_cast<void*>(reinterpret_cast<size_t>(data->GetBegin()) + bytesRead), 4096);
			bytesRead += 4096;
			if (bytesRead > data->GetSize() && i < 2)
			{
				gConsole.LogDebug("End of file before finding all Vorbis headers!");
				throw OggDecoderException();
			}
			else ogg_sync_wrote(&oy, 4096);
		}

		/* Throw the comments plus a few lines about the bitstream we're
		decoding */
		{
			char** ptr = vc.user_comments;
			while(*ptr)
			{
				gConsole.LogDebug("{}", *ptr);
				++ptr;
			}
			gConsole.LogDebug("Bitstream is {} channel, {}Hz", vi.channels, vi.rate);
			gConsole.LogDebug("Encoded by: {}", vc.vendor);
		}

		convsize = 4096 / vi.channels;

		/* OK, got and parsed all three headers. Initialize the Vorbis
		packet->PCM decoder. */
		if (vorbis_synthesis_init(&vd, &vi) == 0) /* central decode state */
										          /* local state for most of the decode
												  so multiple block decodes can
												  proceed in parallel. We could init
												  multiple vorbis_block structures
												  for vd here */

		{										  /* The rest is just a straight decode loop until end of stream */
			vorbis_block_init(&vd, &vb);
			while (!eos)
			{
				while (!eos)
				{
					int result = ogg_sync_pageout(&oy, &og);
					if (result == 0) break; /* need more data */
					if (result< 0)	/* missing or corrupt data at this page position */
					{ 	
						gConsole.LogDebug("Corrupt or missing data in bitstream; continuing...");
					}
					else
					{
						ogg_stream_pagein(&os, &og); /* can safely ignore errors at
													 this point */
						while (true)
						{
							result = ogg_stream_packetout(&os, &op);

							if (result == 0) break; /* need more data */
							if (result < 0) /* missing or corrupt data at this page position */
										    /* no reason to complain; already complained above */
							{
							}
							else
							{
								/* we have a packet.  Decode it */
								float** pcm;
								int samples;

								if (vorbis_synthesis(&vb, &op) == 0) /* test for success! */
									vorbis_synthesis_blockin(&vd, &vb);
								/* 

								**pcm is a multichannel float vector.  In stereo, for
								example, pcm[0] is left, and pcm[1] is right.  samples is
								the size of each channel.  Convert the float values
								(-1.<=range<=1.) to whatever PCM format and write it out */

								while ((samples = vorbis_synthesis_pcmout(&vd, &pcm)) > 0)
								{
									int j;
									int clipflag = 0;
									int bout = (samples < convsize ? samples : convsize);

									/* convert floats to 16 bit signed ints (host order) and
									interleave */
									for (i = 0; i < vi.channels; i++)
									{
										ogg_int16_t* ptr = convbuffer + i;
										float* mono = pcm[i];
										for (j = 0; j < bout; j++)
										{
											int val = floor(mono[j] * 32767.f+.5f);
											/* might as well guard against clipping */
											if (val > 32767)
											{
												val = 32767;
												clipflag = 1;
											}
											if (val < -32768)
											{
												val = -32768;
												clipflag = 1;
											}
											*ptr = val;
											ptr += vi.channels;
										}
									}

									if (clipflag)
										gConsole.LogDebug("Clipping in frame {}", (long)(vd.sequence));

									dupa->AddData(convbuffer, 2 * vi.channels * bout);

									vorbis_synthesis_read(&vd, bout); /* tell libvorbis how
																	  many samples we
																	  actually consumed */
								}            
							}
						}
						if (ogg_page_eos(&og)) eos = 1;
					}
				}
				if (!eos)
				{
					buffer = ogg_sync_buffer(&oy, 4096);
					memcpy(buffer, reinterpret_cast<void*>(reinterpret_cast<size_t>(data->GetBegin()) + bytesRead), 4096);
					bytesRead += 4096;
					if (bytesRead > data->GetSize()) ogg_sync_wrote(&oy, bytesRead - data->GetSize());
					else ogg_sync_wrote(&oy, 4096);

					if (bytesRead - data->GetSize()) eos = 1;
				}
			}

			/* ogg_page and ogg_packet structs always point to storage in
			libvorbis.  They're never freed or manipulated directly */

			vorbis_block_clear(&vb);
			vorbis_dsp_clear(&vd);
		}
		else
		{
			gConsole.LogDebug("Error: Corrupt header during playback initialization.");
		}

		/* clean up this logical bitstream; before exit we see if we're
		followed by another [chained] */

		// TODO: loading chained sounds;
		alBufferData(BufferID, AL_FORMAT_STEREO16, dupa->GetBegin(), dupa->GetSize(), vi.rate);

		ogg_stream_clear(&os);
		vorbis_comment_clear(&vc);
		vorbis_info_clear(&vi);  /* must be called last */
	}

	/* OK, clean up the framer */
	ogg_sync_clear(&oy);

	ogg_sync_init(&oy); /* Now we can read pages */


	// zajebista fala
	/*float freq = 200.f;
	int seconds = 7 * 2;
	unsigned sample_rate = 192000;
	size_t buf_size = seconds * sample_rate;

	short *samples;
	samples = new short[buf_size];
	for(int i = 0; i < buf_size; ++i) 
		samples[i] = 20000 * sin((2.f * i * i * float(M_PI) ) * (freq / sample_rate));*/

#endif //  
}

SoundResource::~SoundResource()
{	
	alDeleteBuffers(1, &BufferID);
}
