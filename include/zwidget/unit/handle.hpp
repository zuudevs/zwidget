#pragma once

#include <Windows.h>
#include <winsvc.h>

#undef min
#undef max

namespace zuu::widget {

	class Window ;

	class WindowHandler {
		friend class Window ;

	private :
		HWND src_ = nullptr ;

		WindowHandler(HWND hwnd) noexcept : src_(hwnd) {}
	
	public :
		constexpr WindowHandler() noexcept = default ;
		constexpr WindowHandler(const WindowHandler&) noexcept = delete ;
		constexpr WindowHandler(WindowHandler&&) noexcept = default ;
		constexpr WindowHandler& operator=(const WindowHandler&) noexcept = delete ;
		constexpr WindowHandler& operator=(WindowHandler&&) noexcept = default ;
		constexpr bool operator==(const WindowHandler&) const noexcept = default ;
		constexpr bool operator!=(const WindowHandler&) const noexcept = default ;

		static inline Window MakeWindow() {

		}
	} ;

} // namespace zuu::widget
