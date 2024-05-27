#pragma once


#ifndef _CJSON_H_
#define _CJSON_H_
/*
实现两个功能：
1.json解析 2.节点增删改查
json一个对象由{  ...  }表示，里面有键值对，格式为  "属性":"值"  ，同时值也可以是对象,数组等等
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
上面是json官网的一个示例
我们使用逻辑上的树状结构去表示一个对象，每个键值对即为一个节点
root--->child(name)
          |_  format--->child(type)      //兄弟节点之间用双向链表连接(实际上）
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

/*将double转化为int*/
#define cJSON_SetIntValue(object,val)\
((object)?((object)->valueInt = (object)->valueDouble=(val)) : (val))

#define cJSON_SetNumberValue(object,val)\
((object)?((object)->valueInt = (object)->valueDouble=(val)) : (val))



typedef struct cJSON {
    struct cJSON* prev, * next;
    struct cJSON* child;

        /*array以及object类型都需要设立子节点*/
    int type;       //值的类型
    char* valueString;      //字符串类型值
    int valueInt;           //int类型
    double valueDouble;     //double类型

    char* string;//保存节点名称



}cJSON;

/*解析函数parse：json字符串-->json对象, 也就是将json数据转化为一棵树，返回指向root节点的指针cJSON*
参数：
    value (const char*):char*指针，指向待解析的json数据
*/
cJSON* cJSON_Parse(const char* value);



/*stringify:将json对象（树）转化为json数据，即打印出来
    从根节点开始递归遍历，将节点树转化为字符串
参数：
    cJSON*item：对象树的根节点
使用该函数后，需要根据返回的char*指针释放内存
    out = cJSON_Stringify(cJSON*item)
    printf(out)
    free(out)
*/
char* cJSON_Stringify(cJSON* item);



/*delete:递归遍历删除各个节点，释放内存
*/
void cJSON_Delete(cJSON* item);





//以下为解析函数调用的各种数据类型转化函数

/*createobject:用于创建object类型的结点（值为对象的键值对）,并设置节点类型为CJSON_OBJECT。返回cJSON类型指针。parse函数调用
*/
cJSON* cJSON_CreateObject(void);

/*createstring:创建string类型的结点（值为字符串类型的属性），设置节点类型为CJSON_STRING。返回cJSON类型指针
*/
cJSON* cJSON_CreateString(const char* string);

/*用于值类型为number的属性，设置节点类型为CJSON_NUMBER，返回cJSON类型指针
*/
cJSON* cJSON_CreateNumber(double num);


/*用于值类型为数组的属性，设置节点类型为CJSON_ARRAY，返回cJSON类型指针*/
cJSON* cJSON_CreateArray(void);

//bool
cJSON* cJSON_CreateBool(int b);
cJSON* cJSON_CreateFalse(void);
cJSON* cJSON_CreateTrue(void);

//null
cJSON* cJSON_CreateNull(void);

/*object节点是一个CJSON_OBJECT类型结点，即它的值是对象，而item则是该对象中的一个键值对
故逻辑上item节点是object节点的子节点，所以这个函数将item加入到object的子节点中，若object已经有子节点，将其加入到子节点链表的尾部
同时将item节点名称设置为string
*/
void cJSON_AddItemToObject(cJSON* object, char* string, cJSON* item);

//和上一个函数类似，将item加入到array（是一个CJSON_ARRAY类型节点）子节点中，已有子节点则加入子节点链表末尾，逻辑上加到数组的末尾
void cJSON_CreateArray(cJSON* array, cJSON* item);


#endif
