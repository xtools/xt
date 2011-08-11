#include "xt/core/standard.h"
#include "xt/core/tools.h"
#include "xt/net/http/tools.h"

static char decode_uri_hex(char *hex_digits);

char decode_uri_hex(char *hex_digits)
{
  assert(hex_digits);
  char c;

  switch (*hex_digits) {
    case '2':
      hex_digits++;
      switch (*hex_digits) {
        case '0':
          c = ' ';
          break;
        case '1':
          c = '!';
          break;
        case '2':
          c = '"';
          break;
        case '3':
          c = '#';
          break;
        case '4':
          c = '$';
          break;
        case '5':
          c = '%';
          break;
        case '6':
          c = '&';
          break;
        case '7':
          c = '\'';
          break;
        case '8':
          c = '(';
          break;
        case '9':
          c = ')';
          break;
        case 'A':
          c = '*';
          break;
        case 'B':
          c = '+';
          break;
        case 'C':
          c = ',';
          break;
        case 'D':
          c = '-';
          break;
        case 'E':
          c = '.';
          break;
        case 'F':
          c = '/';
          break;
        default:
          c = '?';
      }
      break;
    case '3':
      hex_digits++;
      switch (*hex_digits) {
        case 'A':
          c = ':';
          break;
        case 'B':
          c = ';';
          break;
        case 'C':
          c = '<';
          break;
        case 'D':
          c = '=';
          break;
        case 'E':
          c = '>';
          break;
        case 'F':
          c = '?';
          break;
        default:
          c = '?';
      }
      break;
    case '4':
      hex_digits++;
      switch (*hex_digits) {
        case '0':
          c = '@';
          break;
        default:
          c = '?';
      }
      break;
    case '5':
      hex_digits++;
      switch (*hex_digits) {
        case 'B':
          c = '[';
          break;
        case 'C':
          c = '\\';
          break;
        case 'D':
          c = ']';
          break;
        case 'E':
          c = '^';
          break;
        case 'F':
          c = '_';
          break;
        default:
          c = '?';
      }
      break;
    case '6':
      hex_digits++;
      switch (*hex_digits) {
        case '0':
          c = '`';
          break;
        default:
          c = '?';
      }
      break;
    case '7':
      hex_digits++;
      switch (*hex_digits) {
        case 'B':
          c = '{';
          break;
        case 'C':
          c = '|';
          break;
        case 'D':
          c = '}';
          break;
        case 'E':
          c = '~';
          break;
        default:
          c = '?';
      }
      break;
    default:
      c = '?';
  }

  return c;
}

char *xt_net_http_decode_uri(char *uri, unsigned short uri_length)
{
  assert(uri);
  char *decoded_uri;
  char *decoded_uri_pos;

  decoded_uri = malloc(uri_length + 1);
  if (decoded_uri) {
    decoded_uri_pos = decoded_uri;
    while (*uri) {
      if ('%' == *uri) {
        uri++;
        *decoded_uri_pos = decode_uri_hex(uri);
        uri += 2;
      } else if ('+' == *uri) {
        *decoded_uri_pos = ' ';
        uri++;
      } else {
        *decoded_uri_pos = *uri;
        uri++;
      }
      decoded_uri_pos++;
    }
    *decoded_uri_pos = '\0';
  } else {
    xt_core_trace("malloc");
  }

  return decoded_uri;
}
