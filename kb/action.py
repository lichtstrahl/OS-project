import uinput

from action_buffer import add_action
from device import click, combo


class Action:
    def __init__(self, name):
        self.name = name

    def select(self):
        add_action(self)

    def execute(self):
        pass


class Click(Action):
    def __init__(self, name, key):
        super().__init__(name)
        self.key = key

    def execute(self):
        click(self.key)


class Combo(Action):
    def __init__(self, name, keys):
        super().__init__(name)
        self.keys = keys

    def execute(self):
        combo(self.keys)


class Shift(Combo):
    def __init__(self, name, key):
        super().__init__(name, [uinput.KEY_LEFTSHIFT, key])


class Control(Combo):
    def __init__(self, name, key):
        super().__init__(name, [uinput.KEY_LEFTCTRL, key])
