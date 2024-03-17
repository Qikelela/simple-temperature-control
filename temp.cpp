#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <stdlib.h>
#include <algorithm>

namespace fs = std::filesystem;
using namespace std;

struct temperature {
    string name  = "";
    string input = "";
    float max    = 0.0;
    float min    = 0.0;
    temperature(const string a, const string a0, const float a1, const float a2) 
        : name(a), input(a0), max(a1), min(a2) { }
};

vector<temperature> temp;

template <typename T>
void readFile(const string& filePath, T& result) { 
    ifstream inFile(filePath);
    if (inFile.fail()) { inFile.close(); return; }
    inFile >> result;
    inFile.close();
}

void search() {
    const string path = "/sys/class/hwmon";
    for (const auto & entry0 : fs::directory_iterator(path)) {
        for (const auto & entry1 : fs::directory_iterator(entry0.path().u8string())) {
            const string p = entry1.path().filename().u8string();
            if (p.find("temp") != string::npos && p.find("input") != string::npos) {
                const string pp = entry1.path().u8string();
                const int len = pp.length()-5;
                const string pp1 = entry1.path().u8string().replace(len, 5, "label");
                const string pp2 = entry1.path().u8string().replace(len, 5, "max");
                const string pp3 = entry1.path().u8string().replace(len, 5, "min");
                string res1 = "None";  float res2  = 100000.0; float res3  = 0.0;
                readFile(pp1, res1); readFile(pp2, res2); readFile(pp3, res3);
                res2 = clamp(res2, 0.0f, 100000.0f);
                res3 = clamp(res3, 0.0f, 100000.0f);
                temp.push_back(temperature(res1, pp, res2/1000.0, res3/1000.0));
            }
        }
    }
}

int main() {
    search();
    double val = 0.0;
    int max_len = 0;
    int min_max = 0;
    string val_graph = "";
    const int len = temp.size();
    for (int i = 0; i < len; i++) {
        if (temp[i].name.length() > max_len) { max_len = temp[i].name.length(); }
        if (temp[i].max - temp[i].min > min_max) { min_max = temp[i].max - temp[i].min; }
    }
    for (int i = 0; i < len; i++) {
        for (int j = temp[i].name.length(); j < max_len+1; j++) {
            temp[i].name+=" ";
        }
    }
    string separator = ""; for (int i = 0; i < (max_len+8)+(min_max/10); i++) { separator+="─"; }
    while (true) {
        system("clear");
        printf("\x1B[37m%s\n", separator.c_str());
        for (int i = 0; i < len; i++) {
            
            readFile(temp[i].input, val);
            val = val / 1000.0;
            printf(" \x1B[37m%s%.0f°C", temp[i].name.c_str(), val);
            val_graph = "";
            val+=00.0f; //debug
            min_max = temp[i].max - temp[i].min;
            for (float j = temp[i].min; j < temp[i].max-1.0; j+=min_max/10.0) {
                if (j < val) { val_graph+= "■"; } else { val_graph+="☐"; }
            }
            if (float(val/temp[i].max) > 0.8f) {
                printf(" \x1B[31m%s\n", val_graph.c_str());
            } else if (float(val/temp[i].max) > 0.6f) {
                printf(" \x1B[33m%s\n", val_graph.c_str());
            } else {
                printf(" \x1B[32m%s\n", val_graph.c_str());
            }
        }
        printf("\x1B[37m%s\n", separator.c_str());
        sleep(1);
    }
    return 0;
}
