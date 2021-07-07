#include "specific.h"
#include "../assoc.h"

/*insert key and data pair*/
void _assoc_insert(assoc* a, bytes* key, bytes* data);
void _insert_strings(assoc* a, bytes* key, bytes* data, u_int hashIndex);
void _insert_otherType(assoc* a, bytes* key, bytes* data, u_int hashIndex);
/*modified version of djb2 from:
http://www.cse.yorku.ca/~oz/hash.html*/
u_int _generateHashCode (assoc* a, bytes* key);
/*double hashing used as probe*/
u_int _generateHashCode2 (assoc* a, bytes* key);
/*resize when numOfKeys is 60% of capacity*/
void _resizeHashTable (assoc** a);
/*return true if number is prime*/
bool _isPrime (u_int newCapacity);
/*return pointer to data*/
bytes* _assoc_lookup(assoc* a, bytes* key);
bytes* _strings_lookup(assoc* a, bytes* key, u_int hashIndex);
bytes* _otherType_lookup(assoc* a, bytes* key, u_int hashIndex);
void test(void);

assoc* assoc_init(int keysize)
{
  static int flag = 0;

  assoc* a = (assoc*) ncalloc(sizeof(assoc), 1);
  a->hashTable = (hashNode*) ncalloc(sizeof(hashNode), INITSIZE);
  a->capacity = INITSIZE;
  a->keySize = keysize;
  a->numOfKeys = 0;

  /*to run test()*/
  if (flag == 0) {
    flag = 1;
    test();
  }
  return a;
}

void assoc_insert(assoc** a, bytes* key, bytes* data)
{
  /*resize if numOfKeys reaches 60% of capacity*/
  if ((*a)->numOfKeys >= (((*a)->capacity) * CAPACITYLIMIT)) {
    _resizeHashTable(a);
  }
    _assoc_insert(*a, key, data);
}

bytes* assoc_lookup(assoc* a, bytes* key)
{
  return _assoc_lookup(a, key);
}

u_int assoc_count(assoc* a)
{
  return a->numOfKeys;
}

void assoc_free(assoc* a)
{
  free(a->hashTable);
  free(a);
}

void _assoc_insert(assoc* a, bytes* key, bytes* data)
{
  u_int hashIndex;

  if ((a)) {
    hashIndex = _generateHashCode(a, key);
    /*if the table index is empty*/
    if (a->hashTable[hashIndex].key == NULL) {
      a->hashTable[hashIndex].key = key;
      a->hashTable[hashIndex].data = data;
      a->numOfKeys++;
    }
    else { /*if index is not null*/
      if (a->keySize == 0) { /*if it is a string*/
        _insert_strings(a, key, data, hashIndex);
      }
      else { /*if it isn't a string */
        _insert_otherType(a, key, data, hashIndex);
      }
    }
  }
}

void _insert_strings(assoc* a, bytes* key, bytes* data, u_int hashIndex)
{
  u_int secondHashIndex;
  u_int tmpHashCode;
  int i = 1;
  bool isEmpty = false;

  if(strcmp(key, a->hashTable[hashIndex].key) == 0) {
    a->hashTable[hashIndex].data = data;
  }
  else {
    /*resolve collisions*/
    tmpHashCode = _generateHashCode2(a, key);
    secondHashIndex = (hashIndex + tmpHashCode) % a->capacity;

    while (isEmpty == false) {
      secondHashIndex = (hashIndex + (i * tmpHashCode)) % a->capacity;
      i++;
      if (a->hashTable[secondHashIndex].key != NULL) {
        if(strcmp(key, a->hashTable[secondHashIndex].key) == 0) {
          a->hashTable[secondHashIndex].data = data;
          a->numOfKeys--;
          isEmpty = true;
        }
      }
      else {
        isEmpty = true;
      }
    }
    a->hashTable[secondHashIndex].key = key;
    a->hashTable[secondHashIndex].data = data;
    a->numOfKeys++;
  }
}

void _insert_otherType(assoc* a, bytes* key, bytes* data, u_int hashIndex)
{
  u_int secondHashIndex;
  u_int tmpHashCode;
  int i = 1;
  bool isEmpty = false;

  if(memcmp(key, a->hashTable[hashIndex].key, a->keySize) == 0) {
    a->hashTable[hashIndex].data = data;
  }
  else {
    /*resolve collisions*/
    tmpHashCode = _generateHashCode2(a, key);
    secondHashIndex = (hashIndex + tmpHashCode) % a->capacity;

    while (isEmpty == false) {
      secondHashIndex = (hashIndex + (i * tmpHashCode)) % a->capacity;
      i++;
      if (a->hashTable[secondHashIndex].key != NULL) {
        if(memcmp(key, a->hashTable[secondHashIndex].key, a->keySize) == 0) {
          a->hashTable[secondHashIndex].data = data;
          a->numOfKeys--;
          isEmpty = true;
        }
      }
      else {
        isEmpty = true;
      }
    }
    a->hashTable[secondHashIndex].key = key;
    a->hashTable[secondHashIndex].data = data;
    a->numOfKeys++;
  }
}

u_int _generateHashCode (assoc* a, bytes* key)
{
  seperateBytes* newKey = (seperateBytes*) key;
  int numbytes;
  int c = 0;
  u_int hashCode = UNIQUEHASH;

  if (a->keySize == 0) {
    numbytes = strlen(newKey);
  }
  else {
    numbytes = a->keySize;
  }
  while (c < numbytes) {
    hashCode = ((hashCode << ROLLHASHLEFT) + hashCode) + (*newKey++);
    c++;
  }
  return hashCode % a->capacity;
}

u_int _generateHashCode2 (assoc* a, bytes* key)
{
  seperateBytes* newKey = (seperateBytes*) key;
  int numbytes;
  int c = 0;
  u_int hashCode = 0;

  if (a->keySize == 0) {
    numbytes = strlen(newKey);
  }
  else {
    numbytes = a->keySize;
  }
  while (c < numbytes) {
    hashCode = (hashCode + (*newKey++)) * PRIMENUM;
    c++;
  }
  hashCode = hashCode % a->capacity;
  if (hashCode == 0){
    hashCode += 1;
  }
  return hashCode;
}

void _resizeHashTable (assoc** a)
{
  assoc* b = (assoc*) ncalloc(sizeof(assoc), 1);
  u_int newCapacity = (*a)->capacity * SCALEFACTOR;
  u_int i;

  while (!_isPrime(newCapacity)) {
    newCapacity++;
  }
  b->hashTable = (hashNode*) ncalloc(sizeof(hashNode), newCapacity);
  b->capacity = newCapacity;
  b->keySize = (*a)->keySize;
  b->numOfKeys = 0;
  /*rehashing*/
  for (i = 0; i < (*a)->capacity; i++) {
    if ((*a)->hashTable[i].key != NULL) {
      _assoc_insert(b, (*a)->hashTable[i].key, (*a)->hashTable[i].data);
    }
  }
  assoc_free((*a));
  (*a) = b; /*point a to b*/
}

bool _isPrime (u_int capacity)
{
  u_int i;
  u_int cnt = 0;
  u_int remainder;

  for (i = 2; i < capacity; i++) {
    remainder = capacity % i;

    if (remainder == 0){
      cnt++;
    }
  }

  if (cnt == 0){
    return true;
  }
  else {
    return false;
  }
}

bytes* _assoc_lookup(assoc* a, bytes* key)
{
  u_int hashIndex;
  bytes* tmp;

  if (a) {
    hashIndex = _generateHashCode(a, key);
    if (a->hashTable[hashIndex].key == NULL) {
      return NULL;
    }
    else {
      if (a->keySize == 0) {
        if ((tmp = _strings_lookup(a, key, hashIndex)) == NULL) {
          return NULL;
        }
        else {
          return tmp;
        }
      }
      else {
        if ((tmp = _otherType_lookup(a, key, hashIndex)) == NULL) {
          return NULL;
        }
        else {
          return tmp;
        }
      }
    }
  }
  return NULL;
}

bytes* _strings_lookup(assoc* a, bytes* key, u_int hashIndex)
{
  u_int secondHashIndex;
  u_int tmpHashCode;
  int i = 1;

  if (strcmp(a->hashTable[hashIndex].key, key)==0) {
    return a->hashTable[hashIndex].data;
  }
  else {
    tmpHashCode = _generateHashCode2(a, key);
    secondHashIndex = (hashIndex + tmpHashCode) % a->capacity;

    while (a->hashTable[secondHashIndex].key != NULL) {
      secondHashIndex = (hashIndex + (i * tmpHashCode)) % a->capacity;
      i++;

      if (a->hashTable[secondHashIndex].key != NULL) {
        if(strcmp(key, a->hashTable[secondHashIndex].key) == 0) {
          return a->hashTable[secondHashIndex].data;
        }
      }
    }
  }
  return NULL;
}

bytes* _otherType_lookup(assoc* a, bytes* key, u_int hashIndex)
{
  u_int secondHashIndex;
  u_int tmpHashCode;
  int i = 1;

  if (memcmp(a->hashTable[hashIndex].key, key, a->keySize)==0) {
    return a->hashTable[hashIndex].data;
  }
  else {
    tmpHashCode = _generateHashCode2(a, key);
    secondHashIndex = (hashIndex + tmpHashCode) % a->capacity;

    while (a->hashTable[secondHashIndex].key != NULL) {
      secondHashIndex = (hashIndex + (i * tmpHashCode)) % a->capacity;
      i++;

      if (a->hashTable[secondHashIndex].key != NULL) {
        if(memcmp(key, a->hashTable[secondHashIndex].key, a->keySize) == 0) {
          return a->hashTable[secondHashIndex].data;
        }
      }
    }
  }
  return NULL;
}

void test(void)
{
  u_int firstHashIndex;
  u_int secondHashIndex;
  int i1 = 324;
  double d1 = 124.992;

  /*initiate hashTable for strings*/
  assoc* versionStr = assoc_init(0);
  /*initiate hashTable for ints*/
  assoc* versionInt = assoc_init(sizeof(int));
  /*initiate hashTable for doubles*/
  assoc* versionDbl = assoc_init(sizeof(double));
  /*selected string, int and double due to their varying
    byte size*/

  /*testing _generateHashCode returns expected value*/
  firstHashIndex = _generateHashCode(versionStr, "car");
  assert(firstHashIndex == 5);
  firstHashIndex = _generateHashCode(versionInt, &i1);
  assert(firstHashIndex == 9);
  firstHashIndex = _generateHashCode(versionDbl, &d1);
  assert(firstHashIndex == 15);

  /*testing _generateHashCode2 returns expected value*/
  secondHashIndex = _generateHashCode2(versionStr, "car");
  assert(secondHashIndex == 3);
  secondHashIndex = _generateHashCode2(versionInt, &i1);
  assert(secondHashIndex == 16);
  secondHashIndex = _generateHashCode2(versionDbl, &d1);
  assert(secondHashIndex == 14);

  /*testing prime function returns correct bool*/
  assert(_isPrime(13));
  assert(_isPrime(7));
  assert(_isPrime(11));
  assert(!_isPrime(8));
  assert(!_isPrime(6));
  assert(!_isPrime(345));

  assoc_free(versionStr);
  assoc_free(versionInt);
  assoc_free(versionDbl);

  /*remaining functions have been tested in
    testrassoc.c*/
}
