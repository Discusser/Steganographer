#include <glib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <arpa/inet.h>

#include "./image.hpp"

#define HEADER_SIZE 16

Image::Image(const std::string &path, int desired_num_channels) {
  this->data =
      std::shared_ptr<unsigned char>(stbi_load(path.data(), &this->width, &this->height, &this->num_channels, 0));
  this->path = path;
}

const std::string &Image::get_path() {
  return this->path;
}

int Image::get_width() {
  return this->width;
}

int Image::get_height() {
  return this->height;
}

int Image::get_num_channels() {
  return this->num_channels;
}

const unsigned char *Image::get_data() {
  return this->data.get();
}

void Image::encode_message(const std::string &message, int stride) {
  int write_space = this->height * this->width * this->num_channels - HEADER_SIZE;

  if (stride == -1 || static_cast<int>(message.size() * stride) > write_space) {
    // Ensure message.size() * stride < write_space
    stride = std::ceil(static_cast<double>(write_space) / message.size()) - 1;
  }

  // Write header
  int message_size = static_cast<int>(message.size());
  int message_size_bytes = message_size;
  int stride_bytes = stride;
  if (htonl(47) == 47) {
    // Convert everything to little endian if big endian system, to ensure that same message can be decoded on different
    // machines
    message_size_bytes = __builtin_bswap32(message_size_bytes);
    stride_bytes = __builtin_bswap32(stride_bytes);
  }
  memcpy(this->data.get(), &message_size_bytes, 4);
  memcpy(this->data.get() + 12, &stride_bytes, 4);

  for (size_t i = 0; i < message.size(); i++) {
    *(this->data.get() + i * stride + HEADER_SIZE) = message[i];
  }

  stbi_write_png(this->path.append("encoded.png").c_str(), this->width, this->height, this->num_channels,
                 this->data.get(), this->width * this->num_channels);
}

std::string Image::decode_message() {
  int message_size = *(reinterpret_cast<int *>(this->data.get()));
  int stride = *(reinterpret_cast<int *>(this->data.get() + 12));

  if (htonl(47) == 47) {
    // If big endian, convert little endian bytes to big endian
    message_size = __builtin_bswap32(message_size);
    stride = __builtin_bswap32(stride);
  }

  std::string message;

  for (int i = 0; i < message_size; i++) {
    message.push_back(*(this->data.get() + i * stride + HEADER_SIZE));
  }

  return message;
}

Image::Image(unsigned char *data, int width, int height, int num_channels, const std::string &path) {
  this->data = std::shared_ptr<unsigned char>(data);
  this->width = width;
  this->height = height;
  this->num_channels = num_channels;
  this->path = path;
}
