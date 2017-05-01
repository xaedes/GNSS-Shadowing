#pragma once

namespace gnssShadowing {
namespace gui {

		// forward declaration
	class App;

	class KeyboardControl
	{
	public:
		KeyboardControl(App& app);
		~KeyboardControl();

		void update();
	protected:
		App& m_app;
	};

} // namespace gui
} // namespace gnssShadowing
