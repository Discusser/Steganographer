#pragma once

#include <memory>
#include <string>

class Image {
 public:
  Image() {}
  Image(unsigned char *data, int width, int height, int num_channels, const std::string &path);
  Image(const std::string &path, int desired_num_channels);

  void encode_message(const std::string &message, int stride);
  std::string decode_message();

  const unsigned char *get_data();
  const std::string &get_path();
  int get_width();
  int get_height();
  int get_num_channels();

 private:
  std::shared_ptr<unsigned char> data;
  std::string path;
  int width;
  int height;
  int num_channels;
};
