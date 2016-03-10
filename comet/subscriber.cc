#include <string.h>
#include <response.h>
#include "subscriber.h"
#include "channel.h"

namespace comet {

Subscriber::Subscriber(kw::shared_ptr<cpprest::CppRest> rest_frame, const kw::shared_ptr<cpprest::Request>& request)
    :rest_frame_(rest_frame), request_(request)
{
}

Subscriber::~Subscriber()
{
}

void Subscriber::Send(const kw::MemoryBuffer& buffer)
{
    //printf("pub slot:%s\n", buffer.Data());
    kw::shared_ptr<cpprest::Response> response(new cpprest::Response());
    response->content_ = buffer;

    rest_frame_->Server()->Reply(request_->RequestHandler(), response);
}

}
