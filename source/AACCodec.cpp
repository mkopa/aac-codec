#include <fmt/format.h>
#include <AACCodec/AACCodec.h>
#include <cstdint>
#include <vector>


using namespace aaccodec;
using namespace std;


AACCodec::AACCodec(std::string _name) : name(std::move(_name)) {}

// std::string base64_encode(const unsigned char *data,
//                     size_t input_length,
//                     size_t *output_length) {
std::string AACCodec::b64_encode (const uint8_t *src, size_t len) {
  int i = 0;
  int j = 0;
  char *enc = NULL;
  size_t size = 0;
  unsigned char buf[4];
  unsigned char tmp[3];

  // alloc
  enc = (char *) malloc(size);
  if (NULL == enc) { return NULL; }

  // parse until end of source
  while (len--) {
    // read up to 3 bytes at a time into `tmp'
    tmp[i++] = *(src++);

    // if 3 bytes read then encode into `buf'
    if (3 == i) {
      buf[0] = (tmp[0] & 0xfc) >> 2;
      buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
      buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
      buf[3] = tmp[2] & 0x3f;

      // allocate 4 new byts for `enc` and
      // then translate each encoded buffer
      // part by index from the base 64 index table
      // into `enc' unsigned char array
      enc = (char *) realloc(enc, size + 4);
      for (i = 0; i < 4; ++i) {
        enc[size++] = b64_table[buf[i]];
      }

      // reset index
      i = 0;
    }
  }

  // remainder
  if (i > 0) {
    // fill `tmp' with `\0' at most 3 times
    for (j = i; j < 3; ++j) {
      tmp[j] = '\0';
    }

    // perform same codec as above
    buf[0] = (tmp[0] & 0xfc) >> 2;
    buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
    buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
    buf[3] = tmp[2] & 0x3f;

    // perform same write to `enc` with new allocation
    for (j = 0; (j < i + 1); ++j) {
      enc = (char *) realloc(enc, size + 1);
      enc[size++] = b64_table[buf[j]];
    }

    // while there is still a remainder
    // append `=' to `enc'
    while ((i++ < 3)) {
      enc = (char *) realloc(enc, size + 1);
      enc[size++] = '=';
    }
  }

  // Make sure we have enough space to add '\0' character at end.
  enc = (char *) realloc(enc, size + 1);
  enc[size] = '\0';

  return std::string(enc);
}

size_t AACCodec::b64_decode(std::string src) {
  int i = 0;
  int j = 0;
  int l = 0;
  int len = src.length();
  size_t size = 0;
//   unsigned char *dec = NULL;
  unsigned char buf[3];
  unsigned char tmp[4];

  // alloc
//   dec = (unsigned char *) malloc();
//   if (NULL == dec) { return NULL; }

  // parse until end of source
  while (len--) {
    // break if char is `=' or not base64 char
    if ('=' == src[j]) { break; }
    if (!(isalnum(src[j]) || '+' == src[j] || '/' == src[j])) { break; }

    // read up to 4 bytes at a time into `tmp'
    tmp[i++] = src[j++];

    // if 4 bytes read then decode into `buf'
    if (4 == i) {
      // translate values in `tmp' from table
      for (i = 0; i < 4; ++i) {
        // find translation char in `b64_table'
        for (l = 0; l < 64; ++l) {
          if (tmp[i] == b64_table[l]) {
            tmp[i] = l;
            break;
          }
        }
      }

      // decode
      buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
      buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
      buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

      // write decoded buffer to `dec'
    //   dec = (unsigned char *) realloc(dec, size + 3);
    //   if (dec != NULL){
        for (i = 0; i < 3; ++i) {
          dec[size++] = buf[i];
        }
    //   } else {
    //     return NULL;
    //   }

      // reset
      i = 0;
    }
  }

  // remainder
  if (i > 0) {
    // fill `tmp' with `\0' at most 4 times
    for (j = i; j < 4; ++j) {
      tmp[j] = '\0';
    }

    // translate remainder
    for (j = 0; j < 4; ++j) {
        // find translation char in `b64_table'
        for (l = 0; l < 64; ++l) {
          if (tmp[j] == b64_table[l]) {
            tmp[j] = l;
            break;
          }
        }
    }

    // decode remainder
    buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
    buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
    buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

    // write remainer decoded buffer to `dec'
    // dec = (unsigned char *)realloc(dec, size + (i - 1));
    // if (dec != NULL){
      for (j = 0; (j < i - 1); ++j) {
        dec[size++] = buf[j];
      }
    // } else {
    //   return NULL;
    // }
  }

  // Make sure we have enough space to add '\0' character at end.
//   dec = (unsigned char *)realloc(dec, size + 1);
//   if (dec != NULL){
    dec[size] = '\0';
//   } else {
//     return NULL;
//   }

  return size;
}

std::string AACCodec::greet(LanguageCode lang) const {
  switch (lang) {
    default:
    case LanguageCode::EN:
      return fmt::format("Hello AACCodec lib works ok!, {}!", name);
    case LanguageCode::DE:
      return fmt::format("Hallo {}!", name);
    case LanguageCode::ES:
      return fmt::format("¡Hola {}!", name);
    case LanguageCode::FR:
      return fmt::format("Bonjour {}!", name);
  }
}

int32_t AACCodec::sumArrayInt32 (int8_t *array, int32_t length) {
    int total = 0;

    for (int i = 0; i < length; ++i) {
        total += array[i];
    }

    return total;
}

// int32_t AACCodec::sumArrayInt32(vector<int32_t> array) {
//     int total = 0;

// 	for (int n : array) {
//         total += n;
//     }

//     return total;
// }

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