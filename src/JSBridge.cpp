#include "JSBridge.h"
#include <iostream>

JSBridge* JSBridge::s_Instance = nullptr;

JSBridge::JSBridge()
{
    s_Instance = this;
}

JSBridge::~JSBridge()
{
    if (s_Instance == this)
        s_Instance = nullptr;
}

void JSBridge::Register(const std::string& name, JSCallback callback)
{
    m_Functions[name] = std::move(callback);
}

void JSBridge::Unregister(const std::string& name)
{
    m_Functions.erase(name);
}

std::string JSBridge::GetStringFromJS(JSContextRef ctx, JSValueRef value)
{
    JSStringRef jsStr = JSValueToStringCopy(ctx, value, nullptr);
    if (!jsStr) return "";
    
    size_t maxSize = JSStringGetMaximumUTF8CStringSize(jsStr);
    std::string result(maxSize, '\0');
    size_t actualSize = JSStringGetUTF8CString(jsStr, &result[0], maxSize);
    result.resize(actualSize > 0 ? actualSize - 1 : 0);
    JSStringRelease(jsStr);
    
    return result;
}

JSValue JSBridge::ConvertFromJS(JSContextRef ctx, JSValueRef value)
{
    if (JSValueIsNull(ctx, value) || JSValueIsUndefined(ctx, value))
        return nullptr;
    
    if (JSValueIsBoolean(ctx, value))
        return JSValueToBoolean(ctx, value);
    
    if (JSValueIsNumber(ctx, value))
        return JSValueToNumber(ctx, value, nullptr);
    
    if (JSValueIsString(ctx, value))
        return GetStringFromJS(ctx, value);
    
    // For objects/arrays, convert to string representation
    return GetStringFromJS(ctx, value);
}

JSValueRef JSBridge::ConvertToJS(JSContextRef ctx, const JSValue& value)
{
    if (std::holds_alternative<std::nullptr_t>(value))
        return JSValueMakeNull(ctx);
    
    if (auto* b = std::get_if<bool>(&value))
        return JSValueMakeBoolean(ctx, *b);
    
    if (auto* d = std::get_if<double>(&value))
        return JSValueMakeNumber(ctx, *d);
    
    if (auto* s = std::get_if<std::string>(&value))
    {
        JSStringRef jsStr = JSStringCreateWithUTF8CString(s->c_str());
        JSValueRef result = JSValueMakeString(ctx, jsStr);
        JSStringRelease(jsStr);
        return result;
    }
    
    return JSValueMakeUndefined(ctx);
}

JSValueRef JSBridge::JSCallHandler(JSContextRef ctx, JSObjectRef function,
                                   JSObjectRef thisObject, size_t argumentCount,
                                   const JSValueRef arguments[], JSValueRef* exception)
{
    if (!s_Instance)
        return JSValueMakeUndefined(ctx);
    
    JSStringRef nameProperty = JSStringCreateWithUTF8CString("_nativeFuncName");
    JSValueRef nameValue = JSObjectGetProperty(ctx, function, nameProperty, nullptr);
    JSStringRelease(nameProperty);
    
    std::string funcName = GetStringFromJS(ctx, nameValue);
    
    auto it = s_Instance->m_Functions.find(funcName);
    if (it == s_Instance->m_Functions.end())
    {
        std::cerr << "[JSBridge] Function not found: " << funcName << std::endl;
        return JSValueMakeUndefined(ctx);
    }
    
    JSArgs args;
    args.reserve(argumentCount);
    for (size_t i = 0; i < argumentCount; i++)
        args.push_back(ConvertFromJS(ctx, arguments[i]));
    
    try
    {
        JSValue result = it->second(args);
        return ConvertToJS(ctx, result);
    }
    catch (const std::exception& e)
    {
        std::cerr << "[JSBridge] Exception in " << funcName << ": " << e.what() << std::endl;
        return JSValueMakeUndefined(ctx);
    }
}

void JSBridge::BindToContext(JSContextRef ctx)
{
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    
    JSClassDefinition classDef = kJSClassDefinitionEmpty;
    classDef.className = "NativeBridge";
    JSClassRef classRef = JSClassCreate(&classDef);
    JSObjectRef nativeObj = JSObjectMake(ctx, classRef, nullptr);
    JSClassRelease(classRef);
    
    for (const auto& [name, callback] : m_Functions)
    {
        JSStringRef funcNameStr = JSStringCreateWithUTF8CString(name.c_str());
        JSObjectRef funcObj = JSObjectMakeFunctionWithCallback(ctx, funcNameStr, JSCallHandler);
        
        JSStringRef nameProperty = JSStringCreateWithUTF8CString("_nativeFuncName");
        JSStringRef nameValue = JSStringCreateWithUTF8CString(name.c_str());
        JSObjectSetProperty(ctx, funcObj, nameProperty, 
                          JSValueMakeString(ctx, nameValue), 
                          kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontEnum, nullptr);
        JSStringRelease(nameProperty);
        JSStringRelease(nameValue);
        
        JSObjectSetProperty(ctx, nativeObj, funcNameStr, funcObj, kJSPropertyAttributeNone, nullptr);
        JSStringRelease(funcNameStr);
    }
    
    JSStringRef nativeName = JSStringCreateWithUTF8CString("native");
    JSObjectSetProperty(ctx, globalObj, nativeName, nativeObj, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(nativeName);
    
    std::cout << "[JSBridge] Bound " << m_Functions.size() << " functions to window.native" << std::endl;
}

