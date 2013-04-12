#include "adminmodule.h"
#include <fstream>
#include <string>

AdminModule::AdminModule()
  : killHex_("killHex", "Kill Hex jobs")
  , runGanglia_("runGanglia", "Ganglia")
  , runTop_("runTop", "Task manager")
  
{
	addProperty(killHex_);
    addProperty(runGanglia_);
    addProperty(runTop_);
    
    killHex_.onChange(CallMemberAction<AdminModule>(this, &AdminModule::killHex));
    runGanglia_.onChange(CallMemberAction<AdminModule>(this, &AdminModule::runGanglia));
    runTop_.onChange(CallMemberAction<AdminModule>(this, &AdminModule::runTop));
}


void AdminModule::runcmd(int num){
std::string cmd;
if (num == 0){cmd = std::string("hex -kill >> /dev/null");}
else if(num == 1){cmd = std::string("firefox http://biosim.moldyn.org/ganglia/ &");}
else {cmd = std::string("xterm top &");}
FILE* pipe = popen(cmd.c_str(), "r");
pclose(pipe);
}

