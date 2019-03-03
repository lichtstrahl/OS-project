class ActionSetCycle:
    pos = 0

    def __init__(self, sets):
        self.sets = sets

    def selected_set(self):
        return self.sets[self.pos]

    def next_set(self):
        self.pos += 1
        if self.pos >= len(self.sets):
            self.pos = 0
        return self.selected_set()
