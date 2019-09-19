/*
 * ShadeNBT.h
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_NBT_H_
#define INCLUDE_NBT_H_

#ifdef __cplusplus
extern"C"{
#endif

#include <stdint.h>

#include <tigame/ComTypes.h>

typedef int8_t NBTTagType;

typedef const struct NBTTagCompound* NBTTagCompound;
typedef const struct NBTTagList* NBTTagList;
typedef const struct NBTTagEnd* NBTTagEnd;
struct NBTTagPrimitive;
typedef const struct NBTTagPrimitive* NBTTagByte;
typedef const struct NBTTagPrimitive* NBTTagShort;
typedef const struct NBTTagPrimitive* NBTTagInt;
typedef const struct NBTTagPrimitive* NBTTagLong;
typedef const struct NBTTagPrimitive* NBTTagFloat;
typedef const struct NBTTagPrimitive* NBTTagDouble;
struct NBTTagPrimitiveArray;
typedef const struct NBTTagPrimitiveArray* NBTTagByteArray;
typedef const struct NBTTagPrimitiveArray* NBTTagIntArray;
typedef const struct NBTTagPrimitiveArray* NBTTagLongArray;
typedef const struct NBTTagPrimitiveArray* NBTTagFloatArray;
typedef const struct NBTTagPrimitiveArray* NBTTagDoubleArray;
typedef const struct NBTTagString* NBTTagString;
typedef const struct NBTTagUUID* NBTTagUUID;
typedef const struct NBTTag* NBTTag;

enum{
	NBT_TAG_End = 0, NBT_TAG_Byte = 1,
	NBT_TAG_Short = 2, NBT_TAG_Int = 3,
	NBT_TAG_Long = 4, NBT_TAG_Float = 5,
	NBT_TAG_Double = 6, NBT_TAG_String = 7,
	NBT_TAG_ByteArray = 8, NBT_TAG_List = 9,
	NBT_TAG_Compound = 10, NBT_TAG_IntArray = 11,
	NBT_TAG_LongArray = 12, NBT_TAG_FloatArray = 13,
	NBT_TAG_DoubleArray = 14, NBT_TAG_UUID = 15,
	NBT_TAG_AnyNumeric = 99
};



struct NBTTagCompound{
	NBTTag* (*asNBTTag)(NBTTagCompound*);
	NBTTag* (*put)(NBTTagCompound*,const char*,NBTTag*);
	NBTTag* (*get)(NBTTagCompound*,const char*);
	tigame_bool (*hasTag)(NBTTagCompound*,const char*);
	tigame_bool (*hasTagWithType)(NBTTagCompound*,const char*,NBTTagType);

};




#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_NBT_H_ */
