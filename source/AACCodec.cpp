#include <fmt/format.h>
#include <AACCodec/AACCodec.h>
#include <AACCodec/Base64.h>
#include <cstdint>
#include <vector>


using namespace aaccodec;
using namespace std;


// AACCodec::AACCodec(std::string _name) : name(std::move(_name)) {}
AACCodec::AACCodec()  {}

std::string AACCodec::greet(LanguageCode lang) const {
//   dec[0]=1;
//   dec[1]=2;
//   dec[2]=3;
//   std::string ret = AACCodec::b64_encode((uint8_t *)dec, (size_t)3);	
  switch (lang) {
    default:
    case LanguageCode::EN:
	// return ret;
      return fmt::format("Hello AACCodec lib works ok!, {}!", name);
    case LanguageCode::DE:
      return fmt::format("Hallo {}!", name);
    case LanguageCode::ES:
      return fmt::format("¡Hola {}!", name);
    case LanguageCode::FR:
      return fmt::format("Bonjour {}!", name);
  }
}

std::string AACCodec::base64test(string src) {

	std::vector<uint8_t> srcInput(src.begin(), src.end());
	uint8_t byteArray[srcInput.size() * 2];

	int byteArraySize = FromBase64Fast(&srcInput[0], srcInput.size(), byteArray, srcInput.size() * 2);

	// std::string decoded = base64_decode((const std::string) src);
	// std::vector<uint8_t> byteArray(decoded.begin(), decoded.end());
	uint32_t sum = 0;
	for(int i = 0; i < byteArraySize; i++) {
		sum += byteArray[i];
	}
	uint8_t numberStr[4];
    memcpy(numberStr, &sum, 4);  //780 //0x30C
	// return fmt::format("Suma: {}", sum);

	// ToBase64Fast(numberStr, 4, );
	// return base64_encode((const unsigned char *)numberStr, 4, false);
	return ToBase64Fast((const unsigned char *)numberStr, 4);
	// return base64_encode(decoded);
}

int32_t AACCodec::sumArrayInt32 (int8_t *array, int32_t length) {
    int total = 0;

    for (int i = 0; i < length; ++i) {
        total += array[i];
    }

    return total;
}

int AACCodec::aacEncoderInit(int audioObjectType, int channels, int sampleRate, int bitRate, int bitRateMode, int trans_mux) {
  	AACENC_ERROR err = AACENC_OK;

	_h.aot = audioObjectType;
	_h.channels = channels;
	_h.sample_rate = sampleRate;
	_h.bitrate = bitRate;

	//int trans_mux = TT_MP4_ADTS;//2; // adts
	int signaling = 2; // Implicit backward compatible signaling (default for ADIF and ADTS)
	int afterburner = 1; // 1 or 0(default)

	CHANNEL_MODE mode = MODE_INVALID;
    switch (channels) {
		case 1: mode = MODE_1;       break;
		case 2: mode = MODE_2;       break;
		case 3: mode = MODE_1_2;     break;
		case 4: mode = MODE_1_2_1;   break;
		case 5: mode = MODE_1_2_2;   break;
		case 6: mode = MODE_1_2_2_1; break;
		default:
			return 1;
	}

	if ((err = aacEncOpen(&_h.enc, 0x0, channels)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_AOT, audioObjectType)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_SAMPLERATE, sampleRate)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_CHANNELMODE, mode)) != AACENC_OK) {
        return err;
    }

    // Input audio data channel ordering scheme:
    //      - 0: MPEG channel ordering (e. g. 5.1: C, L, R, SL, SR, LFE). (default)
    //      - 1: WAVE file format channel ordering (e. g. 5.1: L, R, C, LFE, SL, SR).
    if ((err = aacEncoder_SetParam(_h.enc, AACENC_CHANNELORDER, 1)) != AACENC_OK) {
        return err;
    }

	if ((err = aacEncoder_SetParam(_h.enc, AACENC_BITRATEMODE, bitRateMode)) != AACENC_OK) {
        return err;
    }

	if (!bitRateMode) {
		if ((err = aacEncoder_SetParam(_h.enc, AACENC_BITRATE, bitRate)) != AACENC_OK) {
			return err;
		}
	}

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_TRANSMUX, trans_mux)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_SIGNALING_MODE, signaling)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_AFTERBURNER, afterburner)) != AACENC_OK) {
        return err;
    }

	// if ((err = aacEncoder_SetParam(_h.enc, AACENC_HEADER_PERIOD, 1)) != AACENC_OK) {
    //     return err;
    // }

	// if ((err = aacEncoder_SetParam(_h.enc, AACENC_METADATA_MODE, 1)) != AACENC_OK) {
    //     return err;
    // }
	// if ((err = aacEncoder_SetParam(_h.enc, AACENC_PROTECTION, 1)) != AACENC_OK) {
    //     return err;
    // }

	if ((err = aacEncEncode(_h.enc, NULL, NULL, NULL, NULL)) != AACENC_OK) {
        return err;
    }

    

    // AACENC_InfoStruct 
	info = {0};
    if ((err = aacEncInfo(_h.enc, &info)) != AACENC_OK) {
    	return err;
    }

    // _h.frame_size = info.frameLength;

	outbuf = new uint8_t[out_size];
	return err;
}

std::string AACCodec::aacEncodeB64(std::string pcmB64) {
	std::vector<uint8_t> srcInput(pcmB64.begin(), pcmB64.end());
	uint8_t byteArray[srcInput.size() * 2];
	int byteArraySize = FromBase64Fast(&srcInput[0], srcInput.size(), byteArray, srcInput.size() * 2);

    int i;
    uint8_t *in_ptr = &byteArray[0];//pcmV.data();
	int in_size = byteArraySize;
	
	in_elem_size = 2;
	AACENC_InArgs in_args = { 0 };

	in_args.numInSamples = in_size <= 0 ? -1 : in_size / 2;
	in_buf.numBufs = 1;
	in_buf.bufs = (void**) &in_ptr;
	in_buf.bufferIdentifiers = &in_identifier;
	in_buf.bufSizes = &in_size;
	in_buf.bufElSizes = &in_elem_size;

	out_elem_size = 1;
	out_buf.numBufs = 1;
	out_buf.bufs = (void**) &outbuf;
	out_buf.bufferIdentifiers = &out_identifier;
	out_buf.bufSizes = &out_size;
	out_buf.bufElSizes = &out_elem_size;
	
	err = aacEncEncode(_h.enc, &in_buf, &out_buf, &in_args, &out_args);
	if (err != AACENC_OK) {
		// error
		// TODO! free(in_ptr);
		if (err != AACENC_ENCODE_EOF) {
			// LOG_FDK(FDKError,"Encoding failed: %s\n", setupErrorText(err));
			return "error";
		}
		return "";
	}

	if (out_args.numOutBytes) {
		return ToBase64Fast(&outbuf[0], out_args.numOutBytes);
		// return base64_encode(outbuf, out_args.numOutBytes, false);
	}

	return "";
}

int AACCodec::aacenc_init(int aot, int channels, int sample_rate, int bitrate)
{
	AACENC_ERROR err = AACENC_OK;

	_h.aot = aot;
	_h.channels = channels;
	_h.sample_rate = sample_rate;
	_h.bitrate = bitrate;

	// AACENC_TRANSMUX
    // Transport type to be used. See ::TRANSPORT_TYPE in FDK_audio.h.
    // Following types can be configured in encoder library:
    //         - 0: raw access units
    //         - 1: ADIF bitstream format
    //         - 2: ADTS bitstream format
    //         - 6: Audio Mux Elements (LATM) with muxConfigPresent = 1
    //         - 7: Audio Mux Elements (LATM) with muxConfigPresent = 0, out of band StreamMuxConfig
    //         - 10: Audio Sync Stream (LOAS)
	int trans_mux = 2; // adts
	int signaling = 0; // Implicit backward compatible signaling (default for ADIF and ADTS)
	int afterburner = 1; // 1 or 0(default)
	// -------------------------------------------------------------------------------
	//  ChannelMode           | ChCfg  | front_El      | side_El  | back_El  | lfe_El
	// -----------------------+--------+---------------+----------+----------+--------
	// MODE_1                 |      1 | SCE           |          |          |
	// MODE_2                 |      2 | CPE           |          |          |
	// MODE_1_2               |      3 | SCE, CPE      |          |          |
	// MODE_1_2_1             |      4 | SCE, CPE      |          | SCE      |
	// MODE_1_2_2             |      5 | SCE, CPE      |          | CPE      |
	// MODE_1_2_2_1           |      6 | SCE, CPE      |          | CPE      | LFE
	// MODE_1_2_2_2_1         |      7 | SCE, CPE, CPE |          | CPE      | LFE
	// -----------------------+--------+---------------+----------+----------+--------
	// MODE_7_1_REAR_SURROUND |      0 | SCE, CPE      |          | CPE, CPE | LFE
	// MODE_7_1_FRONT_CENTER  |      0 | SCE, CPE, CPE |          | CPE      | LFE
	// -------------------------------------------------------------------------------
	//  - SCE: Single Channel Element.
	//  - CPE: Channel Pair.
	//  - LFE: Low Frequency Element.
	CHANNEL_MODE mode = MODE_INVALID;
    switch (channels) {
		case 1: mode = MODE_1;       break;
		case 2: mode = MODE_2;       break;
		case 3: mode = MODE_1_2;     break;
		case 4: mode = MODE_1_2_1;   break;
		case 5: mode = MODE_1_2_2;   break;
		case 6: mode = MODE_1_2_2_1; break;
		default:
			return 1;
	}

	if ((err = aacEncOpen(&_h.enc, 0, channels)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_AOT, aot)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_SAMPLERATE, sample_rate)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_CHANNELMODE, mode)) != AACENC_OK) {
        return err;
    }

    // Input audio data channel ordering scheme:
    //      - 0: MPEG channel ordering (e. g. 5.1: C, L, R, SL, SR, LFE). (default)
    //      - 1: WAVE file format channel ordering (e. g. 5.1: L, R, C, LFE, SL, SR).
    if ((err = aacEncoder_SetParam(_h.enc, AACENC_CHANNELORDER, 1)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_BITRATE, bitrate)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_TRANSMUX, trans_mux)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_SIGNALING_MODE, signaling)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncoder_SetParam(_h.enc, AACENC_AFTERBURNER, afterburner)) != AACENC_OK) {
        return err;
    }

    if ((err = aacEncEncode(_h.enc, NULL, NULL, NULL, NULL)) != AACENC_OK) {
        return err;
    }

    AACENC_InfoStruct info = {0};
    if ((err = aacEncInfo(_h.enc, &info)) != AACENC_OK) {
    	return err;
    }

    _h.frame_size = info.frameLength;

	return err;
}

void AACCodec::aacenc_close()
{
	aacEncClose(&_h.enc);
}

int AACCodec::aacenc_encode(char *pcm, int nb_pcm, int nb_samples, char *aac, int &pnb_aac)
{
	AACENC_ERROR err = AACENC_OK;
    INT iidentify = IN_AUDIO_DATA;
    INT oidentify = OUT_BITSTREAM_DATA;
	INT ibuffer_element_size = 2; // 16bits.
	INT ibuffer_size = 2 * _h.channels * nb_samples;
	// The intput pcm must be resampled to fit the encoder,
	// for example, the intput is 2channels but encoder is 1channels,
	// then we should resample the intput pcm to 1channels
	// to make the intput pcm size equals to the encoder calculated size(ibuffer_size).
	// std::cout << ibuffer_size << std::endl;
	if (ibuffer_size != nb_pcm) {
		return -1;
	}

	AACENC_BufDesc ibuf = {0};
	if (nb_pcm > 0) {
		ibuf.numBufs = 1;
		ibuf.bufs = (void**)&pcm;
		ibuf.bufferIdentifiers = &iidentify;
		ibuf.bufSizes = &ibuffer_size;
		ibuf.bufElSizes = &ibuffer_element_size;
	}
	AACENC_InArgs iargs = {0};
	if (nb_pcm > 0) {
		iargs.numInSamples =  _h.channels * nb_samples;
	} else {
		iargs.numInSamples = -1;
	}
	INT obuffer_element_size = 1;
	INT obuffer_size = pnb_aac;
	AACENC_BufDesc obuf = {0};
	obuf.numBufs = 1;
	obuf.bufs = (void**)&aac;
	obuf.bufferIdentifiers = &oidentify;
	obuf.bufSizes = &obuffer_size;
	obuf.bufElSizes = &obuffer_element_size;
	AACENC_OutArgs oargs = {0};
	if ((err = aacEncEncode(_h.enc, &ibuf, &obuf, &iargs, &oargs)) != AACENC_OK) {
		// Flush ok, no bytes to output anymore.
		if (!pcm && err == AACENC_ENCODE_EOF) {
			pnb_aac = 0;
			return AACENC_OK;
		}
		return err;
	}

	pnb_aac = oargs.numOutBytes;
	return err;
}

int AACCodec::aacenc_frame_size()
{
	return _h.frame_size;
}

int AACCodec::aacenc_max_output_buffer_size()
{
	int out_buffer_size = 8192;
	if (_h.channels * 768 > out_buffer_size) {
		out_buffer_size = _h.channels * 768;
	}
	
	return out_buffer_size;
}