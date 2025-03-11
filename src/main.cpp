#include <memory>

#include <fstream>
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "dbf/DBFHead.h"
#include "dbf/DBFHeadJsonSerializer.hpp"
#include "dbf/DBFHeadField.h"
#include "dbf/DBFHeadFieldJsonSerializer.hpp"

using namespace dbf;

int main() {
	if (!boost::filesystem::exists("json/dbf/")) {
		boost::filesystem::create_directories("json/dbf/");
	}
	{
		DBFHead dBFHead;
		std::ofstream file("json/dbf/DBFHead.json");
		if (file.is_open()) {
			nlohmann::json j = dBFHead;
			file << j.dump(4);
			file.close();
			SPDLOG_INFO("DBFHead.json 写入成功!");
		} else {
			SPDLOG_ERROR("DBFHead.json 打开文件失败");;
		}
	}
	{
		DBFHeadField dBFHeadField;
		std::ofstream file("json/dbf/DBFHeadField.json");
		if (file.is_open()) {
			nlohmann::json j = dBFHeadField;
			file << j.dump(4);
			file.close();
			SPDLOG_INFO("DBFHeadField.json 写入成功!");
		} else {
			SPDLOG_ERROR("DBFHeadField.json 打开文件失败");;
		}
	}
	return 0;
}
