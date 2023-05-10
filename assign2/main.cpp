#include <stdio.h>
#include "main.h"
#include "Cache.h"
#include "Cache.cpp"

Data* getData(string s) {
	stringstream ss;
	ss << s;
	int idata;
    float fdata;
    if (ss >> idata) 
		return new Int(idata);
    else if (ss >> fdata)
    	return new Float(fdata);
    else if (s.compare("true") || s.compare("false"))
    	return new Bool(s.compare("true"));
    else {
		s.resize(s.size()-1);
		return new Address(stoi(s));
	}
    return NULL;	
}
void simulate(string filename)
{
  	ifstream ifs;
  	ifs.open(filename, std::ifstream::in);
  	string s;
	SearchEngine* sr = new BST();
	ReplacementPolicy* rp;
	Cache* c;
  	while (getline(ifs, s))
  	{
    	stringstream ss(s);
    	string code,tmp;
    	ss >> code;
    	int addr;
		//Elem *elem = NULL;
    	switch (code[0]) {
			case 'M': //MAXSIZE
				ss >> addr;
				MAXSIZE = addr;
				break;
			case 'T':// ReplacementPolicy
				ss >> addr;
				if (addr==1) rp = new MFU();
				else if (addr==2) rp = new LFU();
				else if (addr==3) rp = new MFRU();
				else rp = new LFRU();
				c = new Cache(sr,rp);
				break;
    		case 'R': // read
			    ss >> addr; 
    			Data* res;
				res = c->read(addr);
    			if (res == NULL) {
    				ss >> tmp;
    				c->put(addr,getData(tmp));
    			} else {
					cout << res->getValue() << endl;
				}
    			break;
        	case 'U': // put
                ss >> addr;
                ss >> tmp;
                c->put(addr,getData(tmp));
				//if (elem)
				//	std::cout << "Removed:" << elem->addr << endl;
                break;
    		case 'W': // write
                ss >> addr;
    			ss >> tmp;
    			c->write(addr,getData(tmp));
				//if (elem)
				//	std::cout << "Removed:" << elem->addr << endl;
    			break;
    		case 'P': // print
    			cout << "Print replacement buffer\n";
    			c->printRP();
    			break;
    		case 'E': // print
    			cout << "Print search buffer\n";
    			c->printSE();
    			break;
            case 'H': // print
                c->printLP();
                break;
    	}
  	}
	delete c;
}
int main(int argc, char* argv[]) {
    string fileName = "test/test8.txt";
    simulate(fileName);
	return 0;
}

