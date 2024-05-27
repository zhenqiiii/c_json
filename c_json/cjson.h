#pragma once


#ifndef _CJSON_H_
#define _CJSON_H_
/*
ʵ���������ܣ�
1.json���� 2.�ڵ���ɾ�Ĳ�
jsonһ��������{  ...  }��ʾ�������м�ֵ�ԣ���ʽΪ  "����":"ֵ"  ��ͬʱֵҲ�����Ƕ���,����ȵ�
{
    "name": "Jack (\"Bee\") Nimble",
    "format": {
        "type":       "rect",
        "width":      1920,
        "height":     1080,
        "interlace":  false,
        "frame rate": 60
    }
}
������json������һ��ʾ��
����ʹ���߼��ϵ���״�ṹȥ��ʾһ������ÿ����ֵ�Լ�Ϊһ���ڵ�
root--->child(name)
          |_  format--->child(type)      //�ֵܽڵ�֮����˫����������(ʵ���ϣ�
                       |_ width
                       |_height
                       |_interlace
                       |_frame rate
*/

#define CJSON_FALSE    0
#define CJSON_TRUE     1
#define CJSON_NULL     2
#define CJSON_NUMBER   3
#define CJSON_STRING   4
#define CJSON_ARRAY    5
#define CJSON_OBJECT   6

#define cJSON_AddFalseToOBject(object,name) \
cJSON_AddItemToObject(object,name,cJSON_CreateFalse())

#define cJSON_AddTrueToOBject(object,name) \
cJSON_AddItemToObject(object,name,cJSON_CreateTrue())

#define cJSON_AddBoolToOBject(object,name,b) \
cJSON_AddItemToObject(object,name,cJSON_CreateBool())

#define cJSON_AddNULLToOBject(object,name) \
cJSON_AddItemToObject(object,name,cJSON_CreateNull())

#define cJSON_AddNumberToOBject(object,name,number) \
cJSON_AddItemToObject(object,name,cJSON_CreateNumber(number))

#define cJSON_AddStringToOBject(object,name,s) \
cJSON_AddItemToObject(object,name,cJSON_CreateString(s))

/*��doubleת��Ϊint*/
#define cJSON_SetIntValue(object,val)\
((object)?((object)->valueInt = (object)->valueDouble=(val)) : (val))

#define cJSON_SetNumberValue(object,val)\
((object)?((object)->valueInt = (object)->valueDouble=(val)) : (val))



typedef struct cJSON {
    struct cJSON* prev, * next;
    struct cJSON* child;

        /*array�Լ�object���Ͷ���Ҫ�����ӽڵ�*/
    int type;       //ֵ������
    char* valueString;      //�ַ�������ֵ
    int valueInt;           //int����
    double valueDouble;     //double����

    char* string;//����ڵ�����



}cJSON;

/*��������parse��json�ַ���-->json����, Ҳ���ǽ�json����ת��Ϊһ����������ָ��root�ڵ��ָ��cJSON*
������
    value (const char*):char*ָ�룬ָ���������json����
*/
cJSON* cJSON_Parse(const char* value);



/*stringify:��json��������ת��Ϊjson���ݣ�����ӡ����
    �Ӹ��ڵ㿪ʼ�ݹ���������ڵ���ת��Ϊ�ַ���
������
    cJSON*item���������ĸ��ڵ�
ʹ�øú�������Ҫ���ݷ��ص�char*ָ���ͷ��ڴ�
    out = cJSON_Stringify(cJSON*item)
    printf(out)
    free(out)
*/
char* cJSON_Stringify(cJSON* item);



/*delete:�ݹ����ɾ�������ڵ㣬�ͷ��ڴ�
*/
void cJSON_Delete(cJSON* item);





//����Ϊ�����������õĸ�����������ת������

/*createobject:���ڴ���object���͵Ľ�㣨ֵΪ����ļ�ֵ�ԣ�,�����ýڵ�����ΪCJSON_OBJECT������cJSON����ָ�롣parse��������
*/
cJSON* cJSON_CreateObject(void);

/*createstring:����string���͵Ľ�㣨ֵΪ�ַ������͵����ԣ������ýڵ�����ΪCJSON_STRING������cJSON����ָ��
*/
cJSON* cJSON_CreateString(const char* string);

/*����ֵ����Ϊnumber�����ԣ����ýڵ�����ΪCJSON_NUMBER������cJSON����ָ��
*/
cJSON* cJSON_CreateNumber(double num);


/*����ֵ����Ϊ��������ԣ����ýڵ�����ΪCJSON_ARRAY������cJSON����ָ��*/
cJSON* cJSON_CreateArray(void);

//bool
cJSON* cJSON_CreateBool(int b);
cJSON* cJSON_CreateFalse(void);
cJSON* cJSON_CreateTrue(void);

//null
cJSON* cJSON_CreateNull(void);

/*object�ڵ���һ��CJSON_OBJECT���ͽ�㣬������ֵ�Ƕ��󣬶�item���Ǹö����е�һ����ֵ��
���߼���item�ڵ���object�ڵ���ӽڵ㣬�������������item���뵽object���ӽڵ��У���object�Ѿ����ӽڵ㣬������뵽�ӽڵ������β��
ͬʱ��item�ڵ���������Ϊstring
*/
void cJSON_AddItemToObject(cJSON* object, char* string, cJSON* item);

//����һ���������ƣ���item���뵽array����һ��CJSON_ARRAY���ͽڵ㣩�ӽڵ��У������ӽڵ�������ӽڵ�����ĩβ���߼��ϼӵ������ĩβ
void cJSON_CreateArray(cJSON* array, cJSON* item);


#endif
