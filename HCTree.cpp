#include "HCTree.hpp"

//destructor
HCTree::~HCTree() {
    clear(root);
}

//clears the huffman tree
void HCTree::clear(HCNode* curr) {
    if (curr == nullptr) {
        return;
    }

    clear(curr->c0);
    clear(curr->c1);
    delete curr;
}

//generates canonical headers from a huffman tree
void HCTree::codify(HCNode* root, int curr_len) {
    if(root == nullptr) {
        return;
    }

    if(root->c0 == nullptr && root->c1 == nullptr) {
        codes[curr_len].insert(root->symbol);
        codelens[root->symbol] = curr_len;
    } else {
        codify(root->c0, curr_len + 1);
        codify(root->c1, curr_len + 1);
    }
}

//generates canonical codes from a canonical header
void HCTree::canonify() {
    if (codes.size() == 0) {
        return;
    }
    tocanonicals.clear();
    tosymbols.clear();
    uint32_t code = 0;
    uint32_t prev_len = 0;
    for(auto& pair: codes) {
        code <<= pair.first - prev_len;

        for(unsigned char sym: pair.second) {
            tocanonicals[sym] = code;
            tosymbols[code] = sym;

            code++;
        }

        prev_len = pair.first;
    }
}

//generates a classic huffman tree and then calls canonicizing functions
void HCTree::build(const vector<int>& freqs) {
    //TODO
    codelens.clear();
    codelens.assign(256, 0);
    codes.clear();

    std::priority_queue<HCNode*, std::vector<HCNode*>, HCNodePtrComp> priorityq;
    for(size_t i = 0; i < freqs.size(); i++) {
        if (freqs[i] > 0) {
            HCNode* leaf = new HCNode(freqs[i], (unsigned char)i);
            priorityq.push(leaf);
            leaves[i] = leaf;
        } else {
            leaves[i] = nullptr;
        }
    }

    if(priorityq.size() == 0) {
        return;
    } else if (priorityq.size() == 1) {
        root = priorityq.top();
        priorityq.pop();
        codelens[root->symbol] = 1;
        codes[1].insert(root->symbol);
        canonify();
        return;
    }

    while(priorityq.size() > 1) {
        HCNode* left = priorityq.top();
        priorityq.pop();
        HCNode* right = priorityq.top();
        priorityq.pop();
        HCNode* inner = new HCNode(left->count + right->count, std::min(left->symbol, right->symbol));
        inner->c0 = left;
        inner->c1 = right;
        left->p = inner;
        right->p = inner;

        priorityq.push(inner);
    }
    root = priorityq.top();
    priorityq.pop();

    codify(root, 0);
    canonify();
}

//encodes a single symbol canonically
void HCTree::encode(unsigned char symbol, FancyOutputStream & out) {
    if(tocanonicals.find(symbol) == tocanonicals.end()) {
        return;
    } else {
        unsigned char codelength = codelens.at(symbol);
        uint32_t code = tocanonicals.at(symbol);
        bool obit = 0;
        for(int i = codelength - 1; i >= 0; i--) {
            obit = (code >> i) & 1;
            out.write_bit(obit);
        }
    }
}

//decodes a single canonical symbol
unsigned char HCTree::decode(FancyInputStream & in) {
    uint32_t code = 0;
    unsigned char code_length = 0;

    unsigned char tosymbol = 0;
    while(code_length < 256) {
        int ibit = in.read_bit();
        if(ibit == -1) {
            return 0;
        }

        code = (code << 1) | ibit;
        code_length++;

        if(tosymbols.find(code) != tosymbols.end()) {
            tosymbol = tosymbols.at(code);
            if(codelens.at(tosymbol) == code_length) {
                return tosymbol;
            }
        }
    }
    return 0;
}

//fetches one type of canonical header
std::map<int, std::set<unsigned char>> HCTree::getCodes() {
    return codes;
}

//fetches another type of canonical header
std::vector<unsigned char> HCTree::getCodeLens() {
    return codelens;
}

//fetches one kind of canonical header, generates the other from it,
//then calls canonify to generate codes
void HCTree::createCodeBook(std::vector<unsigned char> lengths) {
    codelens.clear();
    codelens = lengths;

    codes.clear();
    for(size_t i = 0; i < codelens.size(); i++) {
        if (codelens.at(i) > 0) {
            codes[codelens.at(i)].insert(i);
        }
    }

    canonify();
}