#pragma once
#include <string>

const std::string NOTA_RUNTIME_JS = R"(
<script>
const ClickEvent = 'click';
const HoverEvent = 'mouseenter';
class NotaComponent {
  constructor(el, config) {
    this.el = el;
    this.config = config;
    this.setupEvents();
  }
  setupEvents() {
    if (this.config.states) {
      this.config.states.forEach(state => {
        if (state.when) {
          this.el.addEventListener(state.when, () => this.setState(state.name));
        }
      });
    }
  }
  setState(name) {
    const state = this.config.states.find(s => s.name === name);
    if (state) {
      // Apply styles from state
      // Prototype: supports simple color/width/height mapping
      for (const [key, val] of Object.entries(state)) {
        if (key === 'type' || key === 'name' || key === 'when') continue;
        // Simple mapping
        if (key === 'color') this.el.style.backgroundColor = val;
        else this.el.style[key] = val;
      }
    }
  }
}
const nota_elements = {};
</script>
)";
