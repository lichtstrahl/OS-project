import time

from action_buffer import buffer_to_string, execute_buffer
from action_set import Letters, CapitalLetters, Digits, Symbols, Controls
from action_set_cycle import ActionSetCycle

DATA_PATH = '/proc/mouseListener/info'


class Model:
    action_set_cycle = ActionSetCycle([Letters(), CapitalLetters(), Digits(), Symbols(), Controls()])
    current_action_set = action_set_cycle.selected_set()

    def print_status(self):
        print()
        print('Action:', self.current_action_set.selected_action().name)
        print('Buffer:', buffer_to_string())
        print('Set:', self.current_action_set.name)

    def select_action(self):
        self.current_action_set.selected_action().select()

    def next_set(self):
        self.current_action_set = self.action_set_cycle.next_set()

    def previous_action(self):
        self.current_action_set.previous_action()

    def next_action(self):
        self.current_action_set.next_action()


def read_data():
    with open(DATA_PATH) as f:
        return int(f.readline()), f.readline().strip()


model = Model()
button_to_command = {
    'LEFT': model.select_action,
    'RIGHT': model.next_set,
    'MIDDLE': execute_buffer,
    'WHEELUP': model.previous_action,
    'WHEELDOWN': model.next_action
}
last_id = read_data()[0]
model.print_status()
while True:
    current_id, button = read_data()
    if current_id != last_id:
        last_id = current_id
        button_to_command[button]()
        model.print_status()
    time.sleep(0.1)
