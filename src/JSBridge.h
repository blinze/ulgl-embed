#pragma once

#include <JavaScriptCore/JavaScript.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <variant>
#include <optional>

// Supported JS value types
using JSValue = std::variant<std::nullptr_t, bool, double, std::string>;
using JSArgs = std::vector<JSValue>;
using JSCallback = std::function<JSValue(const JSArgs&)>;

class JSBridge
{
private:
    std::unordered_map<std::string, JSCallback> m_Functions;
    static JSBridge* s_Instance;

    static JSValueRef JSCallHandler(JSContextRef ctx, JSObjectRef function,
                                    JSObjectRef thisObject, size_t argumentCount,
                                    const JSValueRef arguments[], JSValueRef* exception);

    static JSValue ConvertFromJS(JSContextRef ctx, JSValueRef value);
    static JSValueRef ConvertToJS(JSContextRef ctx, const JSValue& value);
    static std::string GetStringFromJS(JSContextRef ctx, JSValueRef value);

public:
    JSBridge();
    ~JSBridge();

    // Register a C++ function to be callable from JavaScript
    // Usage: bridge.Register("myFunction", [](const JSArgs& args) -> JSValue { ... });
    void Register(const std::string& name, JSCallback callback);

    // Unregister a function
    void Unregister(const std::string& name);

    // Bind all registered functions to a JavaScript context under window.native.*
    void BindToContext(JSContextRef ctx);

    // Get the singleton instance
    static JSBridge* Instance() { return s_Instance; }

    // Helper to get typed values from JSArgs
    template<typename T>
    static std::optional<T> GetArg(const JSArgs& args, size_t index);
};

// Template specializations for GetArg
template<>
inline std::optional<bool> JSBridge::GetArg<bool>(const JSArgs& args, size_t index)
{
    if (index >= args.size()) return std::nullopt;
    if (auto* val = std::get_if<bool>(&args[index])) return *val;
    return std::nullopt;
}

template<>
inline std::optional<double> JSBridge::GetArg<double>(const JSArgs& args, size_t index)
{
    if (index >= args.size()) return std::nullopt;
    if (auto* val = std::get_if<double>(&args[index])) return *val;
    return std::nullopt;
}

template<>
inline std::optional<int> JSBridge::GetArg<int>(const JSArgs& args, size_t index)
{
    if (index >= args.size()) return std::nullopt;
    if (auto* val = std::get_if<double>(&args[index])) return static_cast<int>(*val);
    return std::nullopt;
}

template<>
inline std::optional<float> JSBridge::GetArg<float>(const JSArgs& args, size_t index)
{
    if (index >= args.size()) return std::nullopt;
    if (auto* val = std::get_if<double>(&args[index])) return static_cast<float>(*val);
    return std::nullopt;
}

template<>
inline std::optional<std::string> JSBridge::GetArg<std::string>(const JSArgs& args, size_t index)
{
    if (index >= args.size()) return std::nullopt;
    if (auto* val = std::get_if<std::string>(&args[index])) return *val;
    return std::nullopt;
}

