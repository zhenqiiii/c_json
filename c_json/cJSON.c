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

static const char* ep;//�������λ��
cJSON* cJSON_New_Item()
{
	cJSON* node = (cJSON*)cJSON_malloc(sizeof(cJSON));
	if (node) { memset(node, 0, sizeof(cJSON));}
	return node;
}

//��������
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
		if (item->child)cJSON_Delete(item->child);//�ݹ�ɾ��
		if (item->valueString)cJSON_free(item->valueString);
		if (item->string)cJSON_free(item->string);
		cJSON_free(item);//ɾ������
		item = next;//�ƶ�����һ���ڵ�
	}

}


//����json�ַ����Ƿ����
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
//�ܴ������������ж�ֵ���ͣ���ת����Ӧ��������
const char* parse_value(cJSON* item, const char* value)
{
	//û��ֵ
	if (!value) return NULL;
	//ֵΪfalse
	if (!strncmp(value, "false", 5))
	{
		item->type = CJSON_FALSE;
		return value + 5;
	}
	//ֵΪtrue
	if (!strncmp(value, "true", 4))
	{
		item->type = CJSON_TRUE;
		item->valueInt = 1;
		return value + 4;
	}
	//ֵΪnull
	if (!strncmp(value, "null", 4))
	{
		item->type = CJSON_NULL;
		return value + 4;
	}

	if (value == '\"') { return parse_string(item,value); }//����"ʱתȥ�����ַ���
	if (*value == '-' || (*value>='0' && *value <= '9')) { return parse_number(item, value); }//�������Ż�����ʱתȥ��������
	if (value == '[') { return parse_array(item, value); }//��������
	if (value == '{') { return parse_object(item, value); }//�������

	//�����������
	ep = value;
	return NULL;
	


}


const char* skip(const char* in)
{
	while (in && *in && (unsigned char)*in <= 32)//������������
		in++;
	return in;
}


//ʵ�ʵĴ���ͬ���ͽڵ�ĺ���
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
	//�Ȼ�ȡ����
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
		if (*ptr != '\\')*ptr2++ = *ptr++;//����
	}
	*ptr2 = 0;
	if (*ptr == '\"')ptr++;//������һ����Ҫ�����λ��
	item->valueString = out;//���string�Ŀռ��ͷָ��
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
�����parse_array��parse_object�У����ӹ�����childָ��Ӧ�û�������һ���м̵����ã�ָ����һ��ֱ�����ӵĽڵ�
˫����������ӻ��ǿ�next��prev�����ǣ��������͵Ľ�����������һ��Ԫ���Լ�object�ڵ����ӵ�һ����ֵ�Ի����õ�childָ��
�ܽ��������ǣ���״�ṹ�еĸ��ӽڵ���child���ӣ��ֵܽڵ���˫������next��prev����child�������ֵܽڵ�ʱ�������м�����
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



	//�����ӽڵ�(����Ԫ��)

	item->child = child = cJSON_New_Item();
	if (!item->child)return NULL;

	value = skip(parse_value(child, skip(value)));//�ж�����Ԫ�����ͣ������һ��
	if (!value) return NULL;

	while (*value == ',')
	{
		cJSON* new_item;
		if (!(new_item = cJSON_New_Item()))return NULL;

		//���ӵڶ���
		child->next = new_item;
		new_item->prev = child;
		child = new_item;

		value = skip(parse_value(child, skip(value + 1)));//�ظ�ִ�У�һ��һ������
		if (!value) return NULL;
	}
	if (*value == ']')
		return value + 1;//�����������

	//�����������
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
	if (*value == '}')return value + 1;//�ն���

	//����
	item->child = child = cJSON_New_Item();//�����ӽڵ�
	if (!item->child)return NULL;

	value = skip(parse_string(child, skip(value)));//�ȴ����һ���ӽڵ�ļ�
	if (!value)return NULL;

	child->string = child->valueString;
	child->valueString = NULL;
	if (*value != ':')
	{
		ep = value;
		return NULL;

	}
	value = skip(parse_value(child, skip(value + 1)));//�ٴ��һ���ӽڵ��ֵ
	if (!value) return NULL;

	//������һ��
	while (*value == ',')
	{
		cJSON* new_item;
		if (!(new_item = cJSON_New_Item()))return NULL;

		//���ӵڶ���
		child->next = new_item;
		new_item->prev = child;
		child = new_item;//����
		//�ظ�ִ��
		value = skip(parse_string(child, skip(value + 1)));
		if (!value) return NULL;

		child->string = child->valueString;//������
		child->valueString = NULL;

		if (*value != ':')//��ֵ
		{
			ep = value;
			return NULL;

		}
		value = skip(parse_value(child, skip(value + 1)));
		if (!value) return NULL;
	}


	if (*value == '}')return value + 1;//����

	//�������
	ep = value;
	return NULL;
}


