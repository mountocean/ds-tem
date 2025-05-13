#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>
#include <iomanip> // 添加此头文件以解决 setw 未定义的问题

#include <cctype> // 添加此头文件以解决 isalpha 和 ispunct 未定义的问题

using namespace std;

struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode* left, * right;

    HuffmanNode(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return l->freq > r->freq;
    }
};

// 1. 编码器部分

HuffmanNode* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> minHeap;

    for (auto& pair : freqMap) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();

        HuffmanNode* node = new HuffmanNode('\0', left->freq + right->freq);
        node->left = left;
        node->right = right;
        minHeap.push(node);
    }

    return minHeap.top();
}

void generateHuffmanCodes(HuffmanNode* root, const string& str, unordered_map<char, string>& huffmanCodes) {
    if (!root) return;
    if (root->data != '\0') {
        huffmanCodes[root->data] = str;
    }
    generateHuffmanCodes(root->left, str + "0", huffmanCodes);
    generateHuffmanCodes(root->right, str + "1", huffmanCodes);
}

string encodeText(const string& text, const unordered_map<char, string>& huffmanCodes) {
    string encodedText;
    for (char c : text) {
        encodedText += huffmanCodes.at(c);
    }
    return encodedText;
}

void writeCompressedFile(const string& fileName, const unordered_map<char, string>& huffmanCodes, const string& encodedText) {
    ofstream outFile(fileName, ios::binary);
    for (auto& pair : huffmanCodes) {
        outFile << pair.first << ":" << pair.second << "\n";
    }
    outFile << "\n" << encodedText;
    outFile.close();
}

void encoder() {
    cout << "请输入要压缩的文本文件路径（例如：D:\\code\\input.txt）：";
    string inputFileName;
    cin >> inputFileName;

    ifstream inFile(inputFileName);
    if (!inFile) {
        cout << "无法打开文件 " << inputFileName << "，请检查路径是否正确！" << endl;
        return;
    }

    string text((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    unordered_map<char, int> freqMap;
    for (char c : text) {
        freqMap[c]++;
    }

    HuffmanNode* root = buildHuffmanTree(freqMap);
    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    string encodedText = encodeText(text, huffmanCodes);

    cout << "请输入压缩后的文件路径（例如：D:\\code\\compressed.bin）：";
    string outputFileName;
    cin >> outputFileName;

    writeCompressedFile(outputFileName, huffmanCodes, encodedText);
    cout << "文件已压缩并保存为 " << outputFileName << endl;
}

// 2. 解码器部分

unordered_map<string, char> readHuffmanCodes(const string& fileName) {
    ifstream inFile(fileName);
    unordered_map<string, char> huffmanCodes;
    string line;
    while (getline(inFile, line) && !line.empty()) {
        size_t pos = line.find(":");
        char c = line[0];
        string code = line.substr(pos + 1);
        huffmanCodes[code] = c;
    }
    return huffmanCodes;
}

string decodeText(const string& encodedText, const unordered_map<string, char>& huffmanCodes) {
    string decodedText;
    string currentCode;

    for (char c : encodedText) {
        currentCode += c;  // 将当前字符加入编码串

        // 如果当前编码串在哈夫曼编码表中有匹配项
        if (huffmanCodes.find(currentCode) != huffmanCodes.end()) {
            decodedText += huffmanCodes.at(currentCode);  // 添加解码字符到解码文本
            currentCode.clear();  // 清空当前编码串，准备处理下一个字符
        }
    }

    return decodedText;
}



void decoder() {
    cout << "请输入要解压的文件路径（例如：D:\\code\\compressed.bin）：";
    string inputFileName;
    cin >> inputFileName;

    ifstream inFile(inputFileName, ios::binary);
    if (!inFile) {
        cout << "无法打开文件 " << inputFileName << "，请检查路径是否正确！" << endl;
        return;
    }

    unordered_map<string, char> huffmanCodes;
    string line;

    // 读取编码规则
    while (getline(inFile, line) && !line.empty()) {
        size_t pos = line.find(":");
        if (pos != string::npos) {
            char c = line[0];  // 获取字符
            string code = line.substr(pos + 1);  // 获取对应的编码
            huffmanCodes[code] = c;
            cout << "字符: " << c << ", 编码: " << code << endl;  // 输出编码规则，调试用
        }
    }

    getline(inFile, line);  // 跳过空行

    // 确保编码文本部分被正确读取
   /* string encodedText;
    while (getline(inFile, line)) {
        encodedText += line;
    }*/

    cout << "编码文本总长度: " << line.size() << endl;  // 输出编码文本的总长度，检查是否正确读取

    string decodedText = decodeText(line, huffmanCodes);

    cout << "请输入解压后的文件路径（例如：D:\\code\\decoded.txt）：";
    string outputFileName;
    cin >> outputFileName;

    ofstream outFile(outputFileName);
    outFile << decodedText;
    outFile.close();
    cout << "解码后的文本: " << decodedText << endl;  // 输出解码后的文本，检查是否正确
    cout << "文件已解压并保存为 " << outputFileName << endl;
}

// 3. 统计字母和标点频率
void countCharAndPunctFrequency() {
    cout << "请输入要统计的文本文件路径（例如：D:\\code\\input.txt）：";
    string fileName;
    cin >> fileName;

    ifstream inFile(fileName);
    if (!inFile) {
        cout << "无法打开文件 " << fileName << "，请检查路径是否正确！" << endl;
        return;
    }

    unordered_map<char, int> freq;
    char ch;
    while (inFile.get(ch)) {
        unsigned char uch = static_cast<unsigned char>(ch);
        if (isalpha(uch) || ispunct(uch)) {
            freq[ch]++;
        }
    }

    inFile.close();

    if (freq.empty()) {
        cout << "文件中没有检测到字母或标点符号。" << endl;
        return;
    }

    cout << "\n字母和标点符号的出现频率：\n";
    cout << left << setw(10) << "字符" << "出现次数" << endl;
    cout << "-------------------" << endl;
    for (const auto& entry : freq) {
        if (entry.first == ' ') continue; // 可选：跳过空格
        cout << left << setw(10) << entry.first << entry.second << endl;
    }
}


int main() {
    int choice;
	cout << "欢迎使用哈夫曼解码编码器！\n";
    do {
        cout << "1. 编码\n2. 解码\n3. 统计字母和标点频率\n0. 退出\n请选择：";
        cin >> choice;

        switch (choice) {
        case 1:
            encoder();
            break;
        case 2:
            decoder();
            break;
        case 3:
            countCharAndPunctFrequency();
            break;
        case 0:
            cout << "程序已退出。" << endl;
            break;
        default:
            cout << "无效的选项，请重新输入。\n";
        }
    } while (choice != 0);

    return 0;
}

