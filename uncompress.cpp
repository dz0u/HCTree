#include <iostream>
#include "Helper.hpp"
#include "HCTree.hpp"

int main(int argc, char* argv[]) {
    if(argc != 3) {
        std::cout << "Incorrect usage." << std::endl;
        return 1;
    } else {
        FancyInputStream istream = FancyInputStream(argv[1]);
        FancyOutputStream ostream = FancyOutputStream(argv[2]);
        if (!istream.good() || !ostream.good() || istream.filesize() == 0) {
            return 0;
        } else {
            int filesize = istream.read_int();
            std::vector<unsigned char> codelengths = std::vector<unsigned char>(256);
            for(int i = 0; i < 256; i++) {
                codelengths[i] = (istream.read_byte());
            }
            HCTree tree = HCTree();
            tree.createCodeBook(codelengths);
            for(int i = 0; i < filesize; i++) {
                ostream.write_byte(tree.decode(istream));
            }
            ostream.flush();
        }
    }
    return 0;
}