#include "JsonParser.h"

USING_NS_CC;

JsonParser *JsonParser::createWithString(const char* data)
{
	JsonParser *pRet = new JsonParser();
	if (pRet->initWithString(data))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return nullptr;
}
bool JsonParser::initWithString(const char* data)
{
	content = data;
	return true;
}

JsonParser* JsonParser::createWithFile(const char *_fileName)
{
	JsonParser *pRet = new JsonParser();
	if (pRet->initWithFile(_fileName))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return nullptr;
}

bool JsonParser::initWithFile(const char *_fileName)
{
	std::string path = FileUtils::getInstance()->getWritablePath() + _fileName;
	if (FileUtils::getInstance()->isFileExist(path) == false)
	{
		path = FileUtils::getInstance()->fullPathForFilename(_fileName);
	}
	Data data = FileUtils::getInstance()->getDataFromFile(path);
	content = std::string((const char*)data.getBytes(), 0, data.getSize());
	return true;
}

JsonParser* JsonParser::createWithArray(ValueVector _listData)
{
	JsonParser *pRet = new JsonParser();
	if (pRet->initWithArray(_listData))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return nullptr;
}

bool JsonParser::initWithArray(ValueVector _listData)
{
	this->listData = _listData;
	return true;
}

void JsonParser::decodeDebugData()
{
	rapidjson::Document document;
	document.Parse<0>(content.c_str());

	CCASSERT(!document.HasParseError(), "Parsing to document failure.");

	CCLOG("Parsing to document succeeded.");

	CC_ASSERT(document.IsObject() && document.HasMember("Data"));

	const rapidjson::Value& datas = document["Data"];

	CC_ASSERT(datas.IsArray());

	for (unsigned int i = 0; i < datas.Size(); ++i)
	{
		row = ValueMap();

		const rapidjson::Value& data = datas[i];

		CC_ASSERT(data.HasMember("Attr"));

		const rapidjson::Value& val_attr = data["Attr"];

		CC_ASSERT(val_attr.IsString());

		//row["attr"] = Value(val_attr.GetString());

		CC_ASSERT(data.HasMember("Value"));

		const rapidjson::Value& val_Value = data["Value"];

		CC_ASSERT(val_Value.IsDouble());

		//row["value"] = Value(val_Value.GetDouble());
		row[val_attr.GetString()] = Value(val_Value.GetDouble());

		list.push_back(Value(row));
	}
}

void JsonParser::encodeDebugData(const char *fileName)
{
	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value array(rapidjson::kArrayType);

	for (auto& v : listData)
	{
		ValueMap row = v.asValueMap();
		rapidjson::Value object(rapidjson::kObjectType);
		rapidjson::Value v_attr;
		v_attr.SetString(row["Attr"].asString().c_str(), allocator);
		object.AddMember("Attr", v_attr, allocator);

		rapidjson::Value v_Value;
		v_Value.SetDouble(row["Value"].asFloat());
		object.AddMember("Value", v_Value, allocator);

		array.PushBack(object, allocator);
	}

	document.AddMember("Data", array, allocator);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);

	auto out = buffer.GetString();
	CCLOG("encode %s", out);
	
	
	FILE *fp = fopen(fileName, "wb");
	if (fp)
	{
		size_t ret = fwrite(out, 1, strlen(out), fp);
		fclose(fp);
	}
}

void JsonParser::decodeLoginProtocol()
{
	rapidjson::Document document;
	document.Parse<0>(content.c_str());

	CCASSERT(!document.HasParseError(), "Parsing to document failure.");

	CCLOG("Parsing to document succeeded.");

	CC_ASSERT(document.IsObject() && document.HasMember("Data"));

	const rapidjson::Value& datas = document["Data"];

	CC_ASSERT(datas.IsArray());

	for (unsigned int i = 0; i < datas.Size(); ++i)
	{
		row = ValueMap();

		const rapidjson::Value& data = datas[i];

		CC_ASSERT(data.HasMember("player_id"));

		const rapidjson::Value& val_attr = data["player_id"];

		CC_ASSERT(val_attr.IsString());

		//row["attr"] = Value(val_attr.GetString());

		CC_ASSERT(data.HasMember("role_id"));

		const rapidjson::Value& val_Value = data["role_id"];

		CC_ASSERT(val_Value.IsInt());

		//row["value"] = Value(val_Value.GetDouble());
		row[val_attr.GetString()] = Value(val_Value.GetInt());

		list.push_back(Value(row));
	}
}