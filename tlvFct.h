#ifndef TLVFCT_H
#define TLVFCT

#include <inttypes.h>
#include <string.h>
#define BUF_SIZE 4096

/*Message*/

int createMsg(char * msg);

int setMsg_Body(char * msg, char * body, uint16_t len);

uint8_t getMsg_Magic(char * msg);

uint8_t getMsg_Version(char * msg);

uint16_t getMsg_Body_Length(char * msg);

char * getMsg_Tlv(char * msg);


/*TLV*/

uint8_t getTlv_Type(char * tlv);

uint8_t getTlv_Length(char * tlv);

char * getTlv_Body(char * tlv);


/*Hello*/

uint64_t getHello_Source_Id(char * hello);


/*Hello_short*/

int createHello_short(char * hello_s, uint64_t sId);

/*Hello_long*/

int createHello_long(char * hello_l, uint64_t sId, uint64_t dId);

uint64_t getHello_long_Destination_Id(char * hello_l);


#endif
