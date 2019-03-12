#include <EnginePCH.hpp>

#include <Resources/SoundResource.hpp>

using namespace Poly;

SoundResource::SoundResource(const String& path)
{
	alGenBuffers(1, &BufferID);

	// Declarations and loading file to buffer.

	BinaryBuffer* data = LoadBinaryFile(path);
	Dynarray<char> rawData;

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
		memcpy(buffer, reinterpret_cast<void*>(reinterpret_cast<size_t>(data->GetData()) + bytesRead), 4096);
		bytesRead += 4096;
		if (bytesRead >= data->GetSize()) break;
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

			auto increase = long(bytesRead + 4096 <= data->GetSize() ? 4096 : data->GetSize() - bytesRead);
			buffer = ogg_sync_buffer(&syncState, increase);
			memcpy(buffer, data->GetData() + bytesRead, increase);
			bytesRead += increase;
			if (bytesRead >= data->GetSize() && i < 2)
			{
				gConsole.LogDebug("End of file before finding all Vorbis headers!");
				throw OggDecoderException();
			}
			else ogg_sync_wrote(&syncState, increase);
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

		gConsole.LogDebug("blocksize: {}", vorbis_info_blocksize(&vorbisInfo, 1));

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
									//int clipflag = 0;
									int bout = (samples < convsize ? samples : convsize);

									for (i = 0; i < vorbisInfo.channels; i++)
									{
										ogg_int16_t* ptr = convbuffer + i;
										float* mono = pcm[i];
										for (j = 0; j < bout; j++)
										{
											auto val = int(floor(mono[j] * 32767.f+.5f));

											if (val > 32767)
											{
												val = 32767;
												//clipflag = 1;
											}

											if (val < -32768)
											{
												val = -32768;
												//clipflag = 1;
											}

											*ptr = val;
											ptr += vorbisInfo.channels;
										}
									}

									//if (clipflag)
									//	gConsole.LogDebug("Clipping in frame {}", (long)(vorbisDSPState.sequence));

									const size_t newBlocksize = 2 * vorbisInfo.channels * bout;
									const size_t oldDataSize = rawData.GetSize();
									if(oldDataSize + newBlocksize > rawData.GetCapacity())
									{
										const size_t newCap = std::max(rawData.GetCapacity(), newBlocksize);
										rawData.Reserve(newCap * 2);
									}
									rawData.Resize(oldDataSize + newBlocksize);
									memcpy(rawData.GetData() + oldDataSize, convbuffer, newBlocksize);

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
					if (bytesRead >= data->GetSize())
					{
						eos = 1;
						break;
					}



					auto increase = long(bytesRead + 4096 <= data->GetSize() ? 4096 : data->GetSize() - bytesRead);
					buffer = ogg_sync_buffer(&syncState, increase);
					memcpy(buffer, data->GetData() + bytesRead, increase);
					bytesRead += increase;

					ogg_sync_wrote(&syncState, increase);
				}
			}

			vorbis_block_clear(&vorbisBlock);
			vorbis_dsp_clear(&vorbisDSPState);
		}
		else gConsole.LogDebug("Error: Corrupt header during playback initialization.");

		// TODO: loading chained sounds;
		alBufferData(BufferID, AL_FORMAT_STEREO16, rawData.GetData(), (ALsizei)rawData.GetSize(), vorbisInfo.rate);

		ogg_stream_clear(&streamState);
		vorbis_comment_clear(&vorbisComment);
		vorbis_info_clear(&vorbisInfo);

		if (bytesRead >= data->GetSize()) break;
	}

	ogg_sync_clear(&syncState);

	delete data;
}

SoundResource::~SoundResource()
{
	alDeleteBuffers(1, &BufferID);
}
