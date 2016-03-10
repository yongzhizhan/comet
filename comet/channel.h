#ifndef CHANNEL_H
#define CHANNEL_H

#include <list>
#include <algorithm>
#include <subscriber.h>
#include "shared_lib.h"
#include "memory_buffer.h"
#include "signal_slot.h"

namespace comet {

class Channel
{
public:
    Channel(const char* name):subscribers_(new std::map<void*, kw::shared_ptr<Subscriber> >()), name_(name){}

    void Pub(const kw::MemoryBuffer& buffer)
    {
        kw::shared_ptr<std::map<void*, kw::shared_ptr<Subscriber> > > tmp_subscribers(new std::map<void*, kw::shared_ptr<Subscriber> >());

        {
            kw::MutexGuard guard(mutex_);
            subscribers_.swap(tmp_subscribers);
        }

        std::map<void*, kw::shared_ptr<Subscriber> >::iterator iter = tmp_subscribers->begin();
        for(; iter!=tmp_subscribers->end(); ++iter)
        {
            kw::shared_ptr<Subscriber>& item = iter->second;
            item->Send(buffer);
            item.reset();
        }
    }

    void AddSubscriber(const kw::shared_ptr<Subscriber>& subscriber)
    {
        kw::MutexGuard guard(mutex_);
        (*subscribers_)[subscriber->request()->RequestHandler()->evcon] = subscriber;
    }

    void DelSubscriber(void* conn)
    {
        //run on libevent thread
        kw::MutexGuard guard(mutex_);
        subscribers_->erase(conn);
    }

    const char* name(){return name_;}

    int SubscriberCount()
    {
        return subscribers_->size();
    }

private:
    const char* name_;
    kw::Mutex mutex_;
    kw::shared_ptr<std::map<void*, kw::shared_ptr<Subscriber> > > subscribers_;
};

}

#endif // CHANNEL_H
