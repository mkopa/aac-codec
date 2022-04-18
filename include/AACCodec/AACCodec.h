#ifndef AACCODEC_H
#define AACCODEC_H


#include <aacenc_lib.h>
#include <string>
#include <cstdint>
#include <vector>



namespace aaccodec {

using namespace std;

  class aacenc_t {
public:
	// the encoder handler.
	HANDLE_AACENCODER enc;

	// encoder info.
	int frame_size;

	// user specified params.
	int aot;
	int channels;
	int sample_rate;
	int bitrate;
};


  /**  Language codes to be used with the AACCodec class */
  enum class LanguageCode { EN, DE, ES, FR };
  /*!< Audio object type. See ::AUDIO_OBJECT_TYPE in FDK_audio.h.
                   - 2: MPEG-4 AAC Low Complexity.
                   - 5: MPEG-4 AAC Low Complexity with Spectral Band Replication
                 (HE-AAC).
                   - 29: MPEG-4 AAC Low Complexity with Spectral Band
                 Replication and Parametric Stereo (HE-AAC v2). This
                 configuration can be used only with stereo input audio data.
                   - 23: MPEG-4 AAC Low-Delay.
                   - 39: MPEG-4 AAC Enhanced Low-Delay. Since there is no
                 ::AUDIO_OBJECT_TYPE for ELD in combination with SBR defined,
                 enable SBR explicitely by ::AACENC_SBR_MODE parameter. The ELD
                 v2 212 configuration can be configured by ::AACENC_CHANNELMODE
                 parameter.
                   - 129: MPEG-2 AAC Low Complexity.
                   - 132: MPEG-2 AAC Low Complexity with Spectral Band
                 Replication (HE-AAC).

                   Please note that the virtual MPEG-2 AOT's basically disables
                 non-existing Perceptual Noise Substitution tool in AAC encoder
                 and controls the MPEG_ID flag in adts header. The virtual
                 MPEG-2 AOT doesn't prohibit specific transport formats. */
  enum class AudioObjectType { LC_MPEG4 = 2 };

  /**
   * @brief A class for saying hello in multiple languages
   */
  class AACCodec {
    std::string name;
  public:
    /**
     * @brief Creates a new AACCodec
     * @param name the name to greet
     */
    AACCodec();

    /**
     * @brief Creates a localized string containing the greeting
     * @param lang the language to greet in
     * @return a string containing the greeting
     */
    std::string greet(LanguageCode lang = LanguageCode::EN) const;
    int32_t sumArrayInt32 (int8_t *array, int32_t length);
    int aacenc_init(int aot, int channels, int sample_rate, int bitrate);
    int aacenc_max_output_buffer_size();
    int aacenc_frame_size();
    int aacenc_encode(char *pcm, int nb_pcm, int nb_samples, char *aac, int &pnb_aac);
    void aacenc_close();
    std::string base64test(std::string src);
    int aacEncoderInit(int audioObjectType, int channels, int sampleRate, int bitRate, int bitRateMode);
    std::string aacEncodeB64(std::string pcmB64);
  private:
    aacenc_t _h;
  protected:
    // common variables
    int vbr = 1; // variable bitrate mode
    int bitrate = 0; // automatic determination
    int ch = 0;
    int format, sample_rate, channels=2, bits_per_sample;
    int aot = 2;
    bool afterburner = false;
    int eld_sbr = 0;
    // HANDLE_AACENCODER handle;
    CHANNEL_MODE mode;
    AACENC_InfoStruct info = { 0 };
    // loop variables
    AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
    AACENC_InArgs in_args = { 0 };
    AACENC_OutArgs out_args = { 0 };
    int in_identifier = IN_AUDIO_DATA;
    int in_elem_size;
    int out_identifier = OUT_BITSTREAM_DATA;
    int out_elem_size=1;
    uint8_t* outbuf = nullptr;
    // uint8_t inbuf[2048];
    int out_size = 2048;
    AACENC_ERROR err;
    bool active = false;
    // AACCallbackFDK aacCallback=nullptr;
    UINT encModules = 0x01; 
    UINT openEncModules = 0; 
    int openChannels = 1;
    int sce=1, cpe=0; // for bitrate determination
  };

}  // namespace aaccodec

#endif