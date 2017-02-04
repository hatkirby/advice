#ifndef ADVICE_H_5934AC1B
#define ADVICE_H_5934AC1B

#include <random>
#include <twitter.h>
#include <verbly.h>
#include <string>
#include <memory>
#include <Magick++.h>
#include <stdexcept>
#include "sentence.h"

class advice {
public:

  advice(
    std::string configFile,
    std::mt19937& rng);

  void run() const;

private:

  class could_not_get_images : public std::runtime_error {
  public:

    could_not_get_images() : std::runtime_error("Could not get images for noun")
    {
    }
  };

  std::mt19937& rng_;
  std::unique_ptr<verbly::database> database_;
  std::unique_ptr<sentence> generator_;
  std::unique_ptr<twitter::client> client_;
  std::string fontfile_;
};

#endif /* end of include guard: ADVICE_H_5934AC1B */
