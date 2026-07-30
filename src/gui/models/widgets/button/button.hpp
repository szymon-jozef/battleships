#include "../widget.hpp"
#include <functional>
#include <raylib.h>

namespace battleship {
namespace gui {

class Button : public Widget {
  std::function<void()> onClick;

public:
  Button(const std::string &label, float pos_y, Rectangle rect);

  void draw() override;
  void update() override;

  void setOnClick(std::function<void()> onClickFunc);
};

} // namespace gui
} // namespace battleship
