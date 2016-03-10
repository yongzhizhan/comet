#ifndef SUBSCRIBE_H
#define SUBSCRIBE_H

#include <http_server.h>
#include <cpprest.h>
#include "shared_lib.h"
#include "signal_slot.h"
#include "memory_buffer.h"

namespace comet {

class Channel;

class Subscriber
{
public:
    Subscriber(kw::shared_ptr<cpprest::CppRest> rest_frame, const kw::shared_ptr<cpprest::Request>& request);
    ~Subscriber();

    void Send(const kw::MemoryBuffer& buffer);
    kw::shared_ptr<cpprest::Request> request(){return request_;}

private:
    kw::shared_ptr<cpprest::Request> request_;
    kw::shared_ptr<cpprest::CppRest> rest_frame_;
};

}
#endif // SUBSCRIBE_H
