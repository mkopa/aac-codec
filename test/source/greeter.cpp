#include <doctest/doctest.h>
#include <AACCodec/AACCodec.h>
#include <AACCodec/version.h>

#include <string>

TEST_CASE("AACCodec") {
  using namespace aaccodec;

  AACCodec codec("Tests");

  CHECK(codec.greet(LanguageCode::EN) == "Hello, Tests!");
  CHECK(codec.greet(LanguageCode::DE) == "Hallo Tests!");
  CHECK(codec.greet(LanguageCode::ES) == "¡Hola Tests!");
  CHECK(codec.greet(LanguageCode::FR) == "Bonjour Tests!");
  
}

TEST_CASE("AACCodec version") {
  static_assert(std::string_view(AACCODEC_VERSION) == std::string_view("1.0"));
  CHECK(std::string(AACCODEC_VERSION) == std::string("1.0"));
}
