#ifndef HCTREE_HPP
#define HCTREE_HPP
#include <queue>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include "Helper.hpp"
using namespace std;

/**
 * A Huffman Code Tree class
 */
class HCTree {
    private:
        HCNode* root;
        vector<HCNode*> leaves;
        
        std::map<int, std::set<unsigned char>> codes;
        std::vector<unsigned char> codelens;
        std::map<unsigned char, uint32_t> tocanonicals;
        std::map<uint32_t, unsigned char> tosymbols;

        void clear(HCNode* curr);
        void codify(HCNode* root, int curr_len);
        void canonify();

    public:
        /**
         * Constructor, which initializes everything to null pointers
         */
        HCTree() : root(nullptr) {
            leaves = vector<HCNode*>(256, nullptr);
            codes = map<int, std::set<unsigned char>>();
            codelens = vector<unsigned char>(256, 0);

            tocanonicals = std::map<unsigned char, uint32_t>();
            tosymbols = std::map<uint32_t, unsigned char>();
        }

        ~HCTree();

        /**
         * Use the Huffman algorithm to build a Huffman coding tree.
         * PRECONDITION: freqs is a vector of ints, such that freqs[i] is the frequency of occurrence of byte i in the input file.
         * POSTCONDITION: root points to the root of the trie, and leaves[i] points to the leaf node containing byte i.
         */
        void build(const vector<int>& freqs);

        /**
         * Write to the given FancyOutputStream the sequence of bits coding the given symbol.
         * PRECONDITION: build() has been called, to create the coding tree, and initialize root pointer and leaves vector.
         */
        void encode(unsigned char symbol, FancyOutputStream & out);

        /**
         * Return symbol coded in the next sequence of bits from the stream.
         * PRECONDITION: build() has been called, to create the coding tree, and initialize root pointer and leaves vector.
         */
        unsigned char decode(FancyInputStream & in);

        std::map<int, std::set<unsigned char>> getCodes();

        std::vector<unsigned char> getCodeLens();

        void createCodeBook(std::vector<unsigned char> lengths);
};
#endif // HCTREE_HPP
