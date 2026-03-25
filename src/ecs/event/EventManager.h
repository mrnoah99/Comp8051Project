#include <functional>
#include <memory>
#include <vector>

#include "../Entity.h"
#include "BaseEvent.h"

class EventManager {
    using Handler = std::function<void(const BaseEvent&)>;

    public:
        void emit(const BaseEvent& event) {
            for (const auto& listener : listeners) {
                listener(event);
            }
        }

        void subscribe(const Handler& callback) {
            listeners.emplace_back(callback);
        }

    private:
        std::vector<Handler> listeners;
};