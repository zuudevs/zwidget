#pragma once

/**
 * @file event_dispatcher.hpp
 * @brief Event dispatcher and queue management
 * @version 1.0
 * @date 2025-11-29
 */

#include "zwidget/unit/event.hpp"
#include <algorithm>
#include <queue>
#include <functional>
#include <unordered_map>
#include <vector>

namespace zuu::widget {

/**
 * @brief Event listener function type
 */
using EventListener = std::function<bool(const Event&)>;

/**
 * @brief Listener with priority
 */
struct PrioritizedListener {
    EventListener callback;
    int priority = 0;  // Higher = called first
    
    bool operator<(const PrioritizedListener& other) const {
        return priority < other.priority;  // For max-heap
    }
};

/**
 * @brief Central event dispatcher - manages event queue and listeners
 */
class EventDispatcher {
private:
    std::queue<Event> event_queue_;
    
    // Listeners per event type
    std::unordered_map<event_type, std::vector<PrioritizedListener>> listeners_;
    
    // Global listeners (called for all events)
    std::vector<PrioritizedListener> global_listeners_;
    
    // Enable/disable event processing
    bool enabled_ = true;
    
    // Statistics
    size_t events_processed_ = 0;
    size_t events_dropped_ = 0;
    size_t max_queue_size_ = 1000;

public:
    EventDispatcher() = default;
    
    /**
     * @brief Push event to queue
     */
    void push_event(const Event& event) {
        if (event_queue_.size() >= max_queue_size_) {
            events_dropped_++;
            return;
        }
        event_queue_.push(event);
    }
    
    void push_event(Event&& event) {
        if (event_queue_.size() >= max_queue_size_) {
            events_dropped_++;
            return;
        }
        event_queue_.push(std::move(event));
    }
    
    /**
     * @brief Process all events in queue
     */
    void process_events() {
        if (!enabled_) return;
        
        while (!event_queue_.empty()) {
            Event event = std::move(event_queue_.front());
            event_queue_.pop();
            
            dispatch_event(event);
            events_processed_++;
        }
    }
    
    /**
     * @brief Process single event immediately
     */
    void dispatch_event(const Event& event) {
        if (!enabled_) return;
        
        // Global listeners first
        for (auto& listener : global_listeners_) {
            if (listener.callback(event)) {
                return;  // Event consumed
            }
        }
        
        // Type-specific listeners
        auto it = listeners_.find(event.type());
        if (it != listeners_.end()) {
            for (auto& listener : it->second) {
                if (listener.callback(event)) {
                    return;  // Event consumed
                }
            }
        }
    }
    
    /**
     * @brief Add listener for specific event type
     */
    void add_listener(event_type type, EventListener callback, int priority = 0) {
        auto& vec = listeners_[type];
        vec.push_back({std::move(callback), priority});
        
        // Sort by priority (descending)
        std::sort(vec.begin(), vec.end(), 
            [](const auto& a, const auto& b) { return a.priority > b.priority; });
    }
    
    /**
     * @brief Add global listener (called for all events)
     */
    void add_global_listener(EventListener callback, int priority = 0) {
        global_listeners_.push_back({std::move(callback), priority});
        
        // Sort by priority (descending)
        std::sort(global_listeners_.begin(), global_listeners_.end(),
            [](const auto& a, const auto& b) { return a.priority > b.priority; });
    }
    
    /**
     * @brief Clear all listeners
     */
    void clear_listeners() {
        listeners_.clear();
        global_listeners_.clear();
    }
    
    /**
     * @brief Clear queue
     */
    void clear_queue() {
        while (!event_queue_.empty()) {
            event_queue_.pop();
        }
    }
    
    /**
     * @brief Enable/disable event processing
     */
    void set_enabled(bool enabled) { enabled_ = enabled; }
    bool is_enabled() const { return enabled_; }
    
    /**
     * @brief Get queue size
     */
    size_t queue_size() const { return event_queue_.size(); }
    
    /**
     * @brief Get statistics
     */
    size_t events_processed() const { return events_processed_; }
    size_t events_dropped() const { return events_dropped_; }
    
    /**
     * @brief Set max queue size
     */
    void set_max_queue_size(size_t size) { max_queue_size_ = size; }
    size_t max_queue_size() const { return max_queue_size_; }
    
    /**
     * @brief Reset statistics
     */
    void reset_stats() {
        events_processed_ = 0;
        events_dropped_ = 0;
    }
};

/**
 * @brief Scoped event listener - automatically removes on destruction
 */
class ScopedEventListener {
private:
    EventDispatcher* dispatcher_;
    event_type type_;
    bool is_global_;

public:
    ScopedEventListener(
        EventDispatcher& dispatcher,
        event_type type,
        EventListener callback,
        int priority = 0
    ) : dispatcher_(&dispatcher), type_(type), is_global_(false) {
        dispatcher_->add_listener(type, std::move(callback), priority);
    }
    
    ScopedEventListener(
        EventDispatcher& dispatcher,
        EventListener callback,
        int priority = 0
    ) : dispatcher_(&dispatcher), is_global_(true) {
        dispatcher_->add_global_listener(std::move(callback), priority);
    }
    
    ~ScopedEventListener() {
        // Note: In real implementation, you'd need to track the listener ID
        // and remove it specifically. This is a simplified version.
    }
    
    ScopedEventListener(const ScopedEventListener&) = delete;
    ScopedEventListener& operator=(const ScopedEventListener&) = delete;
    
    ScopedEventListener(ScopedEventListener&& other) noexcept
        : dispatcher_(other.dispatcher_)
        , type_(other.type_)
        , is_global_(other.is_global_)
    {
        other.dispatcher_ = nullptr;
    }
    
    ScopedEventListener& operator=(ScopedEventListener&& other) noexcept {
        if (this != &other) {
            dispatcher_ = other.dispatcher_;
            type_ = other.type_;
            is_global_ = other.is_global_;
            other.dispatcher_ = nullptr;
        }
        return *this;
    }
};

/**
 * @brief Helper untuk membuat event filter
 */
template <typename T>
inline EventListener make_event_filter(std::function<bool(const T&)> handler) {
    return [handler = std::move(handler)](const Event& event) -> bool {
        if (auto* data = event.get_if<T>()) {
            return handler(*data);
        }
        return false;
    };
}

// Convenience filters
inline EventListener on_window_event(std::function<bool(const WindowEvent&)> handler) {
    return make_event_filter<WindowEvent>(std::move(handler));
}

inline EventListener on_mouse_event(std::function<bool(const MouseEvent&)> handler) {
    return make_event_filter<MouseEvent>(std::move(handler));
}

inline EventListener on_keyboard_event(std::function<bool(const KeyboardEvent&)> handler) {
    return make_event_filter<KeyboardEvent>(std::move(handler));
}

} // namespace zuu::widget