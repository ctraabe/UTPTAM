#ifndef SERIAL_H_
#define SERIAL_H_

#include <string>

#include <termios.h>

class Serial
{
public:
  Serial() : id_(-1) {}
  Serial(const std::string &comport, const int baudrate);

  operator bool() const { return id_ != -1; }

  int Read(unsigned char* const buffer, const int length) const;
  int SendByte(const unsigned char byte) const;
  int SendBuffer(const unsigned char* const buffer, const int length) const;
  void Close();

private:
  int id_;
  struct termios original_port_settings_;
};

#endif // SERIAL_H_