
#include "ie_errcode.hpp"

/**
 * 功能描述: 错误码转字符串
 * 
 * 输入参数： code 
 * 返回参数：const char* 
 */
const char* IE_err2str(int code)
{
    auto it = IE_ERR_TABLE.find(code);
    if (it == IE_ERR_TABLE.end())
        return "unknown error";
    return it->second;
}