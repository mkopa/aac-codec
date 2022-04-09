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
    AACCodec(std::string name);

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
  private:
    aacenc_t _h;
  };

}  // namespace aaccodec

#endif