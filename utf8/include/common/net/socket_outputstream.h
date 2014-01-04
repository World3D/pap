#ifndef PAP_COMMON_NET_SOCKET_OUTPUTSTREAM_H_
#define PAP_COMMON_NET_SOCKET_OUTPUTSTREAM_H_

#include "common/net/config.h"
#include "common/lib/vnet/vnet.hpp"
#include "common/net/socket.h"
#include "common/net/packet.h"

namespace pap_common_net {

class SocketOutputStream {

 public:
   SocketOutputStream(
       Socket* socket, 
       uint32_t bufferlength = SOCKETOUTPUT_BUFFERSIZE_DEFAULT,
       uint32_t bufferlength_max = SOCKETOUTPUT_DISCONNECT_MAXSIZE);
   ~SocketOutputStream();

 public:
   uint32_t write(const char* buffer, uint32_t length);
   uint32_t writepacket(const Packet* packet);
   uint32_t flush();
   void init();
   bool resize(int32_t size);
   uint32_t reallength();
   bool isempty();
   void cleanup();
   void setkey(unsigned char const* key);
   int32_t get_keylength();
   void getbuffer(char* buffer, uint32_t length);

 private:
   Socket* socket_;
   struct packet_t* packet_;
   struct endecode_param_t* endecode_param_;

};

}; //namespace pap_common_net


#endif //PAP_COMMON_NET_SOCKET_OUTPUTSTREAM_H_