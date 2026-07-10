#include "../widget.hpp"
#include <functional>

namespace battleship {
namespace gui {

class Button : public Widget {
  std::function<void()> onClick;

public:
  Button(std::string label, float pos_y, Rectangle rect);

  void draw();
  void update();

  void setOnClick(std::function<void()> onClick);
};

} // namespace gui
} // namespace battleship
