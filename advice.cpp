#include "advice.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <deque>
#include <curl_easy.h>
#include <curl_header.h>
#include <sstream>
#include <list>
#include <chrono>
#include <thread>
#include <yaml-cpp/yaml.h>

advice::advice(
  std::string configFile,
  std::mt19937& rng) :
    rng_(rng)
{
  // Load the config file.
  YAML::Node config = YAML::LoadFile(configFile);

  // Set up the Twitter client.
  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());

  client_ = std::unique_ptr<twitter::client>(new twitter::client(auth));

  // Set up the verbly database.
  database_ = std::unique_ptr<verbly::database>(new verbly::database(config["verbly_datafile"].as<std::string>()));

  // Set up the sentence generator.
  generator_ = std::unique_ptr<sentence>(new sentence(*database_, rng_));

  // Read font file path.
  fontfile_ = "@" + config["font"].as<std::string>();
}

void advice::run() const
{
  for (;;)
  {
    try
    {
      verbly::filter whitelist =
        (verbly::notion::wnid == 109287968)    // Geological formations
        || (verbly::notion::wnid == 109208496) // Asterisms (collections of stars)
        || (verbly::notion::wnid == 109239740) // Celestial bodies
        || (verbly::notion::wnid == 109277686) // Exterrestrial objects (comets and meteroids)
        || (verbly::notion::wnid == 109403211) // Radiators (supposedly natural radiators but actually these are just pictures of radiators)
        || (verbly::notion::wnid == 109416076) // Rocks
        || (verbly::notion::wnid == 105442131) // Chromosomes
        || (verbly::notion::wnid == 100324978) // Tightrope walking
        || (verbly::notion::wnid == 100326094) // Rock climbing
        || (verbly::notion::wnid == 100433458) // Contact sports
        || (verbly::notion::wnid == 100433802) // Gymnastics
        || (verbly::notion::wnid == 100439826) // Track and field
        || (verbly::notion::wnid == 100440747) // Skiing
        || (verbly::notion::wnid == 100441824) // Water sport
        || (verbly::notion::wnid == 100445351) // Rowing
        || (verbly::notion::wnid == 100446980) // Archery
          // TODO: add more sports
        || (verbly::notion::wnid == 100021939) // Artifacts
        || (verbly::notion::wnid == 101471682) // Vertebrates
          ;

      verbly::filter blacklist =
        (verbly::notion::wnid == 106883725) // swastika
        || (verbly::notion::wnid == 104416901) // tetraskele
        || (verbly::notion::wnid == 102512053) // fish
        || (verbly::notion::wnid == 103575691) // instrument of execution
        || (verbly::notion::wnid == 103829563) // noose
          ;

      verbly::query<verbly::word> pictureQuery = database_->words(
        (verbly::notion::fullHypernyms %= whitelist)
        && !(verbly::notion::fullHypernyms %= blacklist)
        && (verbly::notion::partOfSpeech == verbly::part_of_speech::noun)
        && (verbly::notion::numOfImages >= 1));

      verbly::word pictured = pictureQuery.first();

      // Accept string from Google Chrome
      std::string accept = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
      curl::curl_header headers;
      headers.add(accept);

      int backoff = 0;

      std::cout << "Generating noun..." << std::endl;
      std::cout << "Noun: " << pictured.getBaseForm().getText() << std::endl;
      std::cout << "Getting URLs..." << std::endl;

      std::string lstdata;
      while (lstdata.empty())
      {
        std::ostringstream lstbuf;
        curl::curl_ios<std::ostringstream> lstios(lstbuf);
        curl::curl_easy lsthandle(lstios);
        std::string lsturl = pictured.getNotion().getImageNetUrl();
        lsthandle.add<CURLOPT_URL>(lsturl.c_str());
        lsthandle.add<CURLOPT_CONNECTTIMEOUT>(30);
        lsthandle.add<CURLOPT_TIMEOUT>(300);

        try
        {
          lsthandle.perform();
        } catch (const curl::curl_easy_exception& e)
        {
          e.print_traceback();

          backoff++;
          std::cout << "Waiting for " << backoff << " seconds..." << std::endl;

          std::this_thread::sleep_for(std::chrono::seconds(backoff));

          continue;
        }

        backoff = 0;

        if (lsthandle.get_info<CURLINFO_RESPONSE_CODE>().get() != 200)
        {
          throw could_not_get_images();
        }

        std::cout << "Got URLs." << std::endl;
        lstdata = lstbuf.str();
      }

      std::vector<std::string> lstvec = verbly::split<std::vector<std::string>>(lstdata, "\r\n");
      if (lstvec.empty())
      {
        throw could_not_get_images();
      }

      std::shuffle(std::begin(lstvec), std::end(lstvec), rng_);

      std::deque<std::string> urls;
      for (std::string& url : lstvec)
      {
        urls.push_back(url);
      }

      bool found = false;
      Magick::Blob img;
      Magick::Image pic;

      while (!found && !urls.empty())
      {
        std::string url = urls.front();
        urls.pop_front();

        std::ostringstream imgbuf;
        curl::curl_ios<std::ostringstream> imgios(imgbuf);
        curl::curl_easy imghandle(imgios);

        imghandle.add<CURLOPT_HTTPHEADER>(headers.get());
        imghandle.add<CURLOPT_URL>(url.c_str());
        imghandle.add<CURLOPT_CONNECTTIMEOUT>(30);
        imghandle.add<CURLOPT_TIMEOUT>(300);

        try
        {
          imghandle.perform();
        } catch (const curl::curl_easy_exception& error) {
          error.print_traceback();

          continue;
        }

        if (imghandle.get_info<CURLINFO_RESPONSE_CODE>().get() != 200)
        {
          continue;
        }

        std::string content_type = imghandle.get_info<CURLINFO_CONTENT_TYPE>().get();
        if (content_type.substr(0, 6) != "image/")
        {
          continue;
        }

        std::string imgstr = imgbuf.str();
        img = Magick::Blob(imgstr.c_str(), imgstr.length());

        try
        {
          pic.read(img);

          if ((pic.rows() > 0) && (pic.columns() >= 400))
          {
            std::cout << url << std::endl;
            found = true;
          }
        } catch (const Magick::ErrorOption& e)
        {
          // Occurs when the the data downloaded from the server is malformed
          std::cout << "Magick: " << e.what() << std::endl;
        }
      }

      if (!found)
      {
        throw could_not_get_images();
      }

      std::string title = generator_->generate();

      // Want a 16:9 aspect
      int idealwidth = pic.rows()*(16.0/9.0);
      if (idealwidth > pic.columns())
      {
        // If the image is narrower than the ideal width, use full width.
        int newheight = pic.columns()*(9.0/16.0);

        // Just take a slice out of the middle of the image.
        int cropy = ((double)(pic.rows() - newheight))/2.0;

        pic.crop(Magick::Geometry(pic.columns(), newheight, 0, cropy));
      } else {
        // If the image is wider than the ideal width, use full height.
        // Just take a slice out of the middle of the image.
        int cropx = ((double)(pic.columns() - idealwidth))/2.0;

        pic.crop(Magick::Geometry(idealwidth, pic.rows(), cropx, 0));
      }

      pic.zoom(Magick::Geometry(400, 225));

      // Layout the text.
      std::list<std::string> words = verbly::split<std::list<std::string>>(title, " ");
      std::vector<std::string> lines;
      std::list<std::string> cur;
      Magick::TypeMetric metric;
      pic.fontPointsize(20);
      pic.font(fontfile_);

      while (!words.empty())
      {
        cur.push_back(words.front());

        std::string prefixText = verbly::implode(std::begin(cur), std::end(cur), " ");
        pic.fontTypeMetrics(prefixText, &metric);

        if (metric.textWidth() > 380)
        {
          if (cur.size() == 1)
          {
            words.pop_front();
          } else {
            cur.pop_back();
          }

          prefixText = verbly::implode(std::begin(cur), std::end(cur), " ");
          lines.push_back(prefixText);
          cur.clear();
        } else {
          words.pop_front();
        }
      }

      if (!cur.empty())
      {
        std::string prefixText = verbly::implode(std::begin(cur), std::end(cur), " ");
        lines.push_back(prefixText);
      }

      int lineHeight = metric.textHeight()-2;
      int blockHeight = lineHeight * lines.size() + 18;
      std::cout << "line " << lineHeight << "; block " << blockHeight << std::endl;

      std::list<Magick::Drawable> drawList;
      drawList.push_back(Magick::DrawableFillColor("black"));
      drawList.push_back(Magick::DrawableFillOpacity(0.5));
      drawList.push_back(Magick::DrawableStrokeColor("transparent"));
      drawList.push_back(Magick::DrawableRectangle(0, 225-blockHeight-20, 400, 255)); // 0, 225-60, 400, 255
      pic.draw(drawList);

      drawList.clear();
      drawList.push_back(Magick::DrawableFont(fontfile_));
      drawList.push_back(Magick::DrawableFillColor("white"));
      drawList.push_back(Magick::DrawablePointSize(14));
      drawList.push_back(Magick::DrawableText(10, 225-blockHeight+4, "How to")); // 10, 255-62-4
      pic.draw(drawList);

      for (int i=0; i<lines.size(); i++)
      {
        drawList.clear();
        drawList.push_back(Magick::DrawableFont(fontfile_));
        drawList.push_back(Magick::DrawableFillColor("white"));
        drawList.push_back(Magick::DrawablePointSize(20));
        drawList.push_back(Magick::DrawableText(10, 255-blockHeight+(i*lineHeight)-4, lines[i])); // 10, 255-20-25
        pic.draw(drawList);
      }

      Magick::Blob outputimg;

      try
      {
        pic.magick("png");
        pic.write(&outputimg);
      } catch (const Magick::WarningCoder& e)
      {
        // Ignore
      }

      std::cout << "Generated image!" << std::endl << "Tweeting..." << std::endl;

      std::string tweetText = "How to " + title;
      size_t tweetLim = 140 - client_->getConfiguration().getCharactersReservedPerMedia();
      if (tweetText.length() > tweetLim)
      {
        tweetText = tweetText.substr(0, tweetLim - 1) + "…";
      }

      long media_id = client_->uploadMedia("image/png", (const char*) outputimg.data(), outputimg.length());
      client_->updateStatus(tweetText, {media_id});

      std::cout << "Tweeted!" << std::endl << "Waiting..." << std::endl;

      std::this_thread::sleep_for(std::chrono::hours(1));
    } catch (const could_not_get_images& ex)
    {
      std::cout << ex.what() << std::endl;
    } catch (const Magick::ErrorImage& ex)
    {
      std::cout << "Image error: " << ex.what() << std::endl;
    } catch (const twitter::twitter_error& ex)
    {
      std::cout << "Twitter error: " << ex.what() << std::endl;
    }
  }
}
