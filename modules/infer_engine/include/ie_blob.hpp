#ifndef __IE_BLOB_HPP__
#define __IE_BLOB_HPP__

#include <stdlib.h>
#include <string.h>

#define IE_BLOB_NAME_MAX_LEN    32

/**
 * 功能描述: Tensor尺寸
 * 
 */
struct IE_Shape
{
    int c;
    int h;
    int w;

    IE_Shape():
        c(0), h(0), w(0)
    {

    }

    IE_Shape(int c_, int h_, int w_):
        c(c_), h(h_), w(w_)
    {
        
    }

    IE_Shape(const IE_Shape& other):
        c(other.c), h(other.h), w(other.w)
    {

    }

    IE_Shape& operator= (const IE_Shape& other)
    {
        c = other.c;
        h = other.h;
        w = other.w;
        return *this;
    }
};

/**
 * 功能描述: 输出Tensor
 * 
 */
class IE_Blob
{
public:
    float* data;
    int c;
    int h;
    int w;
    char name[IE_BLOB_NAME_MAX_LEN];

public:
    IE_Blob():
        data(NULL),
        c(0),
        h(0),
        w(0)
    {
        memset(name, 0, IE_BLOB_NAME_MAX_LEN);
    }

    IE_Blob(int c_, int h_, int w_):
        c(c_),
        h(h_),
        w(w_)
    {
        data = (float*)malloc(sizeof(float) * c * h * w);
        memset(name, 0, IE_BLOB_NAME_MAX_LEN);
    }

    IE_Blob(const char* name_, int c_, int h_, int w_):
        c(c_),
        h(h_),
        w(w_)
    {
        printf("name is: %s, c h w are: %d %d %d \n ",name_, c, h, w);
        data = (float*)malloc(sizeof(float) * c * h * w);
        memset(name, 0, IE_BLOB_NAME_MAX_LEN);

        strncpy(name, name_, IE_BLOB_NAME_MAX_LEN);
    }

    IE_Blob(const IE_Blob& other):
        c(other.c),
        h(other.h),
        w(other.w)
    {
        data = (float*)malloc(sizeof(float) * c * h * w);
        memcpy(data, other.data, sizeof(float) * c * h * w);

        strncpy(name, other.name, IE_BLOB_NAME_MAX_LEN);
    }

    IE_Blob& operator= (const IE_Blob& other)
    {
        c = other.c;
        h = other.h;
        w = other.w;

        if (data != NULL)
        {
            free(data);
        }
        data = (float*)malloc(sizeof(float) * c * h * w);
        memcpy(data, other.data, sizeof(float) * c * h * w);

        strncpy(name, other.name, IE_BLOB_NAME_MAX_LEN);
        
        return *this;
    }

    ~IE_Blob()
    {
        if (NULL != data)
        {
            free(data);
            data = NULL;
        }
    }

    IE_Shape shape()
    {
        return IE_Shape(c, h, w);
    }
};

#endif