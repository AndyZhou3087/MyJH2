#ifndef _H_DYNAMICVALUE_H_
#define _H_DYNAMICVALUE_H_
 
#include "MainScene.h"
/****************************
所有数值相关，使用这个类型，防止修改内存
m_Value 数值值，内存中异或加密
m_EncryptKey 加密KEY 23 防止通过工具查找到原值
randomNum 验证随机数
verifyNum 验证值，每次使用值是否一致，不一致说明作弊
****************************/
template<class T>

class CEncryptValue
{

public:

    CEncryptValue() : m_Value(0), m_EncryptKey(23), randomNum(1987), verifyNum(0)
    {
		m_Value ^= m_EncryptKey;
		verifyNum = m_Value + randomNum;//设置验证值
    }

    ~CEncryptValue()
    {

    }

	//获取值，异或加密，两次异或值还原
    T getValue()
    {
		if (Verify())
			return m_Value ^ m_EncryptKey;
		else
		{
			setValue(0);
			MainScene::cheatAction(0);
			return m_Value ^ m_EncryptKey;
		}
    }

	//设置值，异或加密，设置验证值
    void setValue(T value)
    {
        m_Value = value;
        //m_EncryptKey = rand();
        m_Value ^= m_EncryptKey;
		verifyNum = m_Value + randomNum;//设置验证值
    }
	//改变值，正数增加，负数减少
    void offset(T value)
    {
        setValue(getValue() + value);
    }

	//验证数据
	bool Verify()
	{
		//0.1f--float比较，会有float类型
		if (m_Value + randomNum >= verifyNum - 0.1 && m_Value + randomNum <= verifyNum + 0.1)
			return true;

		return false;
		//**非法修改值 可以在此告诉服务器或者是退出程序**        
	}
public:

    T m_Value;

    int m_EncryptKey;
	int randomNum;
	T verifyNum;
};

 
//int type
typedef CEncryptValue<int> DynamicValueInt;

//float type
typedef CEncryptValue<float> DynamicValueFloat;

#endif