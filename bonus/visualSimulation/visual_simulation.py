import time
import math
from enum import Enum
from common import Common  # Assuming this is the Common module containing checkForError

class VisualSimulation:
    def __init__(self):
        self.map = []
        self.steps = []
        self.robot_location = (0, 0)
        self.docking_station_location = (0, 0)
        self.current_step = Step.Stay
        self.max_battery_steps = 0
        self.battery_steps = 0.0
        self.allowed_number_of_steps = 0
        self.amount_of_dirt = 0
        self.first_run = True
        self.num_steps = 0
        self.dirt_left = 0
        self.status = ""

        self.info_rep_mapping = {
            Elements.Wall: "#",
            Elements.DockingStation: "D",
            Elements.Robot: "R",
            Elements.EmptyWall: " "
        }

        self.dirt_level_mapping = {
            "1": "0",
            "2": "1",
            "3": "2",
            "4": "3",
            "5": "4",
            "6": "5",
            "7": "6",
            "8": "7",
            "9": "8",
        }

        self.step_map = {
            Step.North: (-1, 0),
            Step.East: (0, 1),
            Step.South: (1, 0),
            Step.West: (0, -1),
            Step.Stay: (0, 0)
        }

        self.direction_string_map = {
            Step.East: ">",
            Step.West: "<",
            Step.South: "v",
            Step.North: "^",
            Step.Stay: "-"
        }

        self.string_to_code_color = {
            "Red": "\033[31m",
            "Green": "\033[32m",
            "Yellow": "\033[33m",
            "Blue": "\033[34m",
            "White": "\033[37m",
            "Reset": "\033[0m"
        }

        self.element_to_code_color = {
            Elements.Robot: "\033[0m",
            Elements.DockingStation: "\033[0m",
            Elements.Direction: "\033[0m",
            Elements.Wall: "\033[0m"
        }

    def process_files(self, input_file_name, output_file_name):
        self.process_input_file(input_file_name)
        self.process_output_file(output_file_name)

    def process_input_file(self, file_name):
        house = House(file_name)
        self.load_parameters(house)
        self.load_house_mapping(house)

    def process_output_file(self, file_name):
        with open(file_name, 'r') as file:
            lines = file.readlines()

        Common.check_for_error(len(lines) < 3, "Error: insufficient lines in output file")

        self.num_steps = int(lines[0].split('=')[1].strip())
        self.dirt_left = int(lines[1].split('=')[1].strip())
        self.status = lines[2].split('=')[1].strip()

        if self.num_steps == 0:
            return

        step_line = lines[3].strip()
        for c in step_line:
            try:
                self.steps.append(self.char_to_step(c))
            except ValueError as e:
                raise RuntimeError(f"Invalid direction string: {c}")

    def char_to_step(self, c):
        if c == 'N':
            return Step.North
        if c == 'E':
            return Step.East
        if c == 'S':
            return Step.South
        if c == 'W':
            return Step.West
        if c == 's':
            return Step.Stay
        if c == 'F':
            return Step.Finish
        raise ValueError(f"Invalid step character: {c}")

    def load_parameters(self, house):
        self.max_battery_steps = house.get_max_battery()
        self.battery_steps = self.max_battery_steps
        self.allowed_number_of_steps = house.get_max_steps()
        self.amount_of_dirt = house.get_amount_of_dirt()
        self.docking_station_location = house.get_docking_station_location()
        self.robot_location = self.docking_station_location

    def int_to_house_element_string(self, num):
        if num == Elements.DockingStation.value:
            return self.info_rep_mapping[Elements.DockingStation]
        if num == Elements.Wall.value:
            return self.info_rep_mapping[Elements.Wall]
        return str(num)

    def load_house_mapping(self, house):
        house_surface = house.get_house_surface()
        for row in house_surface:
            temp = [self.int_to_house_element_string(num) for num in row]
            self.map.append(temp)

    def print_house(self, print_dir):
        if not self.first_run:
            self.clear_map_area(len(self.map) + 3)

        for i, row in enumerate(self.map):
            for j, cell in enumerate(row):
                if (i, j) == self.robot_location:
                    if print_dir:
                        print(f"{self.element_to_code_color[Elements.Direction]} {self.direction_string_map[self.current_step]} {self.string_to_code_color['Reset']}", end="")
                    else:
                        print(f"{self.element_to_code_color[Elements.Robot]} {self.info_rep_mapping[Elements.Robot]} {self.string_to_code_color['Reset']}", end="")
                else:
                    if cell == self.info_rep_mapping[Elements.Wall]:
                        print(f"{self.element_to_code_color[Elements.Wall]} {cell} {self.string_to_code_color['Reset']}", end="")
                    elif cell == self.info_rep_mapping[Elements.DockingStation]:
                        print(f"{self.element_to_code_color[Elements.DockingStation]} {cell} {self.string_to_code_color['Reset']}", end="")
                    else:
                        print(f" {cell} ", end="")
            print()
        print(f"Left steps in battery: {math.floor(self.battery_steps)}, out of {self.max_battery_steps}")
        print(f"Allowed number of steps: {self.allowed_number_of_steps}")
        print()

    def update_map(self):
        dir_elements = self.step_map[self.current_step]
        self.robot_location = (self.robot_location[0] + dir_elements[0], self.robot_location[1] + dir_elements[1])

        if self.current_step == Step.Stay:
            if self.robot_location == self.docking_station_location:
                self.charge()
            else:
                self.battery_steps -= 1
                self.clean()
        else:
            self.battery_steps -= 1

        self.allowed_number_of_steps -= 1

    def start_simulation(self):
        self.first_run = True
        print("\nSIMULATION STARTS ...\n")
        self.print_house(False)
        self.first_run = False

        print("\033[?25l")  # Hide the cursor
        for step in self.steps:
            self.current_step = step
            if self.current_step == Step.Finish:
                break

            self.print_house(True)
            time.sleep(1)

            self.update_map()
            self.print_house(False)
            time.sleep(1)
        print("\nSIMULATION ENDED\n")
        print("\033[?25h")  # Show the cursor

    def charge(self):
        self.battery_steps += (self.max_battery_steps / 20)
        if self.battery_steps > self.max_battery_steps:
            self.battery_steps = self.max_battery_steps

    def clean(self):
        current_dirt = self.map[self.robot_location[0]][self.robot_location[1]]
        if current_dirt in self.dirt_level_mapping:
            self.map[self.robot_location[0]][self.robot_location[1]] = self.dirt_level_mapping[current_dirt]
        else:
            raise RuntimeError(f"Invalid current dirt: {current_dirt}")

    def clear_map_area(self, height):
        print(f"\033[{height}A", end="")
        for _ in range(height):
            print("\033[2K\r", end="")  # Clear the line

    def change_info_rep_mapping(self, config_m):
        self.info_rep_mapping = {
            Elements.Wall: config_m.get_wall_rep(),
            Elements.DockingStation: config_m.get_station_rep(),
            Elements.Robot: config_m.get_robot_rep(),
            Elements.EmptyWall: " "
        }

    def get_color_code(self, color_str):
        return self.string_to_code_color.get(color_str, self.string_to_code_color["White"])

    def change_color_mapping(self, config_m):
        string_to_string = config_m.get_color_mapping()
        self.element_to_code_color = {
            Elements.Robot: self.get_color_code(string_to_string[Elements.Robot]),
            Elements.DockingStation: self.get_color_code(string_to_string[Elements.DockingStation]),
            Elements.Direction: self.get_color_code(string_to_string[Elements.Direction]),
            Elements.Wall: self.get_color_code(string_to_string[Elements.Wall])
        }
