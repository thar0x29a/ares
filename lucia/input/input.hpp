enum : uint { BindingLimit = 3 };

struct InputMapping {
  enum class Qualifier : uint { None, Lo, Hi, Rumble };

  InputMapping(const string& name) : name(name) {}

  auto bind() -> void;
  auto bind(uint binding, string assignment) -> void;
  auto bind(uint binding, shared_pointer<HID::Device>, uint groupID, uint inputID, int16_t oldValue, int16_t newValue) -> bool;
  auto unbind() -> void;
  auto unbind(uint binding) -> void;
  auto value() -> int16_t;

  const string name;
  string assignments[BindingLimit];

  struct Binding {
    auto icon() -> image;
    auto text() -> string;

    shared_pointer<HID::Device> device;
    uint64_t deviceID;
    uint groupID;
    uint inputID;
    Qualifier qualifier = Qualifier::None;
  };
  Binding bindings[BindingLimit];
};

struct InputButton : InputMapping {
  using InputMapping::InputMapping;
};

struct InputHotkey : InputMapping {
  using InputMapping::InputMapping;
  auto& onPress(function<void ()> press) { return this->press = press, *this; }
  auto& onRelease(function<void ()> release) { return this->release = release, *this; }

private:
  function<void ()> press;
  function<void ()> release;
  int16_t state = 0;
  friend class InputManager;
};

struct VirtualPad {
  VirtualPad();

  InputButton up{"Up"}, down{"Down"}, left{"Left"}, right{"Right"};
  InputButton select{"Select"}, start{"Start"};
  InputButton a{"A"}, b{"B"};
  InputButton x{"X"}, y{"Y"};
  InputButton l{"L"}, r{"R"};

  vector<InputMapping*> mappings;
};

struct InputManager {
  auto create() -> void;
  auto bind() -> void;
  auto poll(bool force = false) -> void;
  auto eventInput(shared_pointer<HID::Device>, uint groupID, uint inputID, int16_t oldValue, int16_t newValue) -> void;

  //hotkeys.cpp
  auto createHotkeys() -> void;
  auto pollHotkeys() -> void;

  vector<shared_pointer<HID::Device>> devices;
  vector<InputHotkey> hotkeys;

  uint64_t pollFrequency = 5;
  uint64_t lastPoll = 0;
};

extern VirtualPad virtualPad;
extern InputManager inputManager;