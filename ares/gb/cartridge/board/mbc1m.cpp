//MBC1 with mapper bits repurposed for supporting multi-game cartridges
struct MBC1M : Interface {
  using Interface::Interface;
  Memory::Readable<uint8> rom;
  Memory::Writable<uint8> ram;

  auto load(Markup::Node document) -> void override {
    auto board = document["game/board"];
    Interface::load(rom, board["memory(type=ROM,content=Program)"]);
    Interface::load(ram, board["memory(type=RAM,content=Save)"]);
  }

  auto save(Markup::Node document) -> void override {
    auto board = document["game/board"];
    Interface::save(ram, board["memory(type=RAM,content=Save)"]);
  }

  auto unload() -> void override {
  }

  auto read(uint16 address, uint8 data) -> uint8 override {
    if(address >= 0x0000 && address <= 0x3fff) {
      if(io.mode == 0) return rom.read((uint14)address);
      return rom.read(io.rom.bank.bit(4,5) << 18 | (uint14)address);
    }

    if(address >= 0x4000 && address <= 0x7fff) {
      return rom.read(io.rom.bank << 14 | (uint14)address);
    }

    if(address >= 0xa000 && address <= 0xbfff) {
      if(!ram) return data;
      return ram.read((uint13)address);
    }

    return data;
  }

  auto write(uint16 address, uint8 data) -> void override {
    if(address >= 0x2000 && address <= 0x3fff) {
      io.rom.bank.bit(0,3) = data.bit(0,3);
    }

    if(address >= 0x4000 && address <= 0x5fff) {
      io.rom.bank.bit(4,5) = data.bit(0,1);
    }

    if(address >= 0x6000 && address <= 0x7fff) {
      io.mode = data.bit(0);
    }

    if(address >= 0xa000 && address <= 0xbfff) {
      if(!ram) return;
      ram.write((uint14)address, data);
    }
  }

  auto power() -> void override {
    io = {};
  }

  auto serialize(serializer& s) -> void override {
    s(ram);
    s(io.mode);
    s(io.rom.bank);
  }

  struct IO {
    uint1 mode;
    struct ROM {
      uint6 bank = 0x01;
    } rom;
  } io;
};