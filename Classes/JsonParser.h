#ifndef _SPACEX_CLASSES_JSONPARSER_H_
#define _SPACEX_CLASSES_JSONPARSER_H_

#include <cocos2d.h>
#include "document.h"
#include "writer.h"
#include "reader.h"
#include "stringbuffer.h"


class JsonParser : public cocos2d::Ref
{
public:
	static JsonParser *createWithFile(const char *);
	bool initWithFile(const char *);

	static JsonParser *createWithArray(cocos2d::ValueVector);	
	bool initWithArray(cocos2d::ValueVector);
	void decodeDebugData();
	void encodeDebugData();


	CC_SYNTHESIZE(cocos2d::ValueVector, list, List);

private:
	cocos2d::ValueMap row;
	cocos2d::ValueVector listData;
	std::string content;

};

#endif
