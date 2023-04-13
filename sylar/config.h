
#ifndef __SYLAR_CONFIG_H__
#define __SYLAR_CONFIG_H__
#include "log.h"
#include "util.h"
#include <string>
#include <stdint.h>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

namespace sylar{

class ConfigVarBase {
public:
	typedef std::shared_ptr<ConfigVarBase> ptr;

	ConfigVarBase(const std::string& name, const std::string& description = "")
       		:m_name(name)
        	,m_description(description) {

		std::transform(m_name.begin(),m_name.end(),m_name.begin(), ::tolower);
    	
	}

    /**
     * @brief 析构函数
     */
    virtual ~ConfigVarBase() {}

    /**
     * @brief 返回配置参数名称
     */
    const std::string& getName() const { return m_name;}

    /**
     * @brief 返回配置参数的描述
     */
    const std::string& getDescription() const { return m_description;}

    /**
     * @brief 转成字符串
     */
    virtual std::string toString() = 0;

    /**
     * @brief 从字符串初始化值
     */
    virtual bool fromString(const std::string& val) = 0;
private:
    std::string m_name;
    std::string m_description;
};


template<class T>
class ConfigVar : public ConfigVarBase {
public:

    typedef std::shared_ptr<ConfigVar> ptr;

    ConfigVar(const std::string& name
            ,const T& default_value
            ,const std::string& description = "")
        :ConfigVarBase(name, description)
        ,m_val(default_value) {
    }

    /**
     * @brief 将参数值转换成YAML String
     * @exception 当转换失败抛出异常
     */
    std::string toString() override {
        try {
            return boost::lexical_cast<std::string>(m_val);
            //RWMutexType::ReadLock lock(m_mutex);
            //return ToStr()(m_val);
        } catch (std::exception& e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception "
                << e.what() << " convert: " <<typeid(m_val).name() << " to string";
        }
        return "";
    }

    /**
     * @brief 从YAML String 转成参数的值
     * @exception 当转换失败抛出异常
     */
    bool fromString(const std::string& val) override {
        try {

		m_val = boost::lexical_cast<T>(val);
        } catch (std::exception& e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::fromString exception "
                << e.what() << " convert: string to " << typeid(m_val).name();
                //<< " name=" << m_name
                //<< " - " << val;
        }
        return false;
    }

    const T getValue() {
    	return m_val;
    }

    void setValue(const T& val){
    	m_val = val;
    }

private:
    T m_val;


};


class Config {
public:
	typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
	template<class T>
		static typename ConfigVar<T>::ptr Lookup(const std::string& name,
				const T& default_value, const std::string& description = "") {
	
		auto tmp = Lookup<T>(name);
		if(tmp){
			SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "LOOKUP NAME = " << name << "exists";
			return tmp;			
		}
		if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789")
				!= std::string::npos) {
		
			SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "loopup name invalid " << name;
			throw std::invalid_argument(name);
		}

		typename  ConfigVar<T>::ptr v(new ConfigVar<T>(name,default_value,description));
		s_datas[name]=v;
		return v;
	}

	template<class T>
	static typename ConfigVar<T>::ptr Lookup(const std::string& name){
		auto it = s_datas.find(name);
		if(it == s_datas.end())
		{
			return nullptr;
		}

		return std::dynamic_pointer_cast<ConfigVar<T>>( it->second);
	}

	static ConfigVarBase::ptr LookupBase(const std::string& name);
	
	static void LoadFromYaml(const YAML::Node& root);

private:
	static ConfigVarMap s_datas;

};






}


#endif


