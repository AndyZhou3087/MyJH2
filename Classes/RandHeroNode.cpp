#include "RandHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "HeroAttrLayer.h"

#define FIRSTNAMECOUNT 40
#define LASTNAMECOUNT 40
Color3B POTENTIALCOLOR[5] = { Color3B(36, 255, 82), Color3B(52, 179, 240), Color3B(252, 77, 226), Color3B(255, 214, 71), Color3B(255, 69, 69)};
RandHeroNode::RandHeroNode()
{

}


RandHeroNode::~RandHeroNode()
{
	delete heroData;
	heroData = NULL;
}

bool RandHeroNode::init()
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("randHeroNode.csb"));
	this->addChild(csbnode);
	cocos2d::ui::Widget* bg = (cocos2d::ui::Widget*)csbnode->getChildByName("randheadbox");
	bg->addTouchEventListener(CC_CALLBACK_2(RandHeroNode::onClick, this));

	headbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("hbox");
	
	headimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("headimg");
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	vocationtextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("vocationtext");
	vocationtextlbl->setString(ResourceLang::map_lang["vocationtext"]);

	vocationlbl = (cocos2d::ui::Text*)csbnode->getChildByName("vocation");

	potentialtextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("potentialtext");
	potentialtextlbl->setString(ResourceLang::map_lang["potentialtext"]);

	potentiallbl = (cocos2d::ui::Text*)csbnode->getChildByName("potential");

	generate();
	return true;
}

void RandHeroNode::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Layer* layer = HeroAttrLayer::create(NEWHERO, heroData);
		this->getParent()->addChild(layer);
	}
}

void RandHeroNode::generate()
{
	heroData = new Hero();
	heroData->setVocation(rand() % HEROMAX);
	heroData->setPotential(rand() % 5);

	DynamicValueInt lvdint;
	heroData->setExp(lvdint);
	heroData->setSex(rand() % 2);
	heroData->setName(generateName());

	std::string str = StringUtils::format("ui/h_%d_%d.png", heroData->getVocation(), heroData->getSex());
	headimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("ui/herobox_%d.png", heroData->getPotential());
	headbox->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("vocation_%d", heroData->getVocation());
	vocationlbl->setString(ResourceLang::map_lang[str]);

	str = StringUtils::format("potential_%d", heroData->getPotential());
	potentiallbl->setString(ResourceLang::map_lang[str]);

	potentialtextlbl->setColor(Color3B(POTENTIALCOLOR[heroData->getPotential()]));
	potentiallbl->setColor(Color3B(POTENTIALCOLOR[heroData->getPotential()]));

	namelbl->setString(heroData->getName());
}

std::string RandHeroNode::generateName()
{
	int rfirst = rand() % FIRSTNAMECOUNT;
	int rlast = rand() % LASTNAMECOUNT;
	std::string namestr;
	std::string heronamefile[] = { "heroname/firstname.txt" , "heroname/lastname.txt" };
	int randindex[] = { rand() % FIRSTNAMECOUNT , rand() % FIRSTNAMECOUNT };
	for (int i = 0; i < 2; i++)
	{
		std::string fileName = FileUtils::getInstance()->fullPathForFilename(ResourcePath::makePath(heronamefile[i]));

		if (fileName.length() > 0)
		{
			FILE *fp = fopen(fileName.c_str(), "r");
			if (fp)
			{
				fseek(fp, randindex[i] * 14, 0);
				char szReadBuff[13] = { 0 };
				size_t readsize = fread(szReadBuff, 12, 1, fp);
				if (readsize > 0)
				{
					std::string name = StringUtils::format("%s", szReadBuff);
					CommonFuncs::trim(name);
					namestr += name;
				}

			}
		}
	}
	return namestr;
}