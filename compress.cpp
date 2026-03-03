#include <iostream>
#include "HCTree.hpp"

int main(int argc, char* argv[]) {
    // your program's main execution code
    if(argc != 3) {
        std::cout << "Incorrect usage." << std::endl;
        return 1;
    } else {
        FancyInputStream istream = FancyInputStream(argv[1]);
        FancyOutputStream ostream = FancyOutputStream(argv[2]);
        if(!istream.good() || !ostream.good() || istream.filesize() == 0) {
            return 0;
        } else {
            std::vector<int> frequency = vector<int>(256, 0);
            int thisbyte = -1;
            uint32_t i = 0;
            while(true) {
                thisbyte = istream.read_byte();
                if(thisbyte == EOF) {
                    break;
                } else {
                    frequency[(unsigned char)thisbyte]++;
                }
                i++;
            }

            if (i == 0) {
                return 0;
            }

            HCTree tree = HCTree();
            tree.build(frequency);
            std::map<int, std::set<unsigned char>> codes = tree.getCodes();
            std::vector<unsigned char> codelens = tree.getCodeLens();
            
            // Writing the header
            ostream.write_int(i); // Size of the file
            for(unsigned char codelen: codelens) {
                ostream.write_byte(codelen);
            }
            thisbyte = -1;
            istream.reset();
            while(true) {
                thisbyte = istream.read_byte();
                if(thisbyte == EOF) {
                    break;
                } else {
                    tree.encode((unsigned char)thisbyte, ostream);
                }
            }
            ostream.flush();
        }
    }
    return 0;
}