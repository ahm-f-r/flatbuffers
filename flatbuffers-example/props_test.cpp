#include <iostream>
#include <cstdint>
#include <string>
#include <cassert>
#include <vector>
#include <utility>
#include <map>
#include <memory>
#include <optional>
#include <variant>
#include <fstream>

#include "props_generated.h"

using namespace std;
using namespace PropsFB;
using namespace flatbuffers;

int main () {
    
  auto builder = FlatBufferBuilder(1024);

  Offset<String> p_name  = builder.CreateString("Age");
  Offset<String> p_scope = builder.CreateString("Mother.Child");
  Offset<String> p_key   = builder.CreateString("Age0");
  Offset<String> p_value = builder.CreateString("56");
  
  Offset<PropDict> p_dict =
    CreatePropDict(
      builder, 
      p_name, 
      p_scope,
      p_key,
      p_value);

  Offset<PropsRow> p_row = CreatePropsRow(
    builder,
    PropType_UInt64,
    607320,
    0,
    p_dict);

  // Create a std::vector of the offsets we had previous made.
  vector<Offset<PropsRow> > p_row_vector;
  p_row_vector.push_back(p_row);

  // Then serialize that std::vector into the buffer and again get an Offset
  // to that vector. Use `auto` here since the full type is long, and it just
  // a "typed" number.
  auto p_row_v = builder.CreateVector(p_row_vector);

  Offset<PropsPool> p_table = CreatePropsPool(
    builder,
    p_row_v);

  builder.Finish(p_table);

  // Write to Binary File

	uint8_t * buf = builder.GetBufferPointer();
	unsigned int size = builder.GetSize();

	FILE* file = fopen("data.bin", "wb");
	if(!file) perror("write: can't open file out");

	fwrite(&size, sizeof(size), 1, file);
	fwrite(buf, size, 1, file);
	fclose(file);

	cout << "Success" << endl;

  // Read from Binary File
	//FILE* 
  file = fopen("data.bin", "rb");
	if(!file) perror("can't open out file");

	//unsigned int size = 0;
	fread(&size, sizeof(size), 1, file);

	char *buffer = new char[size];
	fread(buffer, size, 1, file);

  auto p_pool = GetPropsPool(buffer);

  auto props = p_pool->props();
  auto prop = props->Get(0);

  cout << "Uid:       " << prop->prop_uid() << endl;
  cout << "Type:      " << EnumNamePropType(prop->prop_type()) << endl;
  cout << "ParentUid: " << prop->parent_uid() << endl;
  cout << "Name:      " << prop->key_value()->name()->str() << endl;
  cout << "Scope:     " << prop->key_value()->scope()->str() << endl;
  cout << "Key:       " << prop->key_value()->key()->str() << endl;
  cout << "Value:     " << prop->key_value()->value()->str() << endl;

	if(buffer) delete [] buffer;
	fclose(file);

  return 0;
}