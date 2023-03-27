#ifndef SOCKETINFOS_HPP
# define SOCKETINFOS_HPP

struct SocketInfos
{
    std::string                         reqType;
    std::string                         uri;
    std::string                         body;
    std::map<std::string, std::string>	headers;
};

#endif