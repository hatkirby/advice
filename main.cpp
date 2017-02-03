#include "advice.h"

int main(int argc, char** argv)
{
  Magick::InitializeMagick(nullptr);

  std::random_device random_device;
  std::mt19937 random_engine{random_device()};

  if (argc != 2)
  {
    std::cout << "usage: advice [configfile]" << std::endl;
    return -1;
  }

  std::string configfile(argv[1]);

  try
  {
    advice bot(configfile, random_engine);

    try
    {
      bot.run();
    } catch (const std::exception& ex)
    {
      std::cout << "Error running bot: " << ex.what() << std::endl;
    }
  } catch (const std::exception& ex)
  {
    std::cout << "Error initializing bot: " << ex.what() << std::endl;
  }
}
