#include <node.h>
using namespace v8;

// 定义一个返回类型为 Napi String 的 greetHello 函数, 注意此处的 info
void greetHello(const v8::FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<String> hello = String::NewFromUtf8(isolate, "hello").ToLocalChecked();
    args.GetReturnValue().Set(hello);
}

// init is entry point.
void Init(Local<Object> exports, Local<Object> module) {

	NODE_SET_METHOD(exports, "greetHello", greetHello);

}
// greet 表示定义的文件名
NODE_MODULE(greet, Init);