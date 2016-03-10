#ifndef SERVER_H
#define SERVER_H

#include <http_server.h>
#include <channel.h>
#include <cpprest.h>
#include <map>
#include "string_arg.h"

namespace comet{

class Server
{
public:
    Server();
    ~Server();

    bool Start(kw::StringArg host, unsigned short port, int thread_count);

private:
    kw::shared_ptr<cpprest::Response> Pub(const kw::shared_ptr<cpprest::Request>& request);
    kw::shared_ptr<cpprest::Response> Sub(const kw::shared_ptr<cpprest::Request>& request);
    kw::shared_ptr<cpprest::Response> Info(const kw::shared_ptr<cpprest::Request>& request);

    kw::shared_ptr<cpprest::Response> Result(bool success, kw::StringArg arg);

private:
    void AddRouteItem(const char* path, cpprest::Method method, kw::shared_ptr<cpprest::Response> (Server::*functor)(const kw::shared_ptr<cpprest::Request>& request));
    void OnClose(void* conn);

private:
    kw::Mutex mutex_;
    cpprest::RouteItems items_;
    std::map<std::string, kw::shared_ptr<Channel> > channels_;
    kw::shared_ptr<cpprest::CppRest> rest_frame_;
    kw::SlotHandle slot_handle_;
};

}

#endif // SERVER_H
