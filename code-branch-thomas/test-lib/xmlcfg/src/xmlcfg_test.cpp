#include "../../../xmlcfg/include/xmlcfg.h"
#include <iostream.h>

int main(int argc, char* argv[])
{
	XMLCfg cfg;
    cfg.SaveCFG("C:/temp/test.xml");
    cfg.SetIntAttribute("/CFG/COMPONENTS/SEGMENTER","mode",25);
    
    std::string value;

     cout << cfg.GetIntAttribute("/CFG/COMPONENTS/SEGMENTER","mode") << endl;
        return 0;
}
