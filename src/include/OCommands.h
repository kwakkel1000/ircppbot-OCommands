#ifndef OCommands_h
#define OCommands_h
#include <core/ModuleBase.h>
#include <interfaces/DataInterface.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

using namespace std;

class DataInterface;
class OCommands : public ModuleBase
{
public:
    OCommands();
    ~OCommands();
    void read();
    void stop();
    void Init(DataInterface* pData);
    void timerrun();

private:

    DataInterface* mpDataInterface;

    void parse_privmsg();
    void ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger);

    //functions
    void god(string nick, string auth, int oa);
    void addchannel(string chan, string nick, string auth, string reqnick, string reqauth, int oa);
    void delchannel(string chan, string nick, string auth, int oa);
    void addobind(string nick, string auth, string command, string newbind, int reqaccess, int oa);
    void delobind(string nick, string auth, string command, string newbind, int oa);
    void addbind(string nick, string auth, string command, string newbind, int reqaccess, int oa);
    void delbind(string nick, string auth, string command, string newbind, int oa);
    void changeolevel(string nick, string auth, string reqnick, string reqauth, int reqaccess, int oa);
    void ousers(string nick, int oa);
    void say(string chan, string nick, string auth, string saystring, int oa);
    void action(string chan, string nick, string auth, string saystring, int oa);
    void raw(string nick, string auth, string dostring, int oa);
    void ocommandscommands(std::string mNick, string auth, int oa);


    boost::mutex parse_mutex;
    bool run;
    std::string ocommandstrigger;
    std::string command_table;
    boost::shared_ptr<boost::thread> privmsg_parse_thread;
};

#endif // OCommands_h

