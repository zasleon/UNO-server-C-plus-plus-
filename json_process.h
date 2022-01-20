#ifndef INCLUDE_json_process_H
#define INCLUDE_json_process_H


#include "json/json.h"
#include "json/reader.h"
#include "json/writer.h"
#include "json/value.h"
#include "server_property.h"//我自己的其他函数

//#pragma comment(lib,"lib_json_d.lib")//链接需要的库文件

/*===========================test================================
可将以下代码放入main函数中测试

	//将json格式字符串传入创建的json结构体
	Json::Value json_struct;//裸创建一个json对象
	Json::FastWriter writer;//裸建一个json写入string的接口
	json_struct["test1"]="sss";//裸赋予json对象属性值
	string kk=writer.write(json_struct);//裸将json对象转化为string字符串
	char StrBuf[1000]={0};
	strcpy(StrBuf,kk.c_str());//string转化为char
	
	
	JSON_package json_msg(StrBuf);//创建json结构体，将上面那个char作为初始创建参入传入
	json_msg.add_item("blood","10");//赋予
	json_msg.add_item("攻击力",8);
	json_msg.add_item("攻击力",17);//赋予攻击力属性，值为17，会替代上面那个8
	cout<<"为一个json结构体添加两个属性数据，其原有属性test1\n"<<json_msg.to_StrBuf()<<endl;//将json结构体具象化为字符串传出，并显示


	//创建两个json结构体，将其中一个放入另一个
	JSON_package json_msg1;//创建json结构体，存放人物基本数据
	cout<<"空数据json结构体内容:"<<json_msg1.to_StrBuf()<<endl;//如果没处理过其内容且想将该结构体转化为string输出时会提示没处理
	json_msg1.add_item("name","虹马玉");
	json_msg1.add_item("blood","102.23456");//只能储存int整型，float和double请用string代替
	json_msg1.add_item("attack",228);

	JSON_package effect_all;//创建json结构体，存放人物效果详细参数
	JSON_package effect;//单个效果
	effect.add_item("effect","2002");
	effect.add_item("effect_p1","1");
	effect.add_item("effect_p2","2");
	effect.add_item("effect_p3","5");
	effect_all.add_struct_array(effect.get_json());
	effect.add_item("effect","2052");
	effect.add_item("effect_p1","3");
	effect.add_item("effect_p2","1");
	effect.add_item("effect_p3","6");
	effect_all.add_struct_array(effect.get_json());
	effect.add_item("effect","2231");
	effect.add_item("effect_p1","23");
	effect.add_item("effect_p2","31");
	effect.add_item("effect_p3","622");
	effect.add_item("effect_p4","6");//之前几个数据没有此项，如果用数组方式查看，则之前数据为null
	effect_all.add_struct_array(effect.get_json());

	cout<<"对数组取值：\n";
	for(int i=0;i<3;i++)//显示数组内所有名为effect的值
		cout<<effect_all.get_json()[i]["effect"];//数组那个i不能赋常值，只能赋值变量

	
	json_msg1.add_struct("效果总览",effect_all.get_json());//将“效果”结构体添加到“人物基本”数据结构体
	cout<<"添加数据的最终效果:\n"<<json_msg1.to_StrBuf()<<endl;
	

	cout<<"获取attack属性内字符串："<<json_msg1.get_value("attack")<<endl;

	JSON_package json_msg2(json_msg1.get_json("效果总览"));//提取结构体中结构体并放入新创建的结构体中
	cout<<"提取结构体中的结构体:\n"<<json_msg2.to_StrBuf()<<endl;
	cout<<"对提取的结构体数组取值：\n";
	for(int i=0;i<3;i++)//显示数组内所有名为effect的值
		cout<<json_msg2.get_json()[i]["effect_p4"];//数组那个i不能赋常值，只能赋值变量


===========================test================================*/


class JSON_package
{
	private:
		string json_string;//保存json数据结构体转变为字符串后的内容
		char* json_string_point;
		char* json_value;//存储想提取的变量的字符串
		Json::Value json_struct;//保存抽象的json数据结构体


		bool whether_process;//是否对json结构体处理过,初始无目标建立时为false，经过添加数据操作后变为true
		bool whether_normal;//是否正常，不正常不处理
	public:

		void ini();//初始化常规操作

		JSON_package();//创建一个无内容JSON_package对象
		JSON_package(char* StrBuf);//创建一个JSON_package结构体，接受消息字符串内容并解析
		JSON_package(Json::Value other_json_struct);//根据一个获取的裸json结构体创建json结构体

		void add_item(string name,string value);//添加属性与值到结构体中
		void add_item(string name,int value);//添加属性与值到结构体中
		void add_struct(string name,Json::Value other_struct);//添加别的结构体，结构体名字为name

		//add_item是在同一个{ }内增加"属性"="值"，互相间用逗号隔开
		//而append是在后面多加个{}，可以将前后两个{ }视为同一个数组内的两个不同成员

		void add_item_array(string name,string value);//将某一单值以数组数据方式存入属性里
		void add_item_array(string name,int value);
		void add_item_array(string array_name,string name,string value);//将某一单值以数组数据方式存入array_name属性里
		void add_item_array(string array_name,string name,int value);//将某一单值以数组数据方式存入array_name属性里
		void add_struct_array(Json::Value other_struct);
		void add_struct_array(string array_name,Json::Value other_struct);//将某一结构体以数组数据方式存入array_name属性里

		Json::Value get_json();//获取整个json结构体
		Json::Value get_json(string name);//获取该结构体中的名字为name的结构体，获取裸结构体（开包找到name的结构体后可以用JSON_package(Json::Value other_json_struct)构造函数继续解析该结构体）

		const char* to_StrBuf();//将json_string中内容转化为char*字符串后传出
		char* get_value(string name);//获取属性名字为name的值

};

void JSON_package::add_struct_array(Json::Value other_struct)
{
	json_struct.append(other_struct);
	whether_process=true;
}

void JSON_package::add_struct_array(string array_name,Json::Value other_struct)
{
	json_struct[array_name].append(other_struct);
	whether_process=true;
}

void JSON_package::add_item_array(string name,string value)
{
	Json::Value item;
	item[name]=value;
	json_struct.append(item);
	whether_process=true;
}
void JSON_package::add_item_array(string name,int value)
{
	Json::Value item;
	item[name]=value;
	json_struct.append(item);
	whether_process=true;
}

void JSON_package::add_item_array(string array_name,string name,string value)
{
	Json::Value item;
	item[name]=value;
	json_struct[array_name].append(item);
	whether_process=true;
}
void JSON_package::add_item_array(string array_name,string name,int value)
{
	Json::Value item;
	item[name]=value;
	json_struct[array_name].append(item);
	whether_process=true;
}



void JSON_package::add_struct(string name,Json::Value other_struct)//添加别的结构体，结构体名字为name
{
	json_struct[name]=other_struct;
	whether_process=true;
}


Json::Value JSON_package::get_json(string name)
{
	if(!whether_process)cout<<"没处理过!\n";
	return json_struct[name];
}


Json::Value JSON_package::get_json()
{
	if(!whether_process)cout<<"没处理过!\n";
	return json_struct;
}
void JSON_package::ini()
{
	whether_normal=true;
	json_value= new char[20];
	json_string="";//字符串内容置空
}

JSON_package::JSON_package(Json::Value other_json_struct)
{
	ini();
	json_struct=other_json_struct;
	whether_process=true;
}

JSON_package::JSON_package()
{
	ini();
	whether_process=false;
}

JSON_package::JSON_package(char* StrBuf)
{
	ini();
	
	json_string=StrBuf;
	Json::Reader reader;
	
	if(StrBuf[0]=='{')//粗略过滤，有时候纯数字字符串也会被parse判断为正常json格式报文，导致取值出错
	{
		if(reader.parse(json_string, json_struct))//将字符串内容转化为结构体
			cout<<"解析成功!\n";
		else
			cout<<"reader parse error[json解析出错!]:"<<strerror(errno)<<endl;
	}
	else
		cout<<"错误json报文!\n";
	
	whether_process=true;
}

const char* JSON_package::to_StrBuf()
{
	if(!whether_process)cout<<"没处理过!\n";
	Json::FastWriter writer;
	//Json::Reader reader;
	json_string=writer.write(json_struct);//将结构体内容转变为字符串形式
	const char* StrBuf=json_string.c_str();
	
	return StrBuf;
}
 char* JSON_package::get_value(string name)
{
	if(!whether_process)cout<<"没处理过!\n";
	Json::FastWriter writer;
	
	//细节：如果这里直接：const char* StrBuf=json_struct[name].toStyledString().c_str();
	//在这里打印能正常显示，但函数返回的则变为乱码
	//原因为const char指向一个临时申请的字符串，在本函数结束时会销毁字符串内容，导致返回的StrBuf指向无效数据
	
	strcpy(json_value,json_struct[name].toStyledString().c_str());
	return json_value;
}



void JSON_package::add_item(string name,string value)//单纯为其添加属性,如果之前有过该name了，则value直接覆盖
{
	json_struct[name]=value;//对结构体新增赋值
	whether_process=true;

}
void JSON_package::add_item(string name,int value)//单纯为其添加属性,如果之前有过该name了，则value直接覆盖
{
	json_struct[name]=value;//对结构体新增赋值
	whether_process=true;
}
















#endif