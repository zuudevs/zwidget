#pragma once

#include "zwidget/unit/size.hpp"

namespace zuu::widget {

	enum class window_state : uint8_t {
		none,
		quit,
		close,
		minimize,
		maximize,
		resize,
		restore,
		hide,
		show,
		focus_gained,
		focus_lost
	} ;

	class WindowEvent {
	public :
		Size size{} ;
		window_state state = window_state::none ;

	private :
		uint8_t _padding[3] = {} ;

	public :
		constexpr WindowEvent() noexcept = default;
		constexpr WindowEvent(window_state s) noexcept : state(s) {}
		constexpr WindowEvent(window_state s, const Size& size_) noexcept
		 : size(size_), state(s) {}
	};

} // namespace zuu::widget
