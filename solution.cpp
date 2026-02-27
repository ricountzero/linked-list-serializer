#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>

using namespace std;

struct ListNode {
    ListNode* prev = nullptr;
    ListNode* next = nullptr;
    ListNode* rand = nullptr;
    string data;
};

ListNode* buildFromFile(const string& path) {
    ifstream in(path);
    if (!in.is_open())
        throw runtime_error("Cannot open " + path);

    vector<string> dataVec;
    vector<int>         randIdx;

    string line;
    while (getline(in, line)) {
        // Use rfind to allow ';' inside data
        auto sep = line.rfind(';');
        if (sep == string::npos)
            throw runtime_error("Bad format: " + line);

        dataVec.push_back(line.substr(0, sep));
        randIdx.push_back(stoi(line.substr(sep + 1)));
    }

    if (dataVec.empty()) return nullptr;

    const int n = static_cast<int>(dataVec.size());
    vector<ListNode*> nodes(n);
    for (int i = 0; i < n; ++i) {
        nodes[i] = new ListNode();
        nodes[i]->data = dataVec[i];
    }

    for (int i = 0; i < n; ++i) {
        nodes[i]->prev = (i > 0)     ? nodes[i - 1] : nullptr;
        nodes[i]->next = (i < n - 1) ? nodes[i + 1] : nullptr;
        nodes[i]->rand = (randIdx[i] >= 0) ? nodes[randIdx[i]] : nullptr;
    }

    return nodes[0];
}

void writeInt32(ofstream& out, int32_t v) {
    out.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

// Binary format per node: [int32 data_len][data bytes][int32 rand_index or -1]
void serialize(ListNode* head, const string& path) {
    unordered_map<ListNode*, int32_t> index;
    int32_t n = 0;
    for (ListNode* cur = head; cur; cur = cur->next)
        index[cur] = n++;

    ofstream out(path, ios::binary);
    if (!out.is_open())
        throw runtime_error("Cannot open " + path);

    writeInt32(out, n);

    for (ListNode* cur = head; cur; cur = cur->next) {
        auto len = static_cast<int32_t>(cur->data.size());
        writeInt32(out, len);
        out.write(cur->data.data(), len);

        int32_t ri = (cur->rand) ? index.at(cur->rand) : -1;
        writeInt32(out, ri);
    }
}

int32_t readInt32(ifstream& in) {
    int32_t v = 0;
    in.read(reinterpret_cast<char*>(&v), sizeof(v));
    return v;
}

ListNode* deserialize(const string& path) {
    ifstream in(path, ios::binary);
    if (!in.is_open())
        throw runtime_error("Cannot open " + path);

    int32_t n = readInt32(in);
    if (n == 0) return nullptr;

    vector<ListNode*> nodes(n);
    vector<int32_t>   randIdx(n);

    for (int32_t i = 0; i < n; ++i) {
        nodes[i] = new ListNode();
        int32_t len = readInt32(in);
        nodes[i]->data.resize(len);
        in.read(&nodes[i]->data[0], len);
        randIdx[i] = readInt32(in);
    }

    for (int32_t i = 0; i < n; ++i) {
        nodes[i]->prev = (i > 0)     ? nodes[i - 1] : nullptr;
        nodes[i]->next = (i < n - 1) ? nodes[i + 1] : nullptr;
        nodes[i]->rand = (randIdx[i] >= 0) ? nodes[randIdx[i]] : nullptr;
    }

    return nodes[0];
}

int main() {
    try {
        ListNode* head = buildFromFile("inlet.in");
        serialize(head, "outlet.out");
        cout << "Serialization complete → outlet.out\n";

        // Sanity check: deserialize and print
        ListNode* head2 = deserialize("outlet.out");
        cout << "Deserialization check:\n";
        int idx = 0;
        for (ListNode* cur = head2; cur; cur = cur->next, ++idx) {
            cout << "  [" << idx << "] data=\"" << cur->data << "\""
                      << "  rand=";
            if (cur->rand) cout << "\"" << cur->rand->data << "\"";
            else           cout << "nullptr";
            cout << "\n";
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
