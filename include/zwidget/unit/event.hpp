#pragma once

#include "generic.hpp"
#include <Windows.h>
#include <objidl.h>

#define NOMINMAX

#include "rect.hpp"

namespace zuu::widget {

	enum class events : uint8_t {
		none,
		window,
		mouse,
		keyboard
	} ;

namespace eventstate {

enum class window : uint8_t {
	none,
	quit,
	close,
	minimize,
	maximize,
	resize,
	hide,
	show
} ;

enum class mouse : uint8_t {
	none,
	hover,
	press,
	release,
	scroll
} ;

enum class mouseclick : uint8_t {
	none,
	left,
	middle,
	right,
	unknown // for additional gaming mouse
} ;

enum class keyboard : uint8_t {
	none,
	press,
	release
} ;

} // namespace eventstate

	class Event {
	private :
		struct empty {} ;

		HWND handle = nullptr ;
		events event_type = events::none ;
	} ;

} // namespace zuu::widget
