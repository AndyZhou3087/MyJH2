#include "Hero.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

Hero::Hero()
{
	m_state = 0;
	m_breakupper = 0;
	m_randattr = 0.0f;
	m_pos = 0;
	m_myhp = 0;
}


Hero::~Hero()
{

}

Hero::Hero(Hero* hero)
{
	m_name = hero->getName();
	m_sex = hero->getSex();
	m_vocation = hero->getVocation();
	m_potential = hero->getPotential();
	m_randattr = hero->getRandAttr();
	m_state = hero->getState();
	m_breakupper = 0;
	m_pos = 0;
	m_myhp = 0;
}

int Hero::getLevel()
{
	int size = GlobalInstance::vec_herosAttr[m_vocation].vec_exp.size();
	for (int i = 0; i < size; i++)
	{
		if (m_exp.getValue() < GlobalInstance::vec_herosAttr[m_vocation].vec_exp[i])
			return i;
	}
	return 0;
}

float Hero::getAtk()
{
	float heroatk = GlobalInstance::vec_herosAttr[m_vocation].vec_atk[getLevel()];

	return heroatk;
}
float Hero::getDf()
{
	float herodf = GlobalInstance::vec_herosAttr[m_vocation].vec_df[getLevel()];
	return herodf;
}
float Hero::getMaxHp()
{
	float herohp = GlobalInstance::vec_herosAttr[m_vocation].vec_maxhp[getLevel()];
	return herohp;
}
float Hero::getAtkSpeed()
{
	float heroatkspeed = GlobalInstance::vec_herosAttr[m_vocation].vec_atkspeed[getLevel()];
	return heroatkspeed;
}
float Hero::getCrit()
{
	float herocrit = GlobalInstance::vec_herosAttr[m_vocation].vec_crit[getLevel()];
	return herocrit;
}
float Hero::getDodge()
{
	float herododge = GlobalInstance::vec_herosAttr[m_vocation].vec_avoid[getLevel()];
	return herododge;
}

void Hero::generate()
{
	setVocation(GlobalInstance::getInstance()->createRandomNum(HEROMAX));
	setPotential(GlobalInstance::getInstance()->createRandomNum(5));

	DynamicValueInt lvdint;
	setExp(lvdint);
	setSex(GlobalInstance::getInstance()->createRandomNum(2));
	std::string nickname;
	bool iscontinue = true;
	do
	{
		nickname = generateName();
	} while (GlobalInstance::getInstance()->checkifSameName(nickname));
	setName(nickname);

	m_myhp = getMaxHp();
}

std::string Hero::generateName()
{
	int rfirst = GlobalInstance::getInstance()->createRandomNum(FIRSTNAMECOUNT);
	int rlast = GlobalInstance::getInstance()->GlobalInstance::createRandomNum(LASTNAMECOUNT);
	std::string namestr;
	std::string heronamefile[] = { "heroname/firstname.txt" , "heroname/lastname.txt" };
	int randindex[] = { rfirst ,rlast };
	for (int i = 0; i < 2; i++)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)//android apk是压缩包无法通过fopen操作Asset文件，通过cocos的文件操作，需要读取整个文件，文件可能有1w行，所有改用java文件处理
		std::string ret;
		JniMethodInfo methodInfo;
		if (JniHelper::getStaticMethodInfo(methodInfo, ANDOIRJNICLSNAME, "readAssetStringByLine", "(Ljava/lang/String;I)Ljava/lang/String;"))
		{
			jstring para1 = methodInfo.env->NewStringUTF(heronamefile[i].c_str());
			jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, para1, randindex[i]);
			ret = methodInfo.env->GetStringUTFChars(jstr, 0);
		}
		CommonFuncs::trim(ret);
		namestr += ret;
#else
		std::string fileName = FileUtils::getInstance()->fullPathForFilename(ResourcePath::makePath(heronamefile[i]));
		if (fileName.length() > 0)
		{
			FILE *fp = fopen(fileName.c_str(), "rb");
			if (fp)
			{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				fseek(fp, randindex[i] * 14, 0);//12个字节名字（4个中文），WIN32回车换行2个字节
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				fseek(fp, randindex[i] * 13, 0);//12个字节名字（4个中文），IOS回车换行1个字节
#endif
				char szReadBuff[13] = { 0 };
				size_t readsize = fread(szReadBuff, 12, 1, fp);
				if (readsize > 0)
				{
					std::string name = StringUtils::format("%s", szReadBuff);
					CommonFuncs::trim(name);
					namestr += name;
				}
				fclose(fp);
				fp = NULL;
			}

		}
#endif
	}
	return namestr;
}