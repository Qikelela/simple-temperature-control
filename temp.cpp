#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <stdlib.h>
#include <set>
namespace fs = std::filesystem;
using namespace std;

vector<string> paths;

template <typename T>
void readFile(const string& filePath, T& result) {
    ifstream inFile(filePath);
    if (inFile.fail()) { inFile.close(); return; }
    inFile >> result;
    inFile.close();
}

void search() {
    string path = "/sys/class/hwmon";
    set<fs::path> sorted;
    for (const auto & entry0 : fs::directory_iterator(path)) {
        for (const auto & entry1 : fs::directory_iterator(entry0.path().u8string())) {
            string p = entry1.path().filename().u8string();
            if (p.find("temp") != string::npos && (p.find("label") != string::npos || p.find("input") != string::npos)) {
                sorted.insert(entry1.path());
            }
        }
    }
    for (auto &fi : sorted) { paths.push_back(fi.u8string()); }
}

int main() {
    search();
    string name; double temp = 0.0;
    int max = 0;
    for (int i = 0; i < paths.size(); i+=2) {
        readFile(paths[i+1], name);
        if (name.length() > max) { max = name.length(); }
    }
    string separator = ""; for (int i = 0; i < max+9; i++) { separator+="-"; }
    const int len = paths.size();
    while (true) {
        system("clear");
        printf("%s\n", separator.c_str());
        for (int i = 0; i < len; i+=2) {
            readFile(paths[i], temp);
            readFile(paths[i+1], name);
            name+=":";
            for (int j = name.length(); j < max+2; j++) {
                name+=" ";
            }
            printf(" %s %.0f°C\n", name.c_str(), temp/1000.0);
        }
        printf("%s\n", separator.c_str());
        sleep(1);
    }
    return 0;
}
