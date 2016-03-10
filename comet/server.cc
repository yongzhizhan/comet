#include "server.h"
#include "subscriber.h"

namespace comet{

Server::Server()
{
}

Server::~Server()
{
}

bool Server::Start(kw::StringArg host, unsigned short port, int thread_count)
{
    printf("listen on %s:%d, thread:%d...\n", host.data(), port, thread_count);

    AddRouteItem("/pub", cpprest::Method_Get, &Server::Pub);
    AddRouteItem("/sub", cpprest::Method_Get, &Server::Sub);
    AddRouteItem("/info", cpprest::Method_Get, &Server::Info);

    rest_frame_.reset(new cpprest::CppRest(host, port, thread_count, items_));
    slot_handle_ = rest_frame_->Server()->close_signal_.Connect(std::tr1::bind(&Server::OnClose, this, std::tr1::placeholders::_1));

    rest_frame_->Start();
    rest_frame_->Join();
}

kw::shared_ptr<cpprest::Response> Server::Pub(const kw::shared_ptr<cpprest::Request>& request)
{
    const char* channel_name = request->GetParamS("name", 0);
    if(0 == channel_name)
    {
        return Result(false, "Invalid Param.");
    }

    kw::shared_ptr<Channel> channel_ptr;

    {
        kw::MutexGuard guard(mutex_);
        std::map<std::string, kw::shared_ptr<Channel> >::iterator iter = channels_.find(channel_name);
        if(channels_.end() == iter)
        {
            char buf[32];
            snprintf(buf, sizeof buf, "channel %s not exists.", channel_name);
            return Result(false, buf);
        }
        else
        {
            channel_ptr = iter->second;
        }
    }

    kw::MemoryBuffer buffer;

    const char* msg = request->GetParamS("msg", 0);
    if(0 != msg)
        buffer.Append(msg, strlen(msg));

    channel_ptr->Pub(buffer);

    return Result(true, "");
}

kw::shared_ptr<cpprest::Response> Server::Sub(const kw::shared_ptr<cpprest::Request>& request)
{
    const char* channel_name = request->GetParamS("name", 0);
    if(0 == channel_name)
    {
        return Result(false, "Invalid Param.");
    }

    kw::shared_ptr<Channel> channel_ptr;

    {
        kw::MutexGuard guard(mutex_);
        std::map<std::string, kw::shared_ptr<Channel> >::iterator iter = channels_.find(channel_name);
        if(channels_.end() == iter)
        {
            channel_ptr.reset(new Channel(channel_name));
            channels_[channel_name] = channel_ptr;
        }
        else
        {
            channel_ptr = iter->second;
        }
    }

    kw::shared_ptr<Subscriber> sub(new Subscriber(rest_frame_, request));
    channel_ptr->AddSubscriber(sub);

    return kw::shared_ptr<cpprest::Response>();

}

kw::shared_ptr<cpprest::Response> Server::Info(const kw::shared_ptr<cpprest::Request>& request)
{
    kw::shared_ptr<cpprest::Response> response(new cpprest::Response);

    int channel_count = 0;
    int sub_count = 0;
    {
        kw::MutexGuard guard(mutex_);
        channel_count = channels_.size();
        std::map<std::string, kw::shared_ptr<Channel> >::iterator iter = channels_.begin();
        for(; channels_.end() != iter; ++iter)
        {
            kw::shared_ptr<Channel>& channel_ptr = iter->second;
            sub_count += channel_ptr->SubscriberCount();
        }
    }

    char buf[1024] = {0};
    snprintf(buf, sizeof buf, "channel count:%d, subscriber count:%d", channel_count, sub_count);
    response->content_.Append(buf);

    return response;
}

kw::shared_ptr<cpprest::Response> Server::Result(bool success, kw::StringArg arg)
{
    kw::shared_ptr<cpprest::Response> response(new cpprest::Response);
    const char* result_str = success ? "true" : "false";

    response->content_.Append("{\"result\":\"").Append(result_str, strlen(result_str)).Append("\", \"msg\":").Append("\"").Append(arg.data(), strlen(arg.data())).Append("\"}");

    return response;
}

void Server::AddRouteItem(const char* path, cpprest::Method method, kw::shared_ptr<cpprest::Response> (Server::*functor)(const kw::shared_ptr<cpprest::Request>& request))
{
    cpprest::RouteItem item;

    item.path = path;
    item.method = method;
    item.functor = std::tr1::bind(functor, this, std::tr1::placeholders::_1);

    items_.push_back(item);
}

void Server::OnClose(void* conn)
{
    kw::MutexGuard guard(mutex_);
    std::map<std::string, kw::shared_ptr<Channel> >::iterator iter = channels_.begin();
    for(; channels_.end() != iter; ++iter)
    {
        kw::shared_ptr<Channel>& channel = iter->second;
        channel->DelSubscriber(conn);
    }
}

}
