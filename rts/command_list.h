#pragma once

#include <deque>

#include "command.h"

class CommandList {
  public:
    size_t size() const { return commands.size(); }

    void clear() { commands.clear(); }

    void add(Command::ptr command) { commands.push_back(command); }

    void process(UserId user_id) {
        for (Command::ptr command : commands) {
            if (command->userId == user_id)
                command->process();
        }
    }

    void write(outstream &stream) {
        stream.write(commands.size());
        for (Command::ptr command : commands)
            command->write(stream);
    }

    void read(instream &stream) {
        size_t readCount = 0;
        stream.read(readCount);
        for (int i = 0; i < readCount; ++i) {
            commands.emplace_back(Command::create(stream));
        }
    }

  private:
    std::deque<Command::ptr> commands;
};