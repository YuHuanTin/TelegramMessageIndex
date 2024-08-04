//
// Created by AFETT on 2024/8/4.
//

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <mutex>
#include <queue>
#include <semaphore>

/**
 * 线程安全消息队列
 */
template<typename MESSAGE_TYPE>
class MessageQueue {
    std::queue<MESSAGE_TYPE>  messages_;
    std::mutex                mtx_msg_;
    std::counting_semaphore<> sem_signal_ { 0 };
    std::atomic<bool>         stop_flag_ { false };

public:
    void send_message(MESSAGE_TYPE &&Msg) {
        // push message
        {
            std::lock_guard lock(mtx_msg_);
            messages_.push(std::move(Msg));
        }
        sem_signal_.release();
    }

    /**
     * 获取消息
     * @return 消息，若无消息，返回 std::nullopt
     */
    std::optional<MESSAGE_TYPE> receive_message() {
        sem_signal_.acquire();
        if (stop_flag_.load()) {
            return std::nullopt;
        }

        MESSAGE_TYPE msg;
        // pop message
        {
            std::lock_guard lock(mtx_msg_);
            if (messages_.empty()) {
                return std::nullopt;
            }

            msg = std::move(messages_.front());
            messages_.pop();
        }
        return msg;
    }

    void stop(const bool Immediate_exit = false) {
        stop_flag_.store(Immediate_exit);
        // Release once to unblock one waiting receiver
        sem_signal_.release();
    }
};

#endif //MESSAGEQUEUE_H
