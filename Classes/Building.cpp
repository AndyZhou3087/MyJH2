#include "Building.h"
#include "DataSave.h"
#include "Resource.h"
#include "CommonFuncs.h"

std::map<std::string, Building*> Building::map_buildingDatas;
Building::Building()
{
	
}


Building::~Building()
{
	destroy();
}

void Building::destroy()
{
	std::map<std::string, Building*>::iterator it;
	for (it = Building::map_buildingDatas.begin(); it != Building::map_buildingDatas.end(); it++)
	{
		delete map_buildingDatas[it->first];
	}
	Building::map_buildingDatas.clear();
}

void Building::parseData()
{
	destroy();
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/buildings.json"));
	rapidjson::Value& allData = doc["b"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			Building* data = new Building;
			rapidjson::Value& value = jsonvalue["name"];
			data->name = value.GetString();

			data->level.setValue(DataSave::getInstance()->getBuildLv(data->name));
			value = jsonvalue["maxlevel"];
			data->maxlevel.setValue(atoi(value.GetString()));
			if (jsonvalue.HasMember("lvres"))
			{
				value = jsonvalue["lvres"];
				int maxlv = data->maxlevel.getValue();
				for (int i = 0; i < maxlv-1; i++)
				{
					std::vector<std::map<std::string, int>> res;
					rapidjson::Value& resvalue = value[i];
					int size = resvalue.Size();
					for (int j = 0; j < size; j++)
					{
						std::string onestr = resvalue[j].GetString();
						if (onestr.length() > 0)
						{
							std::vector<std::string> onevec;
							CommonFuncs::split(onestr, onevec, "-");
							if (onevec.size() >= 2)
							{
								std::map<std::string, int> onemap;
								onemap[onevec[0]] = atoi(onevec[1].c_str());
								res.push_back(onemap);
							}
						}
					}
					data->lvupres.push_back(res);
				}
			}
			map_buildingDatas[data->name] = data;
		}
	}
}