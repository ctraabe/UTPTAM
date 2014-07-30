#include "Serial.h"

#include <iostream>

#include <fcntl.h>
#include <unistd.h>


Serial::Serial(const std::string &comport, const int baudrate)
  : id_(-1)
{
  int baudrate_code = B0;  // Hangup.
  switch(baudrate)
  {
    case 50:
      baudrate_code = B50;
      break;
    case 75:
      baudrate_code = B75;
      break;
    case 110:
      baudrate_code = B110;
      break;
    case 134:
      baudrate_code = B134;
      break;
    case 150:
      baudrate_code = B150;
      break;
    case 200:
      baudrate_code = B200;
      break;
    case 300:
      baudrate_code = B300;
      break;
    case 600:
      baudrate_code = B600;
      break;
    case 1200:
      baudrate_code = B1200;
      break;
    case 1800:
      baudrate_code = B1800;
      break;
    case 2400:
      baudrate_code = B2400;
      break;
    case 4800:
      baudrate_code = B4800;
      break;
    case 9600:
      baudrate_code = B9600;
      break;
    case 19200:
      baudrate_code = B19200;
      break;
    case 38400:
      baudrate_code = B38400;
      break;
    case 57600:
      baudrate_code = B57600;
      break;
    case 115200:
      baudrate_code = B115200;
      break;
    case 230400:
      baudrate_code = B230400;
      break;
    case 460800:
      baudrate_code = B460800;
      break;
    case 500000:
      baudrate_code = B500000;
      break;
    case 576000:
      baudrate_code = B576000;
      break;
    case 921600:
      baudrate_code = B921600;
      break;
    case 1000000:
      baudrate_code = B1000000;
      break;
    default :
      std::cerr << "Failed to open " << comport << ". Invalid baudrate."
        << std::endl;
      return;
      break;
  }

  id_ = open(comport.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (id_ == -1) {
    std::cerr << "Failed to open " << comport << "." << std::endl;
    return;
  }

  int error = tcgetattr(id_, &original_port_settings_);
  if (error == -1)
  {
    Close();
    std::cerr << "ERROR: Unable to read settings on " << comport << "."
      << std::endl;
      return;
  }

  struct termios new_port_settings = {0};
  new_port_settings.c_cflag = baudrate_code | CS8 | CLOCAL | CREAD;
  new_port_settings.c_iflag = IGNPAR;
  error = tcsetattr(id_, TCSANOW, &new_port_settings);
  if (error == -1)
  {
    Close();
    std::cerr << "ERROR: Unable to adjust settings on " << comport << "."
      << std::endl;
    return;
  }
}

int Serial::Read(unsigned char* const buffer, const int length) const
{
  if (id_ == -1)
    return -1;

  return read(id_, buffer, length);
}

int Serial::SendByte(const unsigned char byte) const
{
  return SendBuffer(&byte, 1);
}

int Serial::SendBuffer(const unsigned char* const buffer, const int length)
  const
{
  if (id_ == -1)
    return -1;

  return write(id_, buffer, length);
}

void Serial::Close()
{
  close(id_);
  tcsetattr(id_, TCSANOW, &original_port_settings_);
  id_ = -1;
}


