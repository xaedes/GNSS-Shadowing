#pragma once

#include <unordered_map>

#include <QKeyEvent>

namespace gnssShadowing {
namespace gui {


/// \brief      Keyboard class: Tracks the down-state of each key.
class Keyboard
{
public:
	Keyboard();
	~Keyboard();

	bool isKeyDown(int key);
	bool isKeyUp(int key);

	// todo call or connect
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

protected:
	std::unordered_map<int, bool> m_isKeyDown;
};

} // namespace gui
} // namespace gnssShadowing
