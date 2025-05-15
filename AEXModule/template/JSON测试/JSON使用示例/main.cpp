#include "../../../../AEXModule.h"
#include <fstream>
#include <iostream>

// 测试用例1：创建简单JSON对象
void TestCreateJson() {
    Json::Value root;
    root["name"] = "AEX Module";
    root["version"] = 1.4;
    root["active"] = true;

    // 添加数组
    Json::Value dependencies(Json::arrayValue);
    dependencies.append("OpenCV");
    dependencies.append("TensorRT");
    root["dependencies"] = dependencies;

    // 序列化输出
    Json::StreamWriterBuilder writer;
    std::string jsonStr = Json::writeString(writer, root);
    std::cout << "=== 创建JSON测试 ===\n" << jsonStr << "\n\n";
}

// 测试用例2：解析JSON字符串
void TestParseJson() {
    const char* jsonStr = R"({
        "config": {
            "resolution": [1920, 1080],
            "fps": 60.0
        },
        "plugins": ["detection", "tracking"]
    })";

    Json::CharReaderBuilder reader;
    JSONCPP_STRING errs;
    Json::Value root;

    std::istringstream stream(jsonStr);
    if (Json::parseFromStream(reader, stream, &root, &errs)) {
        std::cout << "=== 解析JSON测试 ===\n";
        std::cout << "分辨率: "
            << root["config"]["resolution"][0].asInt() << "x"
            << root["config"]["resolution"][1].asInt() << "\n";
        std::cout << "帧率: " << root["config"]["fps"].asFloat() << "\n";
        std::cout << "插件数量: " << root["plugins"].size() << "\n\n";
    }
    else {
        std::cerr << "解析错误: " << errs << "\n";
    }
}

// 测试用例3：文件读写测试
void TestFileIO() {
    const std::string filename = "module_config.json";

    // 写入文件
    {
        Json::Value root;
        root["autoSave"] = true;
        root["cacheSize"] = 1024;

        std::ofstream ofs(filename);
        Json::StreamWriterBuilder writer;
        std::unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
        jsonWriter->write(root, &ofs);
    }

    // 读取文件
    {
        std::ifstream ifs(filename);
        Json::Value root;
        Json::CharReaderBuilder reader;
        JSONCPP_STRING errs;

        if (Json::parseFromStream(reader, ifs, &root, &errs)) {
            std::cout << "=== 文件IO测试 ===\n";
            std::cout << "自动保存: " << root["autoSave"].asBool() << "\n";
            std::cout << "缓存大小: " << root["cacheSize"].asInt() << "\n\n";
        }
        else {
            std::cerr << "文件读取错误: " << errs << "\n";
        }
    }
}

// 测试用例4：复杂结构操作
void TestComplexStructure() {
    Json::Value root;

    // 创建嵌套结构
    root["detection"]["model"] = "yolov5";
    root["detection"]["threshold"] = 0.6;

    // 添加对象数组
    Json::Value devices(Json::arrayValue);
    for (int i = 0; i < 3; ++i) {
        Json::Value dev;
        dev["id"] = i;
        dev["name"] = std::string("GPU") + std::to_string(i);
        devices.append(dev);
    }
    root["devices"] = devices;

    // 修改数据
    root["detection"]["threshold"] = 0.65;
    root["devices"][0]["name"] = "NPU";

    // 删除数据
    root.removeMember("version");

    // 序列化输出
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "\t";
    std::cout << "=== 复杂结构测试 ===\n"
        << Json::writeString(writer, root) << "\n";
}

// 测试用例5：异常处理演示
void TestExceptionHandling() {
    try {
        Json::Value root;
        // 故意制造类型错误
        root["invalid"] = "text";
        int value = root["invalid"].asInt();
        std::cout << "异常测试结果: " << value << "\n";
    }
    catch (const Json::LogicError& e) {
        std::cerr << "=== 异常捕获测试 ===\n"
            << "类型转换错误: " << e.what() << "\n\n";
    }
}

int main() {
    TestCreateJson();        // 基础创建测试
    TestParseJson();         // 字符串解析测试
    TestFileIO();            // 文件读写测试
    TestComplexStructure();  // 复杂结构操作
    TestExceptionHandling(); // 异常处理演示

    return 0;
}
