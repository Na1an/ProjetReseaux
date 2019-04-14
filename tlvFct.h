#ifndef TLVFCT_H
#define TLVFCT

#include "include.h"

#define MSG_ENTETE 4
#define TLV_ENTETE 2

/*Message*/

/**
	* Crée un Message vide
	* @param char * msg : le Message
	* @return la taille du Message
	*/
int createMsg(char * msg);

/**
	* Prend le magic d'un Message
	* @param char * msg : le Message
	* @return le magic
	*/
uint8_t getMsg_Magic(char * msg);

/**
	* Prend la version d'un Message
	* @param char * msg : le Message
	* @return la verion
	*/
uint8_t getMsg_Version(char * msg);

/**
	* Prend la taille du corps d'un Message
	* @param char * msg : le Message
	* @return la taille
	*/
uint16_t getMsg_Body_Length(char * msg);

/**
	* Prend le Tlv souhaité d'un Message
	* @param char * msg : le Message
	* @param uint8_t len : nombre d'octet avant le Tlv souhaité après le Message
	* @return le Tlv
	*/
char * getMsg_Tlv(char * msg, uint8_t len);

/**
	* Ajoute un Tlv à un Message
	* @param char * msg : le Message
	* @param char * body : le Tlv
	* @param uint16_t len : la taille du Tlv
	* @return la taille du Message ou -1 si trop grand
	*/
int setMsg_Body(char * msg, char * body, uint16_t len);


/*TLV*/

/**
	* Prend le type d'un Tlv
	* @param char * tlv : le Tlv
	* @return le type
	*/
uint8_t getTlv_Type(char * tlv);

/**
	* Prend la taille de la valeur d'un Tlv
	* @param char * tlv : le Tlv
	* @return la taille
	*/
uint8_t getTlv_Length(char * tlv);

/**
	* Prend la valeur d'un Tlv
	* @param char * tlv : le Tlv
	* @return la valeur
	*/
char * getTlv_Body(char * tlv);


/*Pad1*/

/**
	* Crée un Pad1
	* @param char * pad : le Pad1
	* @return la taille du PadN
	*/
int createPad1(char * pad);


/*PadN*/

/**
	* Crée un PadN
	* @param char * pad : le PadN
	* @param uint8_t len : la taille de la valeur du PadN
	* @return la taille du PadN
	*/
int createPadN(char * pad, uint8_t len);


/*Hello*/

/**
	* Prend l'id source d'un Hello
	* @param char * hello : le Hello
	* @return l'id
	*/
uint64_t getHello_Source_Id(char * hello);


/*Hello_short*/

/**
	* Crée un Hello_Short
	* @param char * hello_s : le Hello_Short
	* @param uint64_t sId : l'id du Voisin émetteur
	* @return la taille du Hello_Short
	*/
int createHello_short(char * hello_s, uint64_t sId);


/*Hello_long*/

/**
	* Crée un Hello_Long
	* @param char * hello_l : le Hello_Long
	* @param uint64_t sId : l'id du Voisin émetteur du Hello_Long
	* @param uint64_t dId : l'id du Voisin destinataire du Hello_Long
	* @return la taille du Hello_Long
	*/
int createHello_long(char * hello_l, uint64_t sId, uint64_t dId);

/**
	* Prend l'id destination d'un Hello_Long
	* @param char * hello_l : le Hello_Long
	* @return l'id
	*/
uint64_t getHello_long_Destination_Id(char * hello_l);


/*Neighbour*/

/**
	* Crée un Neighbour
	* @param char * neighbour : le Neighbour
	* @param struct in6_addr ip : l'ip du Neighbour
	* @param in_port_t port: le port du Neighbour
	* @return la taille du Neighbour
	*/
int createNeighbour(char * neigbour, struct in6_addr ip, in_port_t port);

/**
	* Prend l'ip d'un Neighbour
	* @param char * neighbour : le Neighbour
	* @param struct in6_addr ip : l'ip du Neighbour
	* @return la taille de l'ip
	*/
int getNeighbour_Ip(char * neightbour, struct in6_addr * ip);

/**
	* Prend le port d'un Neighbour
	* @param char * neighbour : le Neighbour
	* @return le port
	*/
in_port_t getNeighbour_Port(char * neighbour);


/*Data*/

/**
	* Crée un Data
	* @param char * data : le Data
	* @param uint64_t sId : l'id du Voisin émetteur du Data
	* @param uint32_t nonce : le nonce du Data
	* @return la taille du Data
	*/
int createData(char * data, uint64_t sId, uint32_t nonce);

/**
	* Prend l'id de l'émetteur d'un Data
	* @param char * data : le Data
	* @return l'id
	*/
uint64_t getData_Sender_Id(char * data);

/**
	* Prend le nonce d'un Data
	* @param char * data : le Data
	* @return le nonce
	*/
uint32_t getData_Nonce(char * data);

/**
	* Prend le type d'un Data
	* @param char * data : le Data
	* @return le type
	*/
uint8_t getData_Type(char * data);

/**
	* Prend les donnees d'un Data
	* @param char * data : le Data
	* @return les donnees
	*/
char * getData_Donnees(char * data);

/**
	* Prend la taille des donnees d'un Data
	* @param char * data : le Data
	* @return la taille
	*/
int getData_Donnees_Taille(char * data);

/**
	* Ajoute des données à un Data
	* @param char * data : le Data
	* @param uint8_t t : le type du Data
	* @param char * donnees : les données
	* @param int taille : la taille des données
	* @return la taille du Data
	*/
int setData_Donnees(char * data, uint8_t t, char * donnees, int taille);


/*Ack*/

/**
	* Crée un Ack
	* @param char * ack : le Ack
	* @param uint64_t sId : l'id du Voisin émetteur du Ack
	* @param uint32_t nonce : le nonce du Ack
	* @return la taille du Ack
	*/
int createAck(char * ack, uint64_t sId, uint32_t nonce);

/**
	* Prend l'id de l'émetteur d'un Ack
	* @param char * ack : le Ack
	* @return l'id
	*/
uint64_t getAck_Sender_Id(char * ack);

/**
	* Prend le nonce d'un Ack
	* @param char * ack : le Ack
	* @return le nonce
	*/
uint32_t getAck_Nonce(char * ack);


/*GoAway*/

/**
	* Crée un GoAway
	* @param char * goAway : le GoAway
	* @param uint8_t code : le code du GoAway
	* @param char * message : le message du GoAway
	* @param int taille : la taille du message du GoAway
	* @return la taille du GoAway
	*/
int createGoAway(char * goAway, uint8_t code, char * message, int taille);

/**
	* Prend le code d'un GoAway
	* @param char * goAway : le GoAway
	* @return le code
	*/
uint8_t getGoAway_Code(char * goAway);

/**
	* Prend le message d'un GoAway
	* @param char * goAway : le GoAway
	* @return le message
	*/
char * getGoAway_Message(char * goAway);

/**
	* Prend la taille du message d'un GoAway
	* @param char * goAway : le GoAway
	* @return le la taille
	*/
int getGoAway_Message_Taille(char * goAway);


/*Warning*/

/**
	* Crée un Warning
	* @param char * warning : le Warning
	* @param char * message : le message du Warning
	* @param int taille : la taille du message du Warning
	* @return la taille du Warning
	*/
int createWarning(char * warning, char * message, int taille);

/**
	* Prend le message d'un Warning
	* @param char * warning : le Warning
	* @return le message
	*/
char * getWarning_Message(char * warning);

/**
	* Prend la taille du message d'un Warning
	* @param char * warning : le Warning
	* @return la taille
	*/
int getWarning_Message_Taille(char * warning);


/*Global*/

/**
	* Affiche correctement un Message
	* @param char * msg : le Message
	* @return 0
	*/
int printMsg(char * msg);

#endif
