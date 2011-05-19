#include "include/OCommands.h"
#include <core/Global.h>
#include <core/DatabaseData.h>
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

extern "C" ModuleInterface* create()
{
    return new OCommands;
}

extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

OCommands::OCommands()
{
}

OCommands::~OCommands()
{
    stop();
	Global::Instance().get_IrcData().DelConsumer(mpDataInterface);
    delete mpDataInterface;
}

void OCommands::Init(DataInterface* pData)
{
	mpDataInterface = pData;
	mpDataInterface->Init(false, false, false, true);
    Global::Instance().get_IrcData().AddConsumer(mpDataInterface);
    ocommandstrigger = Global::Instance().get_ConfigReader().GetString("ocommandstrigger");
    command_table = "OCommandsCommands";
    DatabaseData::Instance().DatabaseData::AddBinds(command_table);
}

void OCommands::stop()
{
    run = false;
    mpDataInterface->stop();
    std::cout << "OCommands::stop" << std::endl;
    privmsg_parse_thread->join();
    std::cout << "privmsg_parse_thread stopped" << std::endl;
}

void OCommands::read()
{
    run = true;
    assert(!privmsg_parse_thread);
    privmsg_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&OCommands::parse_privmsg, this)));
}

void OCommands::parse_privmsg()
{
    std::vector< std::string > data;
    while(run)
    {
        data = mpDataInterface->GetPrivmsgQueue();
        PRIVMSG(data, ocommandstrigger);
    }
}

void OCommands::ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger)
{
    //cout << "OCommands" << endl;
    UsersInterface& U = Global::Instance().get_Users();
    std::string auth = U.GetAuth(nick);
    std::string bind_command = DatabaseData::Instance().GetCommandByBindNameAndBind(command_table, command);
    int bind_access = DatabaseData::Instance().GetAccessByBindNameAndBind(command_table, command);
	std::cout << bind_command << " " << bind_access << std::endl;

	//debug
	if (boost::iequals(bind_command, "debug"))
	{
		if (args.size() == 0)
		{
            U.Debug();
            Global::Instance().get_Channels().Debug();
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//ocommandscommands
	if (boost::iequals(bind_command, "ocommandscommands"))
	{
		if (args.size() == 0)
		{
			if (U.GetGod(nick) == 1)
			{
				ocommandscommands(nick, auth, bind_access);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//god
	if (boost::iequals(bind_command, "god"))
	{
		if (args.size() == 0)
		{
			god(nick, auth, bind_access);
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//delchannel
	if (boost::iequals(bind_command, "delchannel"))
	{
		if (args.size() == 0)
		{
			if (U.GetGod(nick) == 1)
			{
				delchannel(chan, nick, auth, bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//addchannel
	if (boost::iequals(bind_command, "addchannel"))
	{
		if (args.size() == 1)
		{
			if (U.GetGod(nick) == 1)
			{
				addchannel(chan, nick, auth, args[0], U.GetAuth(args[0]), bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	// joinchannel
	if (boost::iequals(bind_command, "joinchannel"))
	{
		if (args.size() == 0)
		{
			if (U.GetGod(nick) == 1)
			{
				joinchannel(chan, nick, auth, bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	// partchannel
	if (boost::iequals(bind_command, "partchannel"))
	{
		if (args.size() == 0)
		{
			if (U.GetGod(nick) == 1)
			{
				partchannel(chan, nick, auth, bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//say
	if (boost::iequals(bind_command, "say"))
	{
		if (args.size() >= 1)
		{
			if (U.GetGod(nick) == 1)
			{
				std::string saystring;
				for (unsigned int j = 0; j < args.size()-1; j++)
				{
					saystring = saystring + args[j] + " ";
				}
				if (args.size() > 0)
				{
					saystring = saystring + args[args.size()-1];
				}
				say(chan, nick, auth, saystring, bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	// action
	if (boost::iequals(bind_command, "action"))
	{
		if (args.size() >= 1)
		{
			if (U.GetGod(nick) == 1)
			{
				std::string saystring;
				for (unsigned int j = 0; j < args.size()-1; j++)
				{
					saystring = saystring + args[j] + " ";
				}
				if (args.size() > 0)
				{
					saystring = saystring + args[args.size()-1];
				}
				action(chan, nick, auth, saystring, bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//raw
	if (boost::iequals(bind_command, "raw"))
	{
		if (args.size() >= 1)
		{
			if (U.GetGod(nick) == 1)
			{
				std::string dostring;
				for (unsigned int j = 0; j < args.size()-1; j++)
				{
					dostring = dostring + args[j] + " ";
				}
				if (args.size() > 0)
				{
					dostring = dostring + args[args.size()-1];
				}
				raw(nick, auth, dostring, bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//changeolevel
	if (boost::iequals(bind_command, "changeolevel"))
	{
		if (args.size() == 2)
		{
			if (U.GetGod(nick) == 1)
			{
				changeolevel(nick, auth, args[0], U.GetAuth(args[0]), convertString(args[1]), bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//delobind
	if (boost::iequals(bind_command, "delobind"))
	{
		if (args.size() == 2)
		{
			if (U.GetGod(nick) == 1)
			{
				delobind(nick, auth, args[0], args[1], bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//delbind
	if (boost::iequals(bind_command, "delbind"))
	{
		if (args.size() == 2)
		{
			if (U.GetGod(nick) == 1)
			{
				delbind(nick, auth, args[0], args[1], bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//addobind
	if (boost::iequals(bind_command, "addobind"))
	{
		if (args.size() == 3)
		{
			if (U.GetGod(nick) == 1)
			{
				addobind(nick, auth, args[0], args[1], convertString(args[2]), bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//addbind
	if (boost::iequals(bind_command, "addbind"))
	{
		if (args.size() == 3)
		{
			if (U.GetGod(nick) == 1)
			{
				addbind(nick, auth, args[0], args[1], convertString(args[2]), bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	//simulate
	if (boost::iequals(bind_command, "simulate"))
	{
		if (args.size() >= 3)
		{
			if (U.GetGod(nick) == 1)
			{
				std::vector< std::string > simulate_args;
				for (unsigned int j = 2; j < args.size(); j++)
				{
					simulate_args.push_back(args[j]);
				}
				simulate(nick, auth, chan, args[0], args[1], simulate_args, bind_access);
			}
			else
			{
				std::string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
				Send(returnstring);
			}
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}
}

void OCommands::god(string nick, string auth, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    if (U.GetGod(nick) > 0)
    {
        vector<string> nicks = U.GetNicks(auth);
        for (unsigned int i = 0; i < nicks.size(); i++)
        {
            if (nicks[i] != "NULL")
            {
                U.SetGod(nicks[i], 0);
            }
        }
        string sqlstring = "UPDATE auth SET god = '0' WHERE auth = '" + auth + "';";!
        RawSql(sqlstring);
        returnstring = "NOTICE " + nick + " :" + irc_reply("god_disabled", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
    else
    {
    	int oaccess = U.GetOaccess(nick);
        if (oaccess >= oa)
        {
            vector<string> nicks = U.GetNicks(auth);
            for (unsigned int i = 0; i < nicks.size(); i++)
            {
                if (nicks[i] != "NULL")
                {
                    U.SetGod(nicks[i], 1);
                }
            }
            string sqlstring = "UPDATE auth SET god = '1' WHERE auth = '" + auth + "';";
            RawSql(sqlstring);
            returnstring = "NOTICE " + nick + " :" + irc_reply("god_enabled", U.GetLanguage(nick)) + "\r\n";
            Send(returnstring);
        }
        else
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
            Send(returnstring);
        }
    }
}

void OCommands::say(string chan, string nick, string auth, string saystring, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        string returnstr = "PRIVMSG " + chan + " :" + saystring + "\r\n";
        Send(returnstr);
        returnstring = "NOTICE " + nick + " :" + irc_reply("say", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::action(string chan, string nick, string auth, string saystring, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        string returnstr = "PRIVMSG " + chan + " :" + char(1) + "ACTION " + saystring + char(1) + "\r\n";
        Send(returnstr);
        returnstring = "NOTICE " + nick + " :" + irc_reply("action", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::raw(string nick, string auth, string dostring, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        string returnstr = dostring + "\r\n";
        Send(returnstr);
        returnstring = "NOTICE " + nick + " :" + irc_reply("raw", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::addchannel(string chan, string nick, string auth, string reqnick, string reqauth, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    string returnstring;
    if (boost::iequals(reqauth,"NULL") != true)
    {
        int oaccess = U.GetOaccess(nick);
        cout << convertInt(oaccess) << endl;
        if (oaccess >= oa)
        {
            if (C.GetCid(chan) == "NULL")
            {
				boost::uuids::uuid uuid = boost::uuids::random_generator()();
				std::stringstream ss;
				ss << uuid;
				std::string ChannelUuid = ss.str();
				std::cout << "ChannelUuid: " << ChannelUuid<< std::endl;
                std::string UserUuid = U.GetUid(reqnick);
				C.RegistrateChannel(ChannelUuid, chan);
                if ((ChannelUuid != "NULL") && (UserUuid != "NULL"))
                {
					C.AddUserToChannel(ChannelUuid, UserUuid, 500);
                }
                string joinstr = "JOIN " + chan + "\r\n";
                Send(joinstr);
                C.AddChannel(chan);
				std::string whostring;
				if (Global::Instance().get_ConfigReader().GetString("whoextra") == "true")
				{
					whostring = "WHO " + chan + " %ncaf\r\n";
				}
				else
				{
					whostring = "WHO " + chan + "\r\n";
				}
				Send(whostring);
                returnstring = "NOTICE " + nick + " :" + irc_reply("addchannel", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$nick$", nick);
                returnstring = irc_reply_replace(returnstring, "$auth$", auth);
                returnstring = irc_reply_replace(returnstring, "$regnick$", reqnick);
                returnstring = irc_reply_replace(returnstring, "$regauth$", reqauth);
                returnstring = irc_reply_replace(returnstring, "$channel$", chan);
                Send(returnstring);
            }
        }
        else
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
            Send(returnstring);
        }
    }
}

void OCommands::delchannel(string chan, string nick, string auth, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    if (oaccess >= oa)
    {
        std::string ChannelUuid = C.GetCid(chan);
        string sqlstring;
        if (ChannelUuid != "NULL")
        {
			C.UnregistrateChannel(ChannelUuid);
            vector<string> nicks = C.GetNicks(chan);
            for (unsigned int i = nicks.size(); i > 0; i--)
            {
                U.DelChannel(nicks[i-1], chan);
            }
            string partstr = "PART " + chan + "\r\n";
            Send(partstr);
            returnstring = "NOTICE " + nick + " :" + irc_reply("delchannel", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$channel$", chan);
            Send(returnstring);
        }
        else
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("delchannel_nochannel", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$channel$", chan);
            Send(returnstring);
        }
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::joinchannel(string chan, string nick, string auth, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    if (oaccess >= oa)
    {
        std::string ChannelUuid = C.GetCid(chan);
        string sqlstring;
        //if (ChannelUuid != "NULL")
        {
            string partstr = "JOIN " + chan + "\r\n";
            Send(partstr);
            returnstring = "NOTICE " + nick + " :" + irc_reply("joinchannel", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$channel$", chan);
            Send(returnstring);
        }
        /*else
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("joinchannel_nochannel", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$channel$", chan);
            Send(returnstring);
        }*/
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::partchannel(string chan, string nick, string auth, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    //ChannelsInterface& C = Global::Instance().get_Channels();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    if (oaccess >= oa)
    {
		string partstr = "PART " + chan + "\r\n";
		Send(partstr);
		returnstring = "NOTICE " + nick + " :" + irc_reply("partchannel", U.GetLanguage(nick)) + "\r\n";
		returnstring = irc_reply_replace(returnstring, "$channel$", chan);
		Send(returnstring);
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::addobind(string nick, string auth, string command, string newbind, int reqaccess, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    //bool exists = false;
    std::vector< std::string > binds = DatabaseData::Instance().GetBindVectorByBindName(command_table);
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        /*for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(binds[i], newbind) == true)
            {
                exists = true;
                returnstring = "NOTICE " + nick + " :" + irc_reply("addobind_exists", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$bind$", newbind);
                returnstring = irc_reply_replace(returnstring, "$command$", command);
                Send(returnstring);
            }
        }
        if (exists == false)
        {
            commands.push_back(command);
            binds.push_back(newbind);
            oas.push_back(reqaccess);
            string sqlstring = "INSERT into ocommands ( command, bind, oaccess) VALUES ( '" + command + "', '" + newbind + "', '" + convertInt(reqaccess) + "' );";
            RawSql(sqlstring);
            returnstring = "NOTICE " + nick + " :" + irc_reply("addobind", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$bind$", newbind);
            returnstring = irc_reply_replace(returnstring, "$command$", command);
            Send(returnstring);
        }*/
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::delobind(string nick, string auth, string command, string bind, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    std::vector< std::string > binds = DatabaseData::Instance().GetBindVectorByBindName(command_table);
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        /*bool deleted = false;
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(binds[i], bind) == true && boost::iequals(commands[i], command) == true)
            {
                commands.erase(commands.begin()+i);
                binds.erase(binds.begin()+i);
                oas.erase(oas.begin()+i);
                string sqlstring = "DELETE from ocommands where bind = '" + bind + "' AND command = '" + command + "';";
                RawSql(sqlstring);
                deleted = true;
                returnstring = "NOTICE " + nick + " :" + irc_reply("delobind", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$bind$", bind);
                returnstring = irc_reply_replace(returnstring, "$command$", command);
                Send(returnstring);
            }
        }
        if (!deleted)
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("delobind_nobind", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$bind$", bind);
            returnstring = irc_reply_replace(returnstring, "$command$", command);
            Send(returnstring);
        }*/
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::addbind(string nick, string auth, string command, string newbind, int reqaccess, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    //bool exists = false;
    std::vector< std::string > binds = DatabaseData::Instance().GetBindVectorByBindName("ChannelBotCommands");
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        /*for (unsigned int i = 0; i < cbinds.size(); i++)
        {
            if (boost::iequals(cbinds[i], newbind) == true)
            {
                exists = true;
                returnstring = "NOTICE " + nick + " :" + irc_reply("addbind_exists", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$bind$", newbind);
                returnstring = irc_reply_replace(returnstring, "$command$", command);
                Send(returnstring);
            }
        }
        if (exists == false)
        {
            ccommands.push_back(command);
            cbinds.push_back(newbind);
            cas.push_back(reqaccess);
            string sqlstring = "INSERT into commands ( command, bind, caccess) VALUES ( '" + command + "', '" + newbind + "', '" + convertInt(reqaccess) + "' );";
            RawSql(sqlstring);
            returnstring = "NOTICE " + nick + " :" + irc_reply("addbind", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$bind$", newbind);
            returnstring = irc_reply_replace(returnstring, "$command$", command);
            Send(returnstring);
        }*/
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::delbind(string nick, string auth, string command, string bind, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    std::vector< std::string > binds = DatabaseData::Instance().GetBindVectorByBindName("ChannelBotCommands");
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        /*bool deleted = false;
        for (unsigned int i = 0; i < cbinds.size(); i++)
        {
            if (boost::iequals(cbinds[i], bind) == true && boost::iequals(ccommands[i], command) == true)
            {
                ccommands.erase(ccommands.begin()+i);
                cbinds.erase(cbinds.begin()+i);
                cas.erase(cas.begin()+i);
                string sqlstring = "DELETE from commands where bind = '" + bind + "' AND command = '" + command + "';";
                RawSql(sqlstring);
                deleted = true;
                returnstring = "NOTICE " + nick + " :" + irc_reply("delbind", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$bind$", bind);
                returnstring = irc_reply_replace(returnstring, "$command$", command);
                Send(returnstring);
            }
        }
        if (!deleted)
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("delbind_nobind", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$bind$", bind);
            returnstring = irc_reply_replace(returnstring, "$command$", command);
            Send(returnstring);
        }*/
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::changeolevel(string nick, string auth, string reqnick, string reqauth, int reqaccess, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    if (boost::iequals(reqauth,"NULL") != true)
    {
        if (U.GetOaccess(nick) >= oa)
        {
            string sqlstring = "UPDATE auth SET oaccess = '" + convertInt(reqaccess) + "' WHERE auth = '" + reqauth + "';";
            RawSql(sqlstring);
            vector<string> nicks = U.GetNicks(reqauth);
            for (unsigned int i = 0; i < nicks.size(); i++)
            {
                if (nicks[i] != "NULL")
                {
                    U.SetOaccess(nicks[i], reqaccess);
                    if (reqaccess < 1)
                    {
                        U.SetGod(nicks[i], 0);
                        string sqlstring = "UPDATE auth SET god = '0' WHERE auth = '" + reqauth + "';";
                        RawSql(sqlstring);
                    }
                }
            }
            returnstring = "NOTICE " + nick + " :" + irc_reply("changeolevel", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$nick$", reqnick);
            returnstring = irc_reply_replace(returnstring, "$auth$", reqauth);
            returnstring = irc_reply_replace(returnstring, "$access$", convertInt(reqaccess));
            Send(returnstring);
        }
        else
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
            Send(returnstring);
        }
    }
}

void OCommands::ousers(string nick, int oa)
{
    /*
    vector<string> auths = C->GetAuths(chan);
    string returnstr;
    cout << auths.size() << endl;
    for ( unsigned int i = 0 ; i < auths.size(); i++ )
    {
        int access = C->GetAccess(chan, auths[i]);
        if (access > 0)
        {
            returnstr = "NOTICE " + nick + " :" + convertInt(access) + "    " + auths[i] + "\r\n";
            Send(returnstr);
        }
    }*/
}

void OCommands::ocommandscommands(string mNick, string auth, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    if (boost::iequals(auth,"NULL") != true)
    {
        if (U.GetOaccess(mNick) >= oa)
        {
            unsigned int length = U.GetWidth(mNick);
            unsigned int amount = U.GetWidthLength(mNick);
            string commandrpl = irc_reply("ocommands", U.GetLanguage(mNick));
            returnstring = "NOTICE " + mNick + " :";
            for (unsigned int l = 0; l < (((length * amount) / 2) - commandrpl.size()/2); l++)
            {
                returnstring = returnstring + " ";
            }
            returnstring = returnstring + commandrpl + "\r\n";
            Send(returnstring);


			returnstring = "NOTICE " + mNick + " :";
			returnstring = returnstring + fillspace("bind", 20);
			returnstring = returnstring + fillspace("command", 20);
			returnstring = returnstring + "access\r\n";
			Send(returnstring);
			std::vector< std::string > binds = DatabaseData::Instance().GetBindVectorByBindName(command_table);
			sort (binds.begin(), binds.end());
			for (unsigned int binds_it = 0; binds_it < binds.size(); binds_it++)
			{
				std::string bind_access = convertInt(DatabaseData::Instance().GetAccessByBindNameAndBind(command_table, binds[binds_it]));
				std::string bind_command = DatabaseData::Instance().GetCommandByBindNameAndBind(command_table, binds[binds_it]);
				if (bind_command != "")
				{
					returnstring = "NOTICE " + mNick + " :";
					returnstring = returnstring + fillspace(binds[binds_it], 20);
					returnstring = returnstring + fillspace(bind_command, 20);
					returnstring = returnstring + bind_access + "\r\n";
					Send(returnstring);
				}
			}
            /*vector<string> sortbinds = binds;
            sort (sortbinds.begin(), sortbinds.end());
            vector<string> command_reply_vector = lineout(sortbinds, amount, length);
            for (unsigned int h = 0; h < command_reply_vector.size(); h++)
            {
                returnstring = "NOTICE " + nick + " :" + command_reply_vector[h] + "\r\n";
                Send(returnstring);
            }*/
        }
    }
}



void OCommands::timerrun()
{
    //cout << "OCommands::timerrun()" << endl;
}
