#pragma once

#include "keymod.hpp"
#include "zwidget/unit/point.hpp"

namespace zuu::widget {

	enum class mouse_state : uint8_t {
		none,
		move,
		enter,
		leave,
		press,
		release,
		double_click,
		scroll
	};

	enum class mouse_button : uint8_t {
		none,
		left,
		middle,
		right,
		x1,     // Additional mouse button 1
		x2      // Additional mouse button 2
	};

	class MouseEvent {
	public :
		Pointf position{} ;
		int scroll_delta = 0 ;
		mouse_state state = mouse_state::none ;
		mouse_button button = mouse_button::none ;
		key_modifier modifiers = key_modifier::none ;

	private :
		uint8_t _padding = 0;  // Explicit padding
		
	public :
		constexpr MouseEvent() noexcept = default;
		constexpr MouseEvent(mouse_state s, const Pointf& pos) noexcept
		 : position(pos), state(s) {}
		constexpr MouseEvent(
			mouse_state s, 
			mouse_button btn, 
			const Pointf& pos
		) noexcept : position(pos), state(s), button(btn) {}
	};

} // namespace zuu::widget
