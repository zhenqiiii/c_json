#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"cjson.h"



/*{
	"name": "Jack (\"Bee\") Nimble",
		"format" : {
		"type":       "rect",
			"width" : 1920,
			"height" : 1080,
			"interlace" : false,
			"frame rate" : 60
	}
}*/

int main() {


	return 0;
}




static void* (*cJSON_malloc)(size_t sz) = malloc;
static void(*cJSON_free)(void* ptr) = free;

static const char* ep;//保存出错位置
cJSON* cJSON_New_Item()
{
	cJSON* node = (cJSON*)cJSON_malloc(sizeof(cJSON));
	if (node) { memset(node, 0, sizeof(cJSON));}
	return node;
}

//解析函数
cJSON* cJSON_Parse(const char* value)
{

	return cJSON_ParseWithOpt(value, 0, 0);
}

void cJSON_Delete(cJSON* item)
{
	cJSON* next;
	while (item)
	{
		next = item->next;
		if (item->child)cJSON_Delete(item->child);//递归删除
		if (item->valueString)cJSON_free(item->valueString);
		if (item->string)cJSON_free(item->string);
		cJSON_free(item);//删除本身
		item = next;//移动到下一个节点
	}

}


//解析json字符串是否出错
cJSON* cJSON_ParseWithOpts(const char* value, const char** return_parse_end, int require_null_terminated)
{
	const char* end = NULL;
	cJSON* c = cJSON_New_Item();
	ep = NULL;
	if (!c) return NULL;

	end = parse_value(c,skip(value));
	if (!end)
	{
		cJSON_Delete(c);
		return NULL;
	}

	if (require_null_terminated)
	{
		end = skip(end);
		if (*end)
		{
			cJSON_Delete(c);
				ep = end;
				return NULL;
		}
	}
	
	if (return_parse_end)
	{
		*return_parse_end = end;
	}

	return c;
}
//总处理函数，首先判断值类型，再转到相应函数处理
const char* parse_value(cJSON* item, const char* value)
{
	//没有值
	if (!value) return NULL;
	//值为false
	if (!strncmp(value, "false", 5))
	{
		item->type = CJSON_FALSE;
		return value + 5;
	}
	//值为true
	if (!strncmp(value, "true", 4))
	{
		item->type = CJSON_TRUE;
		item->valueInt = 1;
		return value + 4;
	}
	//值为null
	if (!strncmp(value, "null", 4))
	{
		item->type = CJSON_NULL;
		return value + 4;
	}

	if (value == '\"') { return parse_string(item,value); }//碰到"时转去处理字符串
	if (*value == '-' || (*value>='0' && *value <= '9')) { return parse_number(item, value); }//碰到负号或数字时转去处理数字
	if (value == '[') { return parse_array(item, value); }//处理数组
	if (value == '{') { return parse_object(item, value); }//处理对象

	//如果都不符合
	ep = value;
	return NULL;
	


}


const char* skip(const char* in)
{
	while (in && *in && (unsigned char)*in <= 32)//无意义则跳过
		in++;
	return in;
}


//实际的处理不同类型节点的函数
const char* parse_string(cJSON* item, const char* str) {
	const char* ptr = str + 1;
	int len = 0;
	char* ptr2;
	char* out;

	if (*str != '\"')
	{
		ep = str;
		return NULL;

	}
	//先获取长度
	while (*ptr != '\"' && *ptr && ++len)
	{
		if (*ptr++ == '\\') ptr++;//true+2,false+1
	}
	out = (char*)CJSON_malloc(len + 1);
	if (!out)return NULL;
	ptr = str + 1;
	ptr2 = out;
	while (*ptr != '\"' && *ptr)
	{
		if (*ptr != '\\')*ptr2++ = *ptr++;//拷贝
	}
	*ptr2 = 0;
	if (*ptr == '\"')ptr++;//跳到下一个需要处理的位置
	item->valueString = out;//存放string的空间的头指针
	item->type = CJSON_STRING;
	return ptr;
}
const char* parse_number(cJSON* item, const char* num) {

	item->valueDouble = atof(num);
	item->valueInt = (int)item->valueDouble;
	item->type = CJSON_NUMBER;
	while (*num == '-' || *num == 'e' || *num == 'E' || *num == '.' || (*num >= '0' && *num <= '9')) { num++; }
	return num;
}



/*
下面的parse_array和parse_object中，连接过程中child指针应该还是起了一个中继的作用，指向下一个直接连接的节点
双向链表的连接还是靠next和prev。但是，数组类型的结点连接数组第一个元素以及object节点连接第一个键值对还是用的child指针
总结下来就是，树状结构中的父子节点用child连接，兄弟节点用双向链表（next和prev），child在连接兄弟节点时还起了中继作用
*/
const char* parse_array(cJSON* item, const char* value) {

	cJSON* child;
	if (*value != '[')
	{
		ep = value;
		return NULL;
	}
	item->type = CJSON_ARRAY;
	value = skip(value + 1);
	if (*value == ']')
		return value + 1;



	//存入子节点(数组元素)

	item->child = child = cJSON_New_Item();
	if (!item->child)return NULL;

	value = skip(parse_value(child, skip(value)));//判定数组元素类型，存入第一个
	if (!value) return NULL;

	while (*value == ',')
	{
		cJSON* new_item;
		if (!(new_item = cJSON_New_Item()))return NULL;

		//连接第二个
		child->next = new_item;
		new_item->prev = child;
		child = new_item;

		value = skip(parse_value(child, skip(value + 1)));//重复执行，一个一个连接
		if (!value) return NULL;
	}
	if (*value == ']')
		return value + 1;//数组输入完成

	//处理特殊情况
	ep = value;
	return NULL;
}
const char* parse_object(cJSON* item, const char* value) {

	cJSON* child;
	if (*value != '{')
	{
		ep = value;
		return NULL;
	}

	item->type = CJSON_OBJECT;
	value = skip(value + 1);
	if (*value == '}')return value + 1;//空对象

	//存入
	item->child = child = cJSON_New_Item();//创建子节点
	if (!item->child)return NULL;

	value = skip(parse_string(child, skip(value)));//先存入第一个子节点的键
	if (!value)return NULL;

	child->string = child->valueString;
	child->valueString = NULL;
	if (*value != ':')
	{
		ep = value;
		return NULL;

	}
	value = skip(parse_value(child, skip(value + 1)));//再存第一个子节点的值
	if (!value) return NULL;

	//继续下一个
	while (*value == ',')
	{
		cJSON* new_item;
		if (!(new_item = cJSON_New_Item()))return NULL;

		//连接第二个
		child->next = new_item;
		new_item->prev = child;
		child = new_item;//后移
		//重复执行
		value = skip(parse_string(child, skip(value + 1)));
		if (!value) return NULL;

		child->string = child->valueString;//键名称
		child->valueString = NULL;

		if (*value != ':')//键值
		{
			ep = value;
			return NULL;

		}
		value = skip(parse_value(child, skip(value + 1)));
		if (!value) return NULL;
	}


	if (*value == '}')return value + 1;//结束

	//出错情况
	ep = value;
	return NULL;
}


