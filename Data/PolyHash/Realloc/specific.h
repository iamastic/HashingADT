#define INITSIZE 17
#define PRIMENUM 33
#define SCALEFACTOR 2
#define CAPACITYLIMIT 0.6
#define UNIQUEHASH 5381
#define ROLLHASHLEFT 5

typedef enum bool {false, true} bool;
typedef void bytes;
typedef char seperateBytes;
typedef unsigned int u_int;

typedef struct hashNode {
  bytes* key;
  bytes* data;
} hashNode;

typedef struct assoc {
  /*underlying array*/
  hashNode* hashTable;
  u_int keySize;
  u_int capacity;
  u_int numOfKeys;
} assoc;
