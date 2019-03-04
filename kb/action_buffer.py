actions = []


def add_action(action):
    actions.append(action)


def execute_buffer():
    for action in actions:
        print(action)
        action.execute()
    actions.clear()


def buffer_to_string():
    result = ''
    for action in actions:
        result += action.name
    return result
