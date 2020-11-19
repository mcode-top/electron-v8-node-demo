## 环境篇

`touch binding.gyp` // 创建构建文件
加入以下代码
```
{
  "targets": [
    {
      "target_name": "greet",  // greet 为输出的文件名        
      "sources": [                          
        "./src/greeting.cpp", // greeting.cpp为输入的c++文件
      ],
    }
  ]
}
```
创建一个入口c++ 文件
```
cd src
touch greeting.cpp

// 放入以下代码
#include <node.h>
using namespace v8;

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
```
使用`node-gyp configure` 如果没有node-gyp 需要使用`npm install -g node-gyp`全局安装,如果安装出错请检查是否安装Python
node-gyp 是负责将c++代码编译成js可以使用的.node文件
nodejs中可以使用`require("*.node")`引入
`node-gyp configure` 是负责构建一个 编译.node动态库的配置框架
构建好后 使用 `node-gyp build` 输出.node文件
在项目根目录 `touch index.js` 输入一下代码
```
const addon = require('./build/Release/greet.node')
console.log(addon,addon.greetHello()) // Function greetHello, hello
```
这就是简单的构建了一个c++ 与 js互通的demo

如果你的模块需求比较复杂项目结构大,建议使用 `Visual Studio`来编写
在项目中的build/binding.sln是c++项目的库,在vs中双击它会自动导入载入一个运行库
## electron中使用
在electron 使用.node文件需要 安装 `npm install node-loader@0.6.0 -s-d`用于加载.node文件
>tips:如果使用高本版的node-loader可能会出现模块被载入当内容为空的问题

在webpack.config.js 中需要设置刚安装的加载器
```
{
            test: /\.node$/,
            use: [
                {
                    loader: 'node-loader',
                    options: {
                        name: '/[path][name].[ext]',
                    },
                },
            ],
}
```
如果是electron+vue 需要在vue.config.js中配置
```
    configureWebpack: (config) => {
        config.module.rules.push({
            test: /\.node$/,
            use: [
                {
                    loader: 'node-loader',
                    options: {
                        name: '/[path][name].[ext]',
                    },
                },
            ],
        })
    },
```
在electron的运行文件中修改nodeIntegration=true
```
    webPreferences: {
      nodeIntegration: true
    }

```
在electron中使用你的.node文件会报如下错误
```
Error: The module '/path/to/native/module.node'
was compiled against a different Node.js version using
NODE_MODULE_VERSION $XYZ. This version of Node.js requires
NODE_MODULE_VERSION $ABC. Please try re-compiling or re-installing
the module (for instance, using `npm rebuild` or `npm install`).
```
>文档说明:Electron 支持原生的 Node 模块，但由于 Electron 非常有可能使用一个与您的系统上所安装的 Node 不同的 V8 引擎，您所使用的模块将需要被重新编译。 否则，当您尝试运行您的应用程序时， 将会遇到以下的错误：
[官网说明](https://www.electronjs.org/docs/tutorial/using-native-node-modules)

所以你需要安装 `npm install --save-dev electron-rebuild`来重构你的模块以适配Electron
这里还需要做一个你electron项目相同版本的electron`npm install electron@你的版本`
安装完成后再项目根目录输入`./node_modules/.bin/electron-rebuild`(win用户可以使用gitbash来运行这行命令)
它将执行与前面 `node-gyp configure` 相似的操作只是将node原生的运行库转为electron原生运行库来构建 [electron-rebuild更多说明](https://github.com/electron/electron-rebuild)
环境转换完成后你只需使用 `node-gyp build`来更新你的node插件
>教程模块涉及过多建议安装时使用cnpm 或者yarn 等来加速安装保障环境不出错 
### 以下是MFC可能需要配置的环境
```
右击左侧项目 -> 属性 -> 常规 -> MFC的使用-> 在共享DLL中使用MFC // node-gyp自动构建会是使用windows标准库
右击左侧项目 -> 属性 -> 常规 -> 字符集-> 使用多字节字符集 // 解决字符冲突
右击左侧项目 -> 属性 -> C/C++ ->  代码生成 -> 运行库 -> 多线程 DLL(/MD) // 与MFC相对应
右击左侧项目 -> 属性 -> C/C++ -> 链接器 -> 输入 -> 忽略特定默认库 -> 加入`Nafxcwd.lib;Libcmtd.lib` // 用于解决常规的一些代码冲突
```

