#include <queue>
#include "MessagesStructure.h"
#include <optional>
#include <mutex>
class MessageQueue
{
    const size_t MAX_QUEUE_SIZE = 1000;

    std::queue<Message> mque;
    mutable std::mutex mtx;

public:
    std::optional<Message> getMessage()
    {

        std::lock_guard<std::mutex> lock(mtx);
        if (!this->mque.empty())
        {
            Message msg = this->mque.front();
            this->mque.pop();
            return msg;
        }
        else
        {
            return std::nullopt;
        }
    }
    void pushMessage(Message msg)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (this->mque.size() == MAX_QUEUE_SIZE)
        {
            this->mque.pop();
        }
        this->mque.push(msg);
    }
    bool isEmpty() const
    {
        return this->mque.empty();
    }
    void clearQueue()
    {
        while (!this->mque.empty())
        {
            this->mque.pop();
        }
    }
};