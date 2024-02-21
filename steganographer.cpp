#include <climits>
#include <cstdlib>
#include <glib.h>
#include <random>

#include "./steganographer.hpp"

char *input_file;
char *input_message;
bool decode = false;
bool random_noise = false;
int width = 512;
int height = 512;

GOptionEntry command_line_arguments[] = {
    {"file", 'f', G_OPTION_FLAG_NONE, G_OPTION_ARG_FILENAME, &input_file,
     "The path to the image that will be used to encode a secret message", ""},
    {"message", 'm', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &input_message,
     "The secret message that will be encoded into the image", ""},
    {"decode", 'd', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &decode,
     "If omitted, a message will be encoded, or else the program will try decoding the message", ""},
    {"random_noise", 'r', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &random_noise,
     "This option will create an image with random pixels instead of using a file", ""},
    {"width", 'w', G_OPTION_FLAG_NONE, G_OPTION_ARG_INT, &width,
     "Specifies the width of the image when using -r. Defaults to 512", ""},
    {"height", 'h', G_OPTION_FLAG_NONE, G_OPTION_ARG_INT, &height,
     "Specifies the height of the image when using -r. Defaults to 512", ""},
    {nullptr},
};

int main(int argc, char *argv[]) {
  if (parse_command_line_arguments(&argc, &argv) == -1) {
    return -1;
  }

  Image image;
  if (random_noise) {
    image = generate_random_image(input_file, width, height);
  } else {
    image = Image(input_file, 0);
  }

  if (decode) {
    g_print("%s\n", image.decode_message().c_str());
  } else {
    image.encode_message(input_message, -1);
  }
}

int parse_command_line_arguments(int *argc, char **argv[]) {
  GError *error = nullptr;
  GOptionContext *context = g_option_context_new(nullptr);
  g_option_context_add_main_entries(context, command_line_arguments, nullptr);
  g_option_context_set_help_enabled(context, true);

  if (!g_option_context_parse(context, argc, argv, &error)) {
    g_error("An error occured while parsing command line arguments: %s\n", error->message);
    g_error_free(error);
  }

  g_option_context_free(context);

  if (!input_file) {
    g_print("Input file was not specified, try --help\n");
    return -1;
  }

  return 0;
}

Image generate_random_image(std::string path, int width, int height) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, INT_MAX);

  int num_channels = 3;
  unsigned char *data = static_cast<unsigned char *>(std::malloc(width * height * num_channels));

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      unsigned char *pixel = data + (i + width * j) * num_channels;
      int random = dist(rng);
      memcpy(pixel, &random, 3);
    }
  }

  return Image(data, width, height, num_channels, path);
}
